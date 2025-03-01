/*
    SPDX-FileCopyrightText: 2007 Alexander Dymo <adymo@kdevelop.org>
    SPDX-FileCopyrightText: 2007 Kris Wong <kris.p.wong@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "core.h"
#include "core_p.h"

#include <QApplication>

#include <KLocalizedString>

#include <language/backgroundparser/backgroundparser.h>
#include <language/duchain/duchain.h>

#include "mainwindow.h"
#include "sessioncontroller.h"
#include "uicontroller.h"
#include "plugincontroller.h"
#include "projectcontroller.h"
#include "partcontroller.h"
#include "languagecontroller.h"
#include "documentcontroller.h"
#include "runcontroller.h"
#include "session.h"
#include "documentationcontroller.h"
#include "sourceformattercontroller.h"
#include "progresswidget/progressmanager.h"
#include "selectioncontroller.h"
#include "debugcontroller.h"
#include "kdevplatform_version.h"
#include "workingsetcontroller.h"
#include "testcontroller.h"
#include "runtimecontroller.h"
#include "debug.h"

#include <csignal>

namespace {
void shutdownGracefully(int sig)
{
    static volatile std::sig_atomic_t handlingSignal = 0;

    if ( !handlingSignal ) {
        handlingSignal = 1;
        qCDebug(SHELL) << "signal " << sig << " received, shutting down gracefully";
        QCoreApplication* app = QCoreApplication::instance();
        if (auto* guiApp = qobject_cast<QApplication*>(app)) {
            guiApp->closeAllWindows();
        }
        app->quit();
        return;
    }

    // re-raise signal with default handler and trigger program termination
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

void installSignalHandler()
{
#ifdef SIGHUP
    std::signal(SIGHUP, shutdownGracefully);
#endif
#ifdef SIGINT
    std::signal(SIGINT, shutdownGracefully);
#endif
#ifdef SIGTERM
    std::signal(SIGTERM, shutdownGracefully);
#endif
}
}

namespace KDevelop {

Core *Core::m_self = nullptr;

CorePrivate::CorePrivate(Core *core)
    : m_core(core)
    , m_cleanedUp(false)
    , m_shuttingDown(false)
{
}

bool CorePrivate::initialize(Core::Setup mode, const QString& session )
{
    m_mode=mode;

    qCDebug(SHELL) << "Creating controllers";

    if( !sessionController )
    {
        sessionController = new SessionController(m_core);
    }
    if( !workingSetController && !(mode & Core::NoUi) )
    {
        workingSetController = new WorkingSetController();
    }
    qCDebug(SHELL) << "Creating ui controller";
    if( !uiController )
    {
        uiController = new UiController(m_core);
    }
    qCDebug(SHELL) << "Creating plugin controller";

    if( !pluginController )
    {
        pluginController = new PluginController(m_core);
        const auto pluginInfos = pluginController->allPluginInfos();
        if (pluginInfos.isEmpty()) {
            QMessageBox::critical(nullptr,
                                  i18nc("@title:window", "No Plugins Found"),
                                  i18n("<p>Could not find any plugins during startup.<br/>"
                                  "Please make sure QT_PLUGIN_PATH is set correctly.</p>"
                                  "Refer to <a href=\"https://community.kde.org/Guidelines_and_HOWTOs/Build_from_source#Set_up_the_runtime_environment\">this article</a> for more information."),
                                  QMessageBox::Abort, QMessageBox::Abort);
            qCWarning(SHELL) << "Could not find any plugins, aborting";
            return false;
        }
    }
    if( !partController && !(mode & Core::NoUi))
    {
        partController = new PartController(m_core, uiController->defaultMainWindow());
    }

    if( !projectController )
    {
        projectController = new ProjectController(m_core);
    }

    if( !documentController )
    {
        documentController = new DocumentController(m_core);
    }

    if( !languageController )
    {
        // Must be initialized after documentController, because the background parser depends
        // on the document controller.
        languageController = new LanguageController(m_core);
    }

    if( !runController )
    {
        runController = new RunController(m_core);
    }

    if( !sourceFormatterController )
    {
        sourceFormatterController = new SourceFormatterController(m_core);
    }

    if ( !progressController)
    {
        progressController = ProgressManager::instance();
    }

    if( !selectionController )
    {
        selectionController = new SelectionController(m_core);
    }

    if( !documentationController && !(mode & Core::NoUi) )
    {
        documentationController = new DocumentationController(m_core);
    }

    if( !runtimeController )
    {
        runtimeController = new RuntimeController(m_core);
    }

    if( !debugController )
    {
        debugController = new DebugController(m_core);
    }

    if( !testController )
    {
        testController = new TestController(m_core);
    }

    qCDebug(SHELL) << "Done creating controllers";

    qCDebug(SHELL) << "Initializing controllers";

    sessionController->initialize( session );
    if( !sessionController->activeSessionLock() ) {
        return false;
    }

    // TODO: Is this early enough, or should we put the loading of the session into
    // the controller construct
    DUChain::initialize();

    if (!(mode & Core::NoUi)) {
        uiController->initialize();
    }
    languageController->initialize();
    languageController->backgroundParser()->suspend();
    // eventually resume the background parser once the project controller
    // has been initialized. At that point we know whether there are projects loading
    // which the background parser is handling internally to defer parse jobs
    QObject::connect(projectController.data(), &ProjectController::initialized,
                     m_core, [this]() {
                         languageController->backgroundParser()->resume();
                     });

    if (partController) {
        partController->initialize();
    }
    projectController->initialize();
    documentController->initialize();

    /* This is somewhat messy.  We want to load the areas before
        loading the plugins, so that when each plugin is loaded we
        know if an area wants some of the tool view from that plugin.
        OTOH, loading of areas creates documents, and some documents
        might require that a plugin is already loaded.
        Probably, the best approach would be to plugins to just add
        tool views to a list of available tool view, and then grab
        those tool views when loading an area.  */

    qCDebug(SHELL) << "Initializing plugin controller (loading session plugins)";
    pluginController->initialize();

    /* To make breakpoints show up in the UI, we need to make sure
       DebugController is initialized and has loaded BreakpointModel
       before UI is made visible. */
    debugController->initialize();

    qCDebug(SHELL) << "Initializing working set controller";
    if(!(mode & Core::NoUi))
    {
        workingSetController->initialize();
        /* Need to do this after everything else is loaded.  It's too
            hard to restore position of views, and toolbars, and whatever
            that are not created yet.  */
        uiController->loadAllAreas(KSharedConfig::openConfig());
        uiController->defaultMainWindow()->show();
    }

    qCDebug(SHELL) << "Initializing remaining controllers";
    runController->initialize();
    sourceFormatterController->initialize();
    selectionController->initialize();
    if (documentationController) {
        documentationController->initialize();
    }
    testController->initialize();
    runtimeController->initialize();

    installSignalHandler();

    qCDebug(SHELL) << "Done initializing controllers";

    return true;
}
CorePrivate::~CorePrivate()
{
    delete selectionController.data();
    delete projectController.data();
    delete languageController.data();
    delete pluginController.data();
    delete uiController.data();
    delete partController.data();
    delete documentController.data();
    delete runController.data();
    delete sessionController.data();
    delete sourceFormatterController.data();
    delete documentationController.data();
    delete debugController.data();
    delete workingSetController.data();
    delete testController.data();
    delete runtimeController.data();
}

bool Core::initialize(Setup mode, const QString& session)
{
    if (m_self)
        return true;

    m_self = new Core();
    bool ret = m_self->d->initialize(mode, session);

    if(ret)
        emit m_self->initializationCompleted();

    return ret;
}

Core *KDevelop::Core::self()
{
    return m_self;
}

Core::Core(QObject *parent)
    : ICore(parent)
{
    d = new CorePrivate(this);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &Core::shutdown);
}

Core::Core(CorePrivate* dd, QObject* parent)
: ICore(parent), d(dd)
{
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &Core::shutdown);
}

Core::~Core()
{
    qCDebug(SHELL);

    //Cleanup already called before mass destruction of GUI
    delete d;
    m_self = nullptr;
}

Core::Setup Core::setupFlags() const
{
    return d->m_mode;
}

void Core::shutdown()
{
    qCDebug(SHELL);

    if (!d->m_shuttingDown) {
        cleanup();
        deleteLater();
    }

    qCDebug(SHELL) << "Shutdown done";
}

bool Core::shuttingDown() const
{
    return d->m_shuttingDown;
}

void Core::cleanup()
{
    qCDebug(SHELL);

    d->m_shuttingDown = true;
    emit aboutToShutdown();

    if (!d->m_cleanedUp) {
        // first of all: request stop of all background parser jobs
        d->languageController->backgroundParser()->abortAllJobs();
        d->languageController->backgroundParser()->suspend();

        d->debugController->cleanup();
        d->selectionController->cleanup();

        if (!(d->m_mode & Core::NoUi)) {
            // Save the layout of the ui here, so run it first
            d->uiController->cleanup();
        }

        if (d->workingSetController)
            d->workingSetController->cleanup();

        /* Must be called before projectController->cleanup(). */
        // Closes all documents (discards, as already saved if the user wished earlier)
        d->documentController->cleanup();
        d->runController->cleanup();
        if (d->partController) {
            d->partController->cleanup();
        }
        d->projectController->cleanup();
        d->sourceFormatterController->cleanup();

        // before unloading language plugins, we need to make sure all parse jobs are done
        d->languageController->backgroundParser()->waitForIdle();

        DUChain::self()->shutdown();

        // Only unload plugins after the DUChain shutdown to prevent issues with non-loaded factories for types
        // See: https://bugs.kde.org/show_bug.cgi?id=379669
        d->pluginController->cleanup();

        d->sessionController->cleanup();

        d->testController->cleanup();

        //Disable the functionality of the language controller
        d->languageController->cleanup();
    }

    d->m_cleanedUp = true;
    emit shutdownCompleted();
}

IUiController *Core::uiController()
{
    return d->uiController.data();
}

ISession* Core::activeSession()
{
    return sessionController()->activeSession();
}

ISessionLock::Ptr Core::activeSessionLock()
{
    return sessionController()->activeSessionLock();
}

SessionController *Core::sessionController()
{
    return d->sessionController.data();
}

UiController *Core::uiControllerInternal()
{
    return d->uiController.data();
}

IPluginController *Core::pluginController()
{
    return d->pluginController.data();
}

PluginController *Core::pluginControllerInternal()
{
    return d->pluginController.data();
}

IProjectController *Core::projectController()
{
    return d->projectController.data();
}

ProjectController *Core::projectControllerInternal()
{
    return d->projectController.data();
}

IPartController *Core::partController()
{
    return d->partController.data();
}

PartController *Core::partControllerInternal()
{
    return d->partController.data();
}

ILanguageController *Core::languageController()
{
    return d->languageController.data();
}

LanguageController *Core::languageControllerInternal()
{
    return d->languageController.data();
}

IDocumentController *Core::documentController()
{
    return d->documentController.data();
}

DocumentController *Core::documentControllerInternal()
{
    return d->documentController.data();
}

IRunController *Core::runController()
{
    return d->runController.data();
}

RunController *Core::runControllerInternal()
{
    return d->runController.data();
}

ISourceFormatterController* Core::sourceFormatterController()
{
    return d->sourceFormatterController.data();
}

SourceFormatterController* Core::sourceFormatterControllerInternal()
{
    return d->sourceFormatterController.data();
}


ProgressManager *Core::progressController()
{
    return d->progressController.data();
}

ISelectionController* Core::selectionController()
{
    return d->selectionController.data();
}

IDocumentationController* Core::documentationController()
{
    return d->documentationController.data();
}

DocumentationController* Core::documentationControllerInternal()
{
    return d->documentationController.data();
}

IRuntimeController* Core::runtimeController()
{
    return d->runtimeController.data();
}

RuntimeController* Core::runtimeControllerInternal()
{
    return d->runtimeController.data();
}

IDebugController* Core::debugController()
{
    return d->debugController.data();
}

DebugController* Core::debugControllerInternal()
{
    return d->debugController.data();
}

ITestController* Core::testController()
{
    return d->testController.data();
}

TestController* Core::testControllerInternal()
{
    return d->testController.data();
}

WorkingSetController* Core::workingSetControllerInternal()
{
    return d->workingSetController.data();
}

QString Core::version()
{
    return QStringLiteral(KDEVPLATFORM_VERSION_STRING);
}

}

