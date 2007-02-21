/***************************************************************************
 *   Copyright (C) 2006-2007 by Alexander Dymo                             *
 *   adymo@kdevelop.org                                                    *
 *   Copyright (C) 2006 by Andreas Pakulat                                 *
 *   apaku@gmx.de                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "filemanager.h"

#include <QDir>
#include <QDirModel>
#include <QLayout>
#include <QTreeView>
#include <QTimer>
#include <QThread>

#include <kurl.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdirlister.h>
#include <ktoolbar.h>
#include <kaction.h>
#include <kstandardaction.h>
#include <kinputdialog.h>
#include <kurlcombobox.h>
#include <kurlcompletion.h>
#include <kio/netaccess.h>

#include "icore.h"
#include "iuicontroller.h"

#include "kdevfilemanagerpart.h"
#include "drilldownview.h"
#include "kdevdirmodel.h"

class FileManagerPrivate
{
public:

    FileManager *m_manager;
    KDevDirModel *m_model;
    DrillDownView *m_view;
    KToolBar *m_toolBar;
    KUrlComboBox *m_urlBox;
    KDevFileManagerPart *m_part;

    FileManagerPrivate(FileManager *manager): m_manager(manager) {}

    void open(const QModelIndex &index)
    {
        if (!index.isValid())
            return;

        KFileItem *fileItem = m_model->itemForIndex(index);
        if (!fileItem)
            return;

        if (fileItem->isFile())
            openFile(fileItem);
        else if (fileItem->isDir())
            m_view->slideRight();
    }

    void openFile(KFileItem *fileItem)
    {
        if (!fileItem)
            return;

        m_part->core()->uiController()->openUrl(fileItem->url());
    }

    void init()
    {
        goHome();
    }

    void goUp()
    {
        if (m_view->isBusy())
            return;
        m_model->goUp();
    }

    void goHome()
    {
        goToUrl(KUrl::fromPath(QDir::homePath()));
    }

    void goLeft()
    {
        if (m_view->isBusy())
            return;
        m_view->slideLeft();
    }

    void goRight()
    {
        if (m_view->isBusy())
            return;
        m_view->slideRight();
    }

    void goToUrl(const KUrl &url)
    {
        if (m_view->isBusy())
            return;
        m_model->dirLister()->openUrl(url, false, true);
    }

    void goToUrl(const QString &url)
    {
        kDebug() << k_funcinfo << endl;
        goToUrl(KUrl(url));
        m_view->setFocus();
    }

    void newFolder()
    {
        KUrl url = m_model->dirLister()->url();
        bool ok;
        QString targetDir = url.url();
        if (targetDir.startsWith("file:///"))
            targetDir = targetDir.mid(7, 1000);
        QString path = KInputDialog::getText(i18n("New Folder"),
            i18n("Create new folder in:\n")+targetDir, "New Folder", &ok, m_view);
        if (ok && !path.isEmpty())
        {
            url.addPath(path);
            KIO::NetAccess::mkdir(url, m_view);
            m_model->dirLister()->openUrl(url);
        }
    }
};

class ToolBarParent: public QWidget {
public:
    ToolBarParent(QWidget *parent): QWidget(parent), m_toolBar(0) {}
    void setToolBar(QToolBar *toolBar)
    {
        m_toolBar = toolBar;
    }

    virtual void resizeEvent(QResizeEvent *ev)
    {
        if (!m_toolBar)
            return QWidget::resizeEvent(ev);
        setMinimumHeight(m_toolBar->sizeHint().height());
        m_toolBar->resize(width(), height());
    }

private:
    QToolBar *m_toolBar;
};


FileManager::FileManager(KDevFileManagerPart *part, QWidget* parent)
    :QWidget(parent), d(new FileManagerPrivate(this))
{
    d->m_part = part;
    setObjectName("FileManager");
//     setWindowIcon(SmallIcon("kdevelop"));
    setWindowTitle(i18n("File Manager"));
    setWhatsThis(i18n("File Manager"));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    ToolBarParent *tbp = new ToolBarParent(this);
    l->addWidget(tbp);

    d->m_toolBar = new KToolBar(tbp);
    d->m_toolBar->setMovable(false);
    d->m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    d->m_toolBar->setIconSize(QSize(16,16));
    d->m_toolBar->setContextMenuEnabled(false);
    tbp->setToolBar(d->m_toolBar);

    d->m_urlBox = new KUrlComboBox(KUrlComboBox::Directories, true, this);
    KUrlCompletion *cmpl = new KUrlCompletion(KUrlCompletion::DirCompletion);
    d->m_urlBox->setCompletionObject(cmpl);
    d->m_urlBox->setFrame(false);
    d->m_urlBox->setInsertPolicy(QComboBox::InsertAtBottom);
    l->addWidget(d->m_urlBox);
    l->addSpacing(2);
    connect(d->m_urlBox, SIGNAL(urlActivated(const KUrl&)),
        this, SLOT(goToUrl(const KUrl&)));
    connect(d->m_urlBox, SIGNAL(returnPressed(const QString&)),
        this, SLOT(goToUrl(const QString&)));

    d->m_view = new DrillDownView(this);
    l->addWidget(d->m_view);

    d->m_model = new KDevDirModel(d->m_view);
    d->m_view->setModel(d->m_model);

    connect(d->m_model->dirLister(), SIGNAL(completed()),
        d->m_view, SLOT(animateNewUrl()));

    connect(d->m_view, SIGNAL(doubleClicked(const QModelIndex &)),
        this, SLOT(open(const QModelIndex &)));
    connect(d->m_view, SIGNAL(returnPressed(const QModelIndex &)),
        this, SLOT(open(const QModelIndex &)));
    connect(d->m_view, SIGNAL(tryToSlideLeft()),
        this, SLOT(goUp()));

    setupActions();

    QTimer::singleShot(0, this, SLOT(init()));
}

void FileManager::setupActions()
{
    KAction *action = KStandardAction::up(this, SLOT(goLeft()), this);
    d->m_toolBar->addAction(action);

    action = KStandardAction::home(this, SLOT(goHome()), this);
    d->m_toolBar->addAction(action);

    action = new KAction(this);
    action->setIcon(KIcon("folder_new"));
    action->setText(i18n("New Folder..."));
    action->setToolTip(i18n("New Folder"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(newFolder()));
    d->m_toolBar->addAction(action);
}

#include "filemanager.moc"
//kate: space-indent on; indent-width 4; replace-tabs on; auto-insert-doxygen on;
