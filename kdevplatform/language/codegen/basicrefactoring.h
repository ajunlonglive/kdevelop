/*
    SPDX-FileCopyrightText: 2014 Miquel Sabaté <mikisabate@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BASICREFACTORING_H_
#define BASICREFACTORING_H_

#include <QObject>
#include <QSharedPointer>
#include <language/languageexport.h>
#include <language/codegen/documentchangeset.h>
#include <language/duchain/navigation/useswidget.h>

class CppLanguageSupport;

namespace KDevelop {
class ContextMenuExtension;
class IndexedDeclaration;
class Context;
class Declaration;
class DUContext;

/**
 * A widget that show the uses that it has collected for
 * the given declaration.
 */
class KDEVPLATFORMLANGUAGE_EXPORT BasicRefactoringCollector
    : public UsesWidget::UsesWidgetCollector
{
    Q_OBJECT

public:
    explicit BasicRefactoringCollector(const IndexedDeclaration& decl);
    QVector<IndexedTopDUContext> allUsingContexts() const;

protected:
    /// Process the uses for the given TopDUContext.
    void processUses(KDevelop::ReferencedTopDUContext topContext) override;

private:
    QVector<IndexedTopDUContext> m_allUsingContexts;
};

/// The base class for Refactoring classes from Language plugins.
class KDEVPLATFORMLANGUAGE_EXPORT BasicRefactoring
    : public QObject
{
    Q_OBJECT

public:
    explicit BasicRefactoring(QObject* parent = nullptr);

    /// Update the context menu with the "Rename" action.
    virtual void fillContextMenu(KDevelop::ContextMenuExtension& extension, KDevelop::Context* context,
                                 QWidget* parent);

    struct NameAndCollector
    {
        QString newName;
        QSharedPointer<BasicRefactoringCollector> collector;
    };

    /**
     * @return Suggestion for new filename based on the current file's name @p current and new identifer @p newName
     */
    virtual QString newFileName(const QUrl& current, const QString& newName);

    /**
     * Add the change(s) related to renaming @p file to @p newName to @p changes and return the result.
     *
     * @param current The URL for the file you want to rename.
     * @param newName The new name of the file *without* the file extension.
     * @param changes The change set to add the rename changes to.
     */
    virtual KDevelop::DocumentChangeSet::ChangeResult addRenameFileChanges(const QUrl& current,
                                                                           const QString& newName,
                                                                           KDevelop::DocumentChangeSet* changes);

    virtual bool shouldRenameUses(Declaration* declaration) const;

    /**
     * @return true if the declaration's file should be renamed if the declaration
     *         was renamed.
     */
    virtual bool shouldRenameFile(KDevelop::Declaration* declaration);

public Q_SLOTS:
    void executeRenameAction();

protected:
    /**
     * Apply the changes to the uses that can be found inside the given
     * context and its children.
     * NOTE: the DUChain must be locked.
     */
    virtual DocumentChangeSet::ChangeResult applyChanges(const QString& oldName, const QString& newName,
                                                         DocumentChangeSet& changes, DUContext* context,
                                                         int usedDeclarationIndex);

    /**
     * Apply the changes to the given declarations.
     * NOTE: the DUChain must be locked.
     */
    virtual DocumentChangeSet::ChangeResult applyChangesToDeclarations(const QString& oldName, const QString& newName,
                                                                       DocumentChangeSet& changes,
                                                                       const QList<IndexedDeclaration>& declarations);

    /**
     * Get the declaration under the current position of the cursor.
     *
     * @param allowUse Set to false if the declarations to be returned
     * cannot come from uses.
     */
    virtual IndexedDeclaration declarationUnderCursor(bool allowUse = true);

    /**
     * Start the renaming of a declaration.
     * This function retrieves the new name for declaration @p decl and if approved renames all instances of it.
     */
    virtual void startInteractiveRename(const KDevelop::IndexedDeclaration& decl);

    /**
     * Asks user to input a new name for @p declaration
     * @return new name or empty string if user changed his mind or new name contains inappropriate symbols (e.g. spaces, points, braces e.t.c) and the collector used for collecting information about @p declaration.
     * NOTE: unlock the DUChain before calling this.
     */
    virtual BasicRefactoring::NameAndCollector newNameForDeclaration(const KDevelop::DeclarationPointer& declaration);

    /**
     * Renames all declarations collected by @p collector from @p oldName to @p newName
     * @param apply - if changes should be applied immediately
     * @return all changes if @p apply is false and empty set otherwise.
     */
    DocumentChangeSet renameCollectedDeclarations(KDevelop::BasicRefactoringCollector* collector,
                                                  const QString& replacementName, const QString& originalName,
                                                  bool apply = true);

    /**
     * @returns true if we can show the interactive rename widget for the
     * given declaration. The default implementation just returns true.
     */
    virtual bool acceptForContextMenu(const Declaration* decl);
};
} // End of namespace KDevelop

#endif /* BASICREFACTORING_H_ */
