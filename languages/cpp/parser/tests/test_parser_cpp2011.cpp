/* This file is part of KDevelop

   Copyright 2010 Milian Wolff <mail@milianw.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "test_parser.h"

#include <QTest>

#include "parsesession.h"

void TestParser::testRangeBasedFor()
{
  QByteArray code("int main() {\n"
                  "  int array[5] = { 1, 2, 3, 4, 5 };\n"
                  "  for (int& x : array) {\n"
                  "    x *= 2;\n"
                  "  }\n"
                  " }\n");
  TranslationUnitAST* ast = parse(code);

  QVERIFY(ast);
  QVERIFY(ast->declarations);
  QVERIFY(control.problems().isEmpty());
}

void TestParser::testRValueReference()
{
  QByteArray code("int&& a = 1;");

  TranslationUnitAST* ast = parse(code);
  QVERIFY(control.problems().isEmpty());

  QVERIFY(ast);
  QVERIFY(ast->declarations);
}

void TestParser::testDefaultDeletedFunctions_data()
{
  QTest::addColumn<QString>("code");

  QTest::newRow("member-default") << "class A { A() = default; };\n";
  QTest::newRow("member-delete") << "class A { A() = delete; };\n";
  QTest::newRow("member-default-outside") << "class A { A(); };\nA::A() = default;\n";
  QTest::newRow("non-member-delete") << "void foo(int) = delete;\n";
}

void TestParser::testDefaultDeletedFunctions()
{
  QFETCH(QString, code);
  TranslationUnitAST* ast = parse(code.toUtf8());
  QVERIFY(control.problems().isEmpty());

  QVERIFY(ast);
  QVERIFY(ast->declarations);
}

void TestParser::testVariadicTemplates_data()
{
  QTest::addColumn<QString>("code");

  QTest::newRow("template-pack-class") << "template<class ... Arg> class A {};\n";
  QTest::newRow("template-pack-typename") << "template<typename ... Arg> class A {};\n";
  QTest::newRow("pack-expansion-baseclasses") << "template<class ... Arg> class A : public Arg... {};\n";
  QTest::newRow("pack-expansion-baseclass") << "template<class ... Arg> class A : public B<Arg...> {};\n";
  QTest::newRow("pack-expansion-tplarg") << "template<class ... Arg> class A { A() { A<Arg...>(); } };\n";
  QTest::newRow("pack-expansion-params") << "template<typename ... Arg> void A(Arg ... params) {}\n";
  QTest::newRow("pack-expansion-params-call") << "template<typename ... Arg> void A(Arg ... params) { A(params...); }\n";
  QTest::newRow("pack-expansion-mem-initlist") << "template<class ... Mixins> class A : public Mixins... { A(Mixins... args) : Mixins(args)... {} };\n";
  QTest::newRow("pack-expansion-mem-initlist-arg") << "template<class ... Args> class A : public B<Args...> { A(Args... args) : B<Args...>(args...) {} };\n";
  QTest::newRow("pack-expansion-initlist") << "template<typename ... Arg> void A(Arg ... params) { SomeList list = { params... }; }\n";
  QTest::newRow("pack-expansion-initlist2") << "template<typename ... Arg> void A(Arg ... params) { int a[] = { params... }; }\n";
  QTest::newRow("pack-expansion-initlist3") << "template<typename ... Arg> void A(Arg ... params) { int a[] = { (params+10)... }; }\n";
  QTest::newRow("pack-expansion-throw") << "template<typename ... Arg> void A() throw(Arg...) {};\n";
  QTest::newRow("sizeof...") << "template<typename ... Arg> void A(Arg ... params) { int i = sizeof...(params); }\n";
  ///TODO: attribute-list?
  ///TODO: alignment-specifier?
  ///TODO: capture-list?
}

void TestParser::testVariadicTemplates()
{
  QFETCH(QString, code);
  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);
  QVERIFY(control.problems().isEmpty());

  QVERIFY(ast);
  QVERIFY(ast->declarations);
}

void TestParser::testStaticAssert_data()
{
  QTest::addColumn<QString>("code");

  // see also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1720.html
  // section 2.3
  QTest::newRow("namespace-scope") << "static_assert(sizeof(long) >= 8, \"bla\");\n";
  QTest::newRow("class-scope") << "template <class A> class B {\n"
                                  "  static_assert(std::is_pod<A>::value, \"bla\");\n"
                                  "};\n";
  QTest::newRow("block-scope") << "void foo() { static_assert(sizeof(long) >= 8, \"bla\"); }\n";
}

void TestParser::testStaticAssert()
{
  QFETCH(QString, code);
  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);
  QVERIFY(control.problems().isEmpty());

  QVERIFY(ast);
  QVERIFY(ast->declarations);
}

void TestParser::testConstExpr_data()
{
  QTest::addColumn<QString>("code");

  QTest::newRow("function") << "constexpr int square(int x) { return x * x; }";
  QTest::newRow("member") << "class A { constexpr int foo() { return 1; } };";
  QTest::newRow("ctor") << "class A { constexpr A(); };";
  QTest::newRow("data") << "constexpr double a = 4.2 * square(2);";
}

void TestParser::testConstExpr()
{
  QFETCH(QString, code);
  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);
  QVERIFY(control.problems().isEmpty());

  QVERIFY(ast);
  QVERIFY(ast->declarations);
}

void TestParser::testEnumClass_data()
{
  QTest::addColumn<QString>("code");

  QTest::newRow("enum") << "enum Foo {A, B};";
  QTest::newRow("enum-class") << "enum class Foo {A, B};";
  QTest::newRow("enum-struct") << "enum struct Foo {A, B};";
  QTest::newRow("enum-typespec") << "enum Foo : int {A, B};";
  QTest::newRow("enum-opaque") << "enum Foo;";
  QTest::newRow("enum-opaque-class") << "enum class Foo;";
  QTest::newRow("enum-opaque-class-typespec") << "enum class Foo : char;";
  QTest::newRow("enum-opaque-typespec") << "enum Foo : unsigned int;";
}

void TestParser::testEnumClass()
{
  QFETCH(QString, code);
  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);
  QVERIFY(control.problems().isEmpty());

  QVERIFY(ast);
  QVERIFY(ast->declarations);
}

void TestParser::testRightAngleBrackets_data()
{
  QTest::addColumn<QString>("code");
  QTest::addColumn<bool>("isValid");

  // see also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1757.html
  // or spec 14.2/3
  QTest::newRow("invalid-1") << "X< 1>2 > x1;" << false; // Syntax error.
  QTest::newRow("valid-1") << "X<(1>2)> x2;" << true; // Okay
  QTest::newRow("valid-2") << "Y<X<1>> x3;" << true; // Okay, same as "Y<X<1> > x3;".
  QTest::newRow("invalid-2") << "Y<X<6>>1>> x4;" << false;  // Syntax error. Instead, write "Y<X<(6>>1)>> x4;".";
  QTest::newRow("valid-3") << "Y<X<(6>>1)>> x5;" << true;  // Okay
}

void TestParser::testRightAngleBrackets()
{
  QFETCH(QString, code);
  QFETCH(bool, isValid);

  code.prepend("template<int i> class X {};\n"
               "template<class T> class Y{};\n");
  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);

  QCOMPARE(control.problems().isEmpty(), isValid);
}

void TestParser::testCharacterTypes_data()
{
  QTest::addColumn<QString>("code");

  // see also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html
  QTest::newRow("char") << "char c = 'a';";
  QTest::newRow("wchar_t") << "wchar_t c = L'a';";
  QTest::newRow("char16_t") << "char16_t c = u'a';";
  QTest::newRow("char32_t") << "char32_t c = U'a';";

  QTest::newRow("char-str") << "const char* c = \"a\";";
  QTest::newRow("wchar_t-str") << "const wchar_t* c = L\"a\";";
  QTest::newRow("char16_t-str") << "const char16_t* c = u\"a\";";
  QTest::newRow("char32_t-str") << "const char32_t* c = U\"a\";";
  QTest::newRow("utf8-str") << "const char* c = u8\"a\";";
}

void TestParser::testCharacterTypes()
{
  QFETCH(QString, code);

  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);

  QVERIFY(control.problems().isEmpty());
}

void TestParser::testRawStrings_data()
{
  QTest::addColumn<QString>("code");

  // see also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
  QTest::newRow("char") << "const char* s = R\"(a)\";";
  QTest::newRow("wchar_t") << "const wchar_t* s = LR\"(a)\";";
  QTest::newRow("char16_t") << "const char16_t* s = uR\"(a)\";";
  QTest::newRow("char32_t") << "const char32_t* s = UR\"(a)\";";
  QTest::newRow("utf8") << "const char* s = u8R\"(a)\";";

  QTest::newRow("empty") << "const char* s = R\"()\";";
  QTest::newRow("delim1") << "const char* s = R\"g(a)g\";";
  QTest::newRow("delim2") << "const char* s = R\"g()\")g\";";
  QTest::newRow("delim3") << "const char* s = R\"*d~()\")*d~\";";
  QTest::newRow("delim4") << "const char* s = R\"*d~()*d~))*d~\";";
  QTest::newRow("escape") << "const char* s = R\"(\\n\\t)\";";
  QTest::newRow("newline") << "const char* s = R\"(\n\t\n)\";";
}

void TestParser::testRawStrings()
{
  QFETCH(QString, code);

  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);

  QVERIFY(control.problems().isEmpty());
}

void TestParser::testNullPtr_data()
{
  QTest::addColumn<QString>("code");

  // see also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf
  QTest::newRow("assign") << "char* ch = nullptr;";
  QTest::newRow("compare") << "if(ch == nullptr);";
  QTest::newRow("ternary") << "char* ch3 = true ? nullptr : nullptr;";
  QTest::newRow("sizeof") << "sizeof(nullptr);";
  QTest::newRow("typeid") << "typeid(nullptr);";
  QTest::newRow("throw") << "throw nullptr;";
}

void TestParser::testNullPtr()
{
  QFETCH(QString, code);

  code = "void foo() {\n" + code + "\n}\n";

  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);

  QVERIFY(control.problems().isEmpty());
}

void TestParser::testInlineNamespace()
{
  const QString code = "inline namespace foo {/*...*/}";

  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);

  QVERIFY(control.problems().isEmpty());
}

void TestParser::testDecltype_data()
{
  QTest::addColumn<QString>("code");

  // see also: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2343.pdf
  // simple type specifier
  QTest::newRow("idexpr-noparen") << "int i; decltype(i) j;";
  QTest::newRow("idexpr-paren") << "int i; decltype((i)) j;";
  QTest::newRow("classmember-noparen") << "struct A {int i;}; A a; decltype(a.i) j;";
  QTest::newRow("classmember2-noparen") << "struct A {int i;}; A* a; decltype(a->i) j;";
  QTest::newRow("classmember-paren") << "struct A {int i;}; A a; decltype((a.i)) j;";
  QTest::newRow("classmember2-paren") << "struct A {int i;}; A* a; decltype((a->i)) j;";
  QTest::newRow("functioncall") << "int foo() {} decltype(foo()) i;";
  QTest::newRow("expr") << "decltype(1+2) i;";
  QTest::newRow("idexpr-ref") << "int i; decltype(i)& i;";
}

void TestParser::testDecltype()
{
  QFETCH(QString, code);

  TranslationUnitAST* ast = parse(code.toUtf8());
  dump(ast);

  QVERIFY(control.problems().isEmpty());
}