/*
    SPDX-FileCopyrightText: 2013 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CMAKECOMMITCHANGESJOB_H
#define CMAKECOMMITCHANGESJOB_H

#include <KJob>
#include <cmaketypes.h>
#include <util/path.h>

namespace KDevelop {
    class IProject;
    class ProjectTargetItem;
    class ProjectFolderItem;
}
class CMakeManager;
class CMakeFolderItem;
struct CMakeProjectData;

struct ProcessedTarget
{
    Target target;
    QStringList includes;
    QStringList defines;
    QString outputName;
    KDevelop::Path location;
};
Q_DECLARE_TYPEINFO(ProcessedTarget, Q_MOVABLE_TYPE);

class CMakeCommitChangesJob : public KJob
{
Q_OBJECT
public:
    explicit CMakeCommitChangesJob(const KDevelop::Path& url, CMakeManager* manager, KDevelop::IProject* project);

    KDevelop::Path::List addProjectData(const CMakeProjectData& data);
    void setFindParentItem(bool find);
    virtual void start();

public Q_SLOTS:
    void reloadFiles();
    void folderAvailable(KDevelop::ProjectFolderItem* item);

Q_SIGNALS:
    void folderCreated(KDevelop::ProjectFolderItem* item);

private Q_SLOTS:
    void makeChanges();

private:
    void reloadFiles(KDevelop::ProjectFolderItem* item);
    void setTargetFiles(KDevelop::ProjectTargetItem* target, const KDevelop::Path::List& files);

    KDevelop::Path m_path;
    QVector<Subdirectory> m_subdirectories;
    QVector<ProcessedTarget> m_targets;
    QVector<CMakeTest> m_tests;
    KDevelop::IProject* m_project;
    CMakeManager* m_manager;

    QStringList m_directories;
    CMakeDefinitions m_definitions;
    bool m_projectDataAdded;
    KDevelop::ProjectFolderItem* m_parentItem;
    bool m_waiting;
    bool m_findParent;
};

#endif // CMAKECOMMITCHANGESJOB_H
