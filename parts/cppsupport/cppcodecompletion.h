/***************************************************************************
                          cppcodecompletion.h  -  description
                             -------------------
    begin                : Sat Jul 21 2001
    copyright            : (C) 2001 by Victor R�der
    email                : victor_roeder@gmx.de
    copyright            : (C) 2002,2003 by Roberto Raggi
    email                : roberto@kdevelop.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __CPPCODECOMPLETION_H__
#define __CPPCODECOMPLETION_H__

#include <qobject.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qguardedptr.h>

#include <ktexteditor/viewcursorinterface.h>
#include <ktexteditor/editinterface.h>
#include <ktexteditor/codecompletioninterface.h>

#include "cppsupportpart.h"
#include "ast.h"

class ParsedClass;
class ParsedClassContainer;
class ParsedScopeContainer;
class CodeInformationRepository;
class CppCodeCompletionData;
class SimpleContext;

class CppCodeCompletion : public QObject
{
    Q_OBJECT
public:
    CppCodeCompletion( CppSupportPart* part );
    virtual ~CppCodeCompletion();

    CodeInformationRepository* repository() { return m_repository; }

    int expressionAt( const QString& text, int index );
    QStringList splitExpression( const QString& text );
    QStringList typeOf( const QString& name, const QStringList& scope );
    QStringList evaluateExpression( QString expr, SimpleContext* ctx );

    static QStringList typeName( const QString& name );
    
public slots:
    void completeText();

private slots:
    void slotActivePartChanged(KParts::Part *part);
    void slotArgHintHided();
    void slotCompletionBoxHided( KTextEditor::CompletionEntry entry );
    void slotTextChanged();
    void slotFileParsed( const QString& fileName );
    void slotTimeout();

private:
    void setupCodeInformationRepository();
    FunctionDefinitionAST* functionDefinition( AST* node );    
    void computeRecoveryPoints();
    
    QStringList evaluateExpressionInternal( QStringList& exprList, const QStringList& scope, SimpleContext* ctx=0 );
    
    QStringList typeOf( const QValueList<Tag>& tags );
    QStringList typeOf( const QString& name, ParsedClass* klass );
    QStringList typeOf( const QString& name, ParsedScopeContainer* scope );
    QStringList typeOf( const QString& name, const QValueList<ParsedMethod*>& methods );
    
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, SimpleContext* ctx );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, const QStringList& type );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, QValueList<Tag>& tags );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, ParsedClass* klass );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, ParsedScopeContainer* scope );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, const QValueList<ParsedMethod*>& methods );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, const QValueList<ParsedAttribute*>& attributes );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, const QValueList<ParsedClass*>& lst );
    void computeCompletionEntryList( QValueList<KTextEditor::CompletionEntry>& entryList, const QValueList<ParsedScopeContainer*>& lst );
    
    void computeSignatureList( QStringList& signatureList, const QString& name, const QStringList& type );
    void computeSignatureList( QStringList& signatureList, const QString& name, ParsedClass* klass );
    void computeSignatureList( QStringList& signatureList, const QString& name, const QValueList<ParsedMethod*>& methods );
    void computeSignatureList( QStringList& signatureList, const QString& name, QValueList<Tag>& tags );
    
    SimpleContext* computeContext( FunctionDefinitionAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, StatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, StatementListAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, IfStatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, ForStatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, DoStatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, WhileStatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, SwitchStatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, DeclarationStatementAST* ast, int line, int col );
    void computeContext( SimpleContext*& ctx, ConditionAST* ast, int line, int col );

    QString getText( int startLine, int startColumn, int endLine, int endColumn );
    
    QString findClass( const QString& className );

private:
    QGuardedPtr<CppSupportPart> m_pSupport;
    QTimer* m_ccTimer;
    QString m_activeFileName;
    KTextEditor::ViewCursorInterface* m_activeCursor;
    KTextEditor::EditInterface* m_activeEditor;
    KTextEditor::CodeCompletionInterface* m_activeCompletion;

    bool m_bArgHintShow;
    bool m_bCompletionBoxShow;

    unsigned int m_ccLine;
    unsigned int m_ccColumn;

    CodeInformationRepository* m_repository;
    CppCodeCompletionData* d;
};

#endif
