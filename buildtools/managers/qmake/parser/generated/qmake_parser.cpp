// THIS FILE IS GENERATED
// WARNING! All changes made in this file will be lost!

#include "qmake_parser.h"


#include "qmakelexer.h"
#include <kdebug.h>
#include <QtCore/QString>

namespace QMake
  {

  void parser::tokenize( const QString& contents )
  {
    m_contents =  contents;
    QMake::Lexer lexer( this,  contents );
    int kind =  parser::Token_EOF;

    do
      {
        kind =  lexer.getNextTokenKind();
        kDebug(9024) <<  kind <<  "(" <<  lexer.getTokenBegin() <<  "," <<  lexer.getTokenEnd() <<  ")::" <<  tokenText(lexer.getTokenBegin(),  lexer.getTokenEnd()) <<  "::" <<  endl; //" "; // debug output

        if  ( !kind ) // when the lexer returns 0, the end of file is reached
          kind =  parser::Token_EOF;

        parser::token_type &t =  this->token_stream->next();
        t.kind =  kind;
        t.begin =  lexer.getTokenBegin();
        t.end =  lexer.getTokenEnd();
      }

    while  ( kind !=  parser::Token_EOF );

    this->yylex(); // produce the look ahead token
  }

  QString parser::tokenText( std::size_t begin,  std::size_t end ) const
    {
      return  m_contents.mid((int)begin,  (int)end - begin + 1);
    }

  void parser::reportProblem( parser::ProblemType type,  const QString& message )
  {
    if  (type ==  Error)
      kDebug(9024) <<  "** ERROR: " <<  message <<  endl;
    else if  (type ==  Warning)
      kDebug(9024) <<  "** WARNING: " <<  message <<  endl;
    else if  (type ==  Info)
      kDebug(9024) <<  "** Info: " <<  message <<  endl;
  }


  // custom error recovery
  void parser::yy_expected_token(int /*expected*/,  std::size_t /*where*/,  char const *name)
  {
    reportProblem(
      parser::Error,
      QString("Expected token \"%1\"").arg(name));
  }

  void parser::yy_expected_symbol(int /*expected_symbol*/,  char const *name)
  {
    std::size_t line;
    std::size_t col;
    size_t index =  token_stream->index() - 1;
    token_type &token =  token_stream->token(index);
    kDebug(9024) <<  "token starts at: " <<  token.begin <<  endl;
    kDebug(9024) <<  "index is: " <<  index <<  endl;
    token_stream->start_position(index,  &line,  &col);
    size_t tokenLength =  token.end -  token.begin;
    QString tokenValue =  tokenText(token.begin,  token.end);
    reportProblem(
      parser::Error,
      QString("Expected symbol \"%1\" (current token: \"%2\" [%3] at line: %4 col: %5)")
      .arg(name)
      .arg(token.kind !=  0 ?  tokenValue :  "EOF")
      .arg(token.kind)
      .arg(line)
      .arg(col));
  }


} // end of namespace QMake


namespace QMake
  {

  bool parser::parse_arg_list(arg_list_ast **yynode)
  {
    *yynode =  create<arg_list_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_IDENTIFIER
         ||  yytoken ==  Token_VALUE ||  yytoken ==  Token_RPAREN)
      {
        if  (yytoken ==  Token_IDENTIFIER
             ||  yytoken ==  Token_VALUE)
          {
            id_or_value_ast *__node_0 =  0;

            if  (!parse_id_or_value(&__node_0))
              {
                yy_expected_symbol(ast_node::Kind_id_or_value,  "id_or_value");
                return  false;
              }

            if  (yytoken !=  Token_COMMA)
              {
                yy_expected_token(yytoken,  Token_COMMA,  "comma");
                return  false;
              }

            yylex();

            id_or_value_ast *__node_1 =  0;

            if  (!parse_id_or_value(&__node_1))
              {
                yy_expected_symbol(ast_node::Kind_id_or_value,  "id_or_value");
                return  false;
              }
          }

        else if  (true /*epsilon*/)
        {}
        else
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_function_args(function_args_ast **yynode)
  {
    *yynode =  create<function_args_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_LPAREN)
      {
        if  (yytoken !=  Token_LPAREN)
          {
            yy_expected_token(yytoken,  Token_LPAREN,  "lparen");
            return  false;
          }

        yylex();

        arg_list_ast *__node_2 =  0;

        if  (!parse_arg_list(&__node_2))
          {
            yy_expected_symbol(ast_node::Kind_arg_list,  "arg_list");
            return  false;
          }

        if  (yytoken !=  Token_RPAREN)
          {
            yy_expected_token(yytoken,  Token_RPAREN,  "rparen");
            return  false;
          }

        yylex();

      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_function_scope(function_scope_ast **yynode)
  {
    *yynode =  create<function_scope_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_LPAREN)
      {
        function_args_ast *__node_3 =  0;

        if  (!parse_function_args(&__node_3))
          {
            yy_expected_symbol(ast_node::Kind_function_args,  "function_args");
            return  false;
          }

        scope_body_ast *__node_4 =  0;

        if  (!parse_scope_body(&__node_4))
          {
            yy_expected_symbol(ast_node::Kind_scope_body,  "scope_body");
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_id_or_value(id_or_value_ast **yynode)
  {
    *yynode =  create<id_or_value_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_IDENTIFIER
         ||  yytoken ==  Token_VALUE)
      {
        if  (yytoken ==  Token_IDENTIFIER)
          {
            if  (yytoken !=  Token_IDENTIFIER)
              {
                yy_expected_token(yytoken,  Token_IDENTIFIER,  "identifier");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_VALUE)
          {
            if  (yytoken !=  Token_VALUE)
              {
                yy_expected_token(yytoken,  Token_VALUE,  "value");
                return  false;
              }

            yylex();

          }

        else
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_op(op_ast **yynode)
  {
    *yynode =  create<op_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_PLUSEQ
         ||  yytoken ==  Token_EQUAL
         ||  yytoken ==  Token_MINUSEQ
         ||  yytoken ==  Token_STAREQ
         ||  yytoken ==  Token_TILDEEQ)
      {
        if  (yytoken ==  Token_PLUSEQ)
          {
            if  (yytoken !=  Token_PLUSEQ)
              {
                yy_expected_token(yytoken,  Token_PLUSEQ,  "pluseq");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_MINUSEQ)
          {
            if  (yytoken !=  Token_MINUSEQ)
              {
                yy_expected_token(yytoken,  Token_MINUSEQ,  "minuseq");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_STAREQ)
          {
            if  (yytoken !=  Token_STAREQ)
              {
                yy_expected_token(yytoken,  Token_STAREQ,  "stareq");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_EQUAL)
          {
            if  (yytoken !=  Token_EQUAL)
              {
                yy_expected_token(yytoken,  Token_EQUAL,  "equal");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_TILDEEQ)
          {
            if  (yytoken !=  Token_TILDEEQ)
              {
                yy_expected_token(yytoken,  Token_TILDEEQ,  "tildeeq");
                return  false;
              }

            yylex();

          }

        else
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_project(project_ast **yynode)
  {
    *yynode =  create<project_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_NEWLINE
         ||  yytoken ==  Token_IDENTIFIER ||  yytoken ==  Token_EOF)
      {
        while  (yytoken ==  Token_NEWLINE
                ||  yytoken ==  Token_IDENTIFIER)
          {
            stmt_ast *__node_5 =  0;

            if  (!parse_stmt(&__node_5))
              {
                yy_expected_symbol(ast_node::Kind_stmt,  "stmt");
                return  false;
              }
          }

        if  (Token_EOF !=  yytoken)
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_scope_body(scope_body_ast **yynode)
  {
    *yynode =  create<scope_body_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_RBRACE
         ||  yytoken ==  Token_COLON)
      {
        if  (yytoken ==  Token_RBRACE)
          {
            if  (yytoken !=  Token_RBRACE)
              {
                yy_expected_token(yytoken,  Token_RBRACE,  "rbrace");
                return  false;
              }

            yylex();

            if  (yytoken ==  Token_NEWLINE)
              {
                if  (yytoken !=  Token_NEWLINE)
                  {
                    yy_expected_token(yytoken,  Token_NEWLINE,  "newline");
                    return  false;
                  }

                yylex();

              }

            else if  (true /*epsilon*/)
            {}
            else
              {
                return  false;
              }

            while  (yytoken ==  Token_NEWLINE
                    ||  yytoken ==  Token_IDENTIFIER)
              {
                stmt_ast *__node_6 =  0;

                if  (!parse_stmt(&__node_6))
                  {
                    yy_expected_symbol(ast_node::Kind_stmt,  "stmt");
                    return  false;
                  }
              }

            if  (yytoken !=  Token_LBRACE)
              {
                yy_expected_token(yytoken,  Token_LBRACE,  "lbrace");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_COLON)
          {
            if  (yytoken !=  Token_COLON)
              {
                yy_expected_token(yytoken,  Token_COLON,  "colon");
                return  false;
              }

            yylex();

            stmt_ast *__node_7 =  0;

            if  (!parse_stmt(&__node_7))
              {
                yy_expected_symbol(ast_node::Kind_stmt,  "stmt");
                return  false;
              }
          }

        else
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_stmt(stmt_ast **yynode)
  {
    *yynode =  create<stmt_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_NEWLINE
         ||  yytoken ==  Token_IDENTIFIER)
      {
        if  (yytoken ==  Token_IDENTIFIER)
          {
            if  (yytoken !=  Token_IDENTIFIER)
              {
                yy_expected_token(yytoken,  Token_IDENTIFIER,  "identifier");
                return  false;
              }

            yylex();

            if  (yytoken ==  Token_PLUSEQ
                 ||  yytoken ==  Token_EQUAL
                 ||  yytoken ==  Token_MINUSEQ
                 ||  yytoken ==  Token_STAREQ
                 ||  yytoken ==  Token_TILDEEQ)
              {
                variable_assignment_ast *__node_8 =  0;

                if  (!parse_variable_assignment(&__node_8))
                  {
                    yy_expected_symbol(ast_node::Kind_variable_assignment,  "variable_assignment");
                    return  false;
                  }
              }

            else if  (yytoken ==  Token_LPAREN)
              {
                function_scope_ast *__node_9 =  0;

                if  (!parse_function_scope(&__node_9))
                  {
                    yy_expected_symbol(ast_node::Kind_function_scope,  "function_scope");
                    return  false;
                  }
              }

            else if  (yytoken ==  Token_RBRACE
                      ||  yytoken ==  Token_COLON)
              {
                scope_body_ast *__node_10 =  0;

                if  (!parse_scope_body(&__node_10))
                  {
                    yy_expected_symbol(ast_node::Kind_scope_body,  "scope_body");
                    return  false;
                  }
              }

            else
              {
                return  false;
              }
          }

        else if  (yytoken ==  Token_NEWLINE)
          {
            if  (yytoken !=  Token_NEWLINE)
              {
                yy_expected_token(yytoken,  Token_NEWLINE,  "newline");
                return  false;
              }

            yylex();

          }

        else
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_value_list(value_list_ast **yynode)
  {
    *yynode =  create<value_list_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_CONT
         ||  yytoken ==  Token_IDENTIFIER
         ||  yytoken ==  Token_VALUE)
      {
        if  (yytoken ==  Token_IDENTIFIER
             ||  yytoken ==  Token_VALUE)
          {
            id_or_value_ast *__node_11 =  0;

            if  (!parse_id_or_value(&__node_11))
              {
                yy_expected_symbol(ast_node::Kind_id_or_value,  "id_or_value");
                return  false;
              }
          }

        else if  (yytoken ==  Token_CONT)
          {
            if  (yytoken !=  Token_CONT)
              {
                yy_expected_token(yytoken,  Token_CONT,  "cont");
                return  false;
              }

            yylex();

            if  (yytoken !=  Token_NEWLINE)
              {
                yy_expected_token(yytoken,  Token_NEWLINE,  "newline");
                return  false;
              }

            yylex();

          }

        else
          {
            return  false;
          }

        while  (yytoken ==  Token_CONT
                ||  yytoken ==  Token_IDENTIFIER
                ||  yytoken ==  Token_VALUE)
          {
            if  (yytoken ==  Token_IDENTIFIER
                 ||  yytoken ==  Token_VALUE)
              {
                id_or_value_ast *__node_12 =  0;

                if  (!parse_id_or_value(&__node_12))
                  {
                    yy_expected_symbol(ast_node::Kind_id_or_value,  "id_or_value");
                    return  false;
                  }
              }

            else if  (yytoken ==  Token_CONT)
              {
                if  (yytoken !=  Token_CONT)
                  {
                    yy_expected_token(yytoken,  Token_CONT,  "cont");
                    return  false;
                  }

                yylex();

                if  (yytoken !=  Token_NEWLINE)
                  {
                    yy_expected_token(yytoken,  Token_NEWLINE,  "newline");
                    return  false;
                  }

                yylex();

              }

            else
              {
                return  false;
              }
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }

  bool parser::parse_variable_assignment(variable_assignment_ast **yynode)
  {
    *yynode =  create<variable_assignment_ast>();

    (*yynode)->start_token =  token_stream->index() -  1;

    if  (yytoken ==  Token_PLUSEQ
         ||  yytoken ==  Token_EQUAL
         ||  yytoken ==  Token_MINUSEQ
         ||  yytoken ==  Token_STAREQ
         ||  yytoken ==  Token_TILDEEQ)
      {
        op_ast *__node_13 =  0;

        if  (!parse_op(&__node_13))
          {
            yy_expected_symbol(ast_node::Kind_op,  "op");
            return  false;
          }

        if  (yytoken ==  Token_CONT
             ||  yytoken ==  Token_IDENTIFIER
             ||  yytoken ==  Token_VALUE)
          {
            value_list_ast *__node_14 =  0;

            if  (!parse_value_list(&__node_14))
              {
                yy_expected_symbol(ast_node::Kind_value_list,  "value_list");
                return  false;
              }
          }

        else if  (true /*epsilon*/)
        {}
        else
          {
            return  false;
          }

        if  (yytoken ==  Token_NEWLINE)
          {
            if  (yytoken !=  Token_NEWLINE)
              {
                yy_expected_token(yytoken,  Token_NEWLINE,  "newline");
                return  false;
              }

            yylex();

          }

        else if  (yytoken ==  Token_EOF)
          {
            if  (yytoken !=  Token_EOF)
              {
                yy_expected_token(yytoken,  Token_EOF,  "EOF");
                return  false;
              }

            yylex();

          }

        else
          {
            return  false;
          }
      }

    else
      {
        return  false;
      }

    (*yynode)->end_token =  token_stream->index() -  1;

    return  true;
  }


} // end of namespace QMake


