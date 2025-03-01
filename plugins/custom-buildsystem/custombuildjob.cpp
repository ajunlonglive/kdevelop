/*
    SPDX-FileCopyrightText: 2010 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-or-later
*/

#include "custombuildjob.h"

#include <KLocalizedString>
#include <KConfigGroup>
#include <KShell>

#include <interfaces/iproject.h>
#include <outputview/outputmodel.h>
#include <outputview/outputdelegate.h>
#include <util/environmentprofilelist.h>
#include <util/commandexecutor.h>
#include <project/projectmodel.h>

#include "custombuildsystemplugin.h"
#include "configconstants.h"

using namespace KDevelop;

CustomBuildJob::CustomBuildJob( CustomBuildSystem* plugin, KDevelop::ProjectBaseItem* item, CustomBuildSystemTool::ActionType t )
    : OutputJob( plugin )
    , type( t )
    , exec(nullptr)
    , killed( false )
    , enabled( false )
{
    setCapabilities( Killable );
    QString subgrpname;
    switch( type ) {
        case CustomBuildSystemTool::Build:
            subgrpname = ConfigConstants::toolGroupPrefix() + QLatin1String("Build");
            break;
        case CustomBuildSystemTool::Clean:
            subgrpname = ConfigConstants::toolGroupPrefix() + QLatin1String("Clean");
            break;
        case CustomBuildSystemTool::Install:
            subgrpname = ConfigConstants::toolGroupPrefix() + QLatin1String("Install");
            break;
        case CustomBuildSystemTool::Configure:
            subgrpname = ConfigConstants::toolGroupPrefix() + QLatin1String("Configure");
            break;
        case CustomBuildSystemTool::Prune:
            subgrpname = ConfigConstants::toolGroupPrefix() + QLatin1String("Prune");
            break;
        case CustomBuildSystemTool::Undefined:
            return;
    }
    projectName = item->project()->name();
    builddir = plugin->buildDirectory( item ).toLocalFile();
    KConfigGroup g = plugin->configuration( item->project() );
    if(g.isValid()) {
        KConfigGroup grp = g.group( subgrpname );
        enabled = grp.readEntry(ConfigConstants::toolEnabled(), false);
        cmd = grp.readEntry(ConfigConstants::toolExecutable(), QUrl()).toLocalFile();
        environment = grp.readEntry(ConfigConstants::toolEnvironment(), QString());
        arguments = grp.readEntry(ConfigConstants::toolArguments(), QString());
    }

    QString title;
    switch (type) {
    case CustomBuildSystemTool::Build:
        title = i18nc("Building: <command> <project item name>", "Building: %1 %2", cmd, item->text());
        break;
    case CustomBuildSystemTool::Clean:
        title = i18nc("Cleaning: <command> <project item name>", "Cleaning: %1 %2", cmd, item->text());
        break;
    case CustomBuildSystemTool::Install:
        title = installPrefix.isEmpty() ? i18nc("Installing: <command> <project item name>", "Installing: %1 %2", cmd, item->text())
              : i18nc("Installing: <command> <project item name> <installPrefix>", "Installing: %1 %2 %3", cmd, item->text(), installPrefix.toDisplayString(QUrl::PreferLocalFile));
        break;
    case CustomBuildSystemTool::Configure:
        title = i18nc("Configuring: <command> <project item name>", "Configuring: %1 %2", cmd, item->text());
        break;
    case CustomBuildSystemTool::Prune:
        title = i18nc("Pruning: <command> <project item name>", "Pruning: %1 %2", cmd, item->text());
        break;
    default:
        title = QStringLiteral("Internal Error: CustomBuildJob");
        break;
    }
    setTitle(title);
    setObjectName(title);
    setDelegate( new KDevelop::OutputDelegate );
}

void CustomBuildJob::start()
{
    if( type == CustomBuildSystemTool::Undefined ) {
        setError( UndefinedBuildType );
        setErrorText( i18n( "Undefined Build type" ) );
        emitResult();
    } else if( cmd.isEmpty() ) {
        setError( NoCommand );
        setErrorText(i18n("No command given for custom %1 tool in project \"%2\".",
            CustomBuildSystemTool::toolName(type), projectName));
        emitResult();
    } else if( !enabled ) {
        setError( ToolDisabled );
        setErrorText(i18n("The custom %1 tool in project \"%2\" is disabled",
            CustomBuildSystemTool::toolName(type), projectName));
        emitResult();
    } else {
        // prepend the command name to the argument string
        // so that splitArgs works correctly
        const QString allargv = KShell::quoteArg(cmd) + QLatin1Char(' ') + arguments;

        KShell::Errors err;
        QStringList strargs = KShell::splitArgs( allargv, KShell::AbortOnMeta, &err );
        if( err != KShell::NoError ) {
            setError( WrongArgs );
            setErrorText( i18n( "The given arguments would need a real shell, this is not supported currently." ) );
            emitResult();
            return;
        }
        // and remove the command name back out of the split argument list
        Q_ASSERT(!strargs.isEmpty());
        strargs.removeFirst();

        setStandardToolView( KDevelop::IOutputView::BuildView );
        setBehaviours( KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll );
        auto* model = new KDevelop::OutputModel( QUrl::fromLocalFile(builddir) );
        model->setFilteringStrategy( KDevelop::OutputModel::CompilerFilter );
        setModel( model );

        startOutput();

        exec = new KDevelop::CommandExecutor( cmd, this );

        auto env = KDevelop::EnvironmentProfileList(KSharedConfig::openConfig()).createEnvironment(environment, QProcess::systemEnvironment());
        if (!installPrefix.isEmpty())
            env.append(QLatin1String("DESTDIR=")+installPrefix.toDisplayString(QUrl::PreferLocalFile));

        exec->setArguments( strargs );
        exec->setEnvironment( env );
        exec->setWorkingDirectory( builddir );

        
        connect( exec, &CommandExecutor::completed, this, &CustomBuildJob::procFinished );
        connect( exec, &CommandExecutor::failed, this, &CustomBuildJob::procError );

        connect( exec, &CommandExecutor::receivedStandardError, model, &OutputModel::appendLines );
        connect( exec, &CommandExecutor::receivedStandardOutput, model, &OutputModel::appendLines );

        model->appendLine(QStringLiteral("%1> %2 %3").arg(builddir, cmd, arguments));
        exec->start();
    }
}

bool CustomBuildJob::doKill()
{
    killed = true;
    exec->kill();
    return true;
}

void CustomBuildJob::procError( QProcess::ProcessError err )
{
    if( !killed ) {
        if( err == QProcess::FailedToStart ) {
            setError( FailedToStart );
            setErrorText( i18n( "Failed to start command." ) );
        } else if( err == QProcess::Crashed ) {
            setError( Crashed );
            setErrorText( i18n( "Command crashed." ) );
        } else {
            setError( UnknownExecError );
            setErrorText( i18n( "Unknown error executing command." ) );
        }
    }
    emitResult();
}

KDevelop::OutputModel* CustomBuildJob::model()
{
    return qobject_cast<KDevelop::OutputModel*>( OutputJob::model() );
}

void CustomBuildJob::procFinished(int code)
{
    //TODO: Make this configurable when the first report comes in from a tool
    //      where non-zero does not indicate error status
    if( code != 0 ) {
        setError( FailedShownError );
        model()->appendLine( i18n( "*** Failed ***" ) );
    } else {
        model()->appendLine( i18n( "*** Finished ***" ) );
    }
    emitResult();
}

