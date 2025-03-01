/*
    SPDX-FileCopyrightText: 2006-2008 Hamish Rodda <rodda@kde.org>
    SPDX-FileCopyrightText: 2007-2008 David Nolden <david.nolden.kdevelop@art-master.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "codecompletionworker.h"

#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KLocalizedString>

#include "../duchain/ducontext.h"
#include "../duchain/duchainlock.h"
#include "../duchain/duchain.h"
#include <debug.h>
#include "codecompletion.h"
#include "codecompletionitem.h"
#include "codecompletionmodel.h"
#include "codecompletionitemgrouper.h"
#include <util/foregroundlock.h>

using namespace KTextEditor;
using namespace KDevelop;

CodeCompletionWorker::CodeCompletionWorker(KDevelop::CodeCompletionModel* model) :
    m_hasFoundDeclarations(false)
    , m_mutex(new QMutex())
    , m_abort(false)
    , m_fullCompletion(true)
    , m_model(model)
{
}

CodeCompletionWorker::~CodeCompletionWorker()
{
    delete m_mutex;
}

void CodeCompletionWorker::setFullCompletion(bool full)
{
    m_fullCompletion = full;
}

bool CodeCompletionWorker::fullCompletion() const
{
    return m_fullCompletion;
}

void CodeCompletionWorker::failed()
{
    foundDeclarations({}, {});
}

void CodeCompletionWorker::foundDeclarations(const QList<CompletionTreeElementPointer>& items,
                                             const CodeCompletionContext::Ptr& completionContext)
{
    m_hasFoundDeclarations = true;
    emit foundDeclarationsReal(items, completionContext);
}

void CodeCompletionWorker::computeCompletions(const KDevelop::DUContextPointer& context,
                                              const KTextEditor::Cursor& position, KTextEditor::View* view)
{
    {
        QMutexLocker lock(m_mutex);
        m_abort = false;
    }

    ///@todo It's not entirely safe to pass KTextEditor::View* through a queued connection
    // We access the view/document which is not thread-safe, so we need the foreground lock
    ForegroundLock foreground;

    //Compute the text we should complete on
    KTextEditor::Document* doc = view->document();
    if (!doc) {
        qCDebug(LANGUAGE) << "No document for completion";
        failed();
        return;
    }

    KTextEditor::Range range;
    QString text;
    {
        QMutexLocker lock(m_mutex);
        DUChainReadLocker lockDUChain;

        if (context) {
            qCDebug(LANGUAGE) << context->localScopeIdentifier().toString();
            range = KTextEditor::Range(context->rangeInCurrentRevision().start(), position);
        } else
            range = KTextEditor::Range(KTextEditor::Cursor(position.line(), 0), position);

        updateContextRange(range, view, context);

        text = doc->text(range);
    }

    if (position.column() == 0) //Seems like when the cursor is a the beginning of a line, kate does not give the \n
        text += QLatin1Char('\n');

    if (aborting()) {
        failed();
        return;
    }
    m_hasFoundDeclarations = false;

    KTextEditor::Cursor cursorPosition = view->cursorPosition();
    QString followingText; //followingText may contain additional text that stands for the current item. For example in the case "QString|", QString is in addedText.
    if (position < cursorPosition)
        followingText = view->document()->text(KTextEditor::Range(position, cursorPosition));

    foreground.unlock();

    computeCompletions(context, position, followingText, range, text);

    if (!m_hasFoundDeclarations)
        failed();
}

void KDevelop::CodeCompletionWorker::doSpecialProcessing(uint)
{
}

CodeCompletionContext* CodeCompletionWorker::createCompletionContext(const KDevelop::DUContextPointer& context,
                                                                     const QString& contextText,
                                                                     const QString& followingText,
                                                                     const KDevelop::CursorInRevision& position) const
{
    Q_UNUSED(context);
    Q_UNUSED(contextText);
    Q_UNUSED(followingText);
    Q_UNUSED(position);
    return nullptr;
}

void CodeCompletionWorker::computeCompletions(const KDevelop::DUContextPointer& context,
                                              const KTextEditor::Cursor& position, const QString& followingText,
                                              const KTextEditor::Range& contextRange, const QString& contextText)
{
    Q_UNUSED(contextRange);

    qCDebug(LANGUAGE) << "added text:" << followingText;

    CodeCompletionContext::Ptr completionContext(createCompletionContext(context, contextText, followingText, CursorInRevision::castFromSimpleCursor(KTextEditor::Cursor(
                                                                                                                                                         position))));
    if (KDevelop::CodeCompletionModel* m = model())
        m->setCompletionContext(completionContext);

    if (completionContext && completionContext->isValid()) {
        {
            DUChainReadLocker lock(DUChain::lock());

            if (!context) {
                failed();
                qCDebug(LANGUAGE) << "Completion context disappeared before completions could be calculated";
                return;
            }
        }
        QList<CompletionTreeItemPointer> items = completionContext->completionItems(aborting(), fullCompletion());

        if (aborting()) {
            failed();
            return;
        }

        QList<QExplicitlySharedDataPointer<CompletionTreeElement>> tree = computeGroups(items, completionContext);

        if (aborting()) {
            failed();
            return;
        }

        tree += completionContext->ungroupedElements();

        foundDeclarations(tree, completionContext);
    } else {
        qCDebug(LANGUAGE) << "setContext: Invalid code-completion context";
    }
}

QList<QExplicitlySharedDataPointer<CompletionTreeElement>> CodeCompletionWorker::computeGroups(
    const QList<CompletionTreeItemPointer>& items,
    const QExplicitlySharedDataPointer<CodeCompletionContext>& completionContext)
{
    Q_UNUSED(completionContext);
    QList<QExplicitlySharedDataPointer<CompletionTreeElement>> tree;
    /**
     * 1. Group by argument-hint depth
     * 2. Group by inheritance depth
     * 3. Group by simplified attributes
     * */
    CodeCompletionItemGrouper<ArgumentHintDepthExtractor, CodeCompletionItemGrouper<InheritanceDepthExtractor,
            CodeCompletionItemGrouper<SimplifiedAttributesExtractor>>> argumentHintDepthGrouper(tree, nullptr, std::move(
            items));
    return tree;
}

void CodeCompletionWorker::abortCurrentCompletion()
{
    QMutexLocker lock(m_mutex);
    m_abort = true;
}

bool& CodeCompletionWorker::aborting()
{
    return m_abort;
}

KDevelop::CodeCompletionModel* CodeCompletionWorker::model() const
{
    return m_model;
}

void CodeCompletionWorker::updateContextRange(Range& contextRange, View* view, const DUContextPointer& context) const
{
    Q_UNUSED(contextRange);
    Q_UNUSED(view);
    Q_UNUSED(context);
}
