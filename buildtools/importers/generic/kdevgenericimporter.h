/* This file is part of KDevelop
    Copyright (C) 2004,2005 Roberto Raggi <roberto@kdevelop.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef KDEVGENERICIMPORTER_H
#define KDEVGENERICIMPORTER_H

#include <kdevprojecteditor.h>
#include <qstringlist.h>

class QFileInfo;

class KDevGenericImporter: public KDevProjectEditor
{
    Q_OBJECT
public:
    KDevGenericImporter(QObject *parent = 0,
                        const QStringList &args = QStringList());
    virtual ~KDevGenericImporter();

//
// KDevProjectEditor interface
//
    virtual Features features() const
    { return Features(Folders | Files); }

    virtual bool addFolder(KDevProjectFolderItem *folder, KDevProjectFolderItem *parent);
    virtual bool addTarget(KDevProjectTargetItem *target, KDevProjectFolderItem *parent);
    virtual bool addFile(KDevProjectFileItem *file, KDevProjectFolderItem *parent);
    virtual bool addFile(KDevProjectFileItem *file, KDevProjectTargetItem *parent);

    virtual bool removeFolder(KDevProjectFolderItem *folder, KDevProjectFolderItem *parent);
    virtual bool removeTarget(KDevProjectTargetItem *target, KDevProjectFolderItem *parent);
    virtual bool removeFile(KDevProjectFileItem *file, KDevProjectFolderItem *parent);
    virtual bool removeFile(KDevProjectFileItem *file, KDevProjectTargetItem *parent);

    virtual QList<KDevProjectTargetItem*> targets() const;
    virtual FileItemList filesForTarget(KDevProjectTargetItem*) const;

//
// KDevProjectImporter interface
//
    virtual KDevProject *project() const;

    virtual QList<KDevProjectFolderItem*> parse(KDevProjectFolderItem *item);
    virtual KDevProjectItem *import(KDevProjectModel *model, const QString &fileName);
    virtual QString findMakefile(KDevProjectFolderItem *dom) const;
    virtual QStringList findMakefiles(KDevProjectFolderItem *dom) const;

private:
    bool isValid(const QFileInfo &fileName) const;

private:
    KDevProject *m_project;

    QStringList includes;
    QStringList excludes;
};

#endif // KDEVGENERICIMPORTER_H
