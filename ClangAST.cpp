#pragma once

#include "Map.cpp"
#include "DxLib.h"
#include "clang-c\Index.h"
using namespace std;

CXFile file;
Map map;
CXChildVisitResult visitChildrenCallback(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	
	CXString usrStr = clang_getCursorKindSpelling(clang_getCursorKind(cursor));
	CXString disStr = clang_getCursorDisplayName(cursor);
	CXType t = clang_getCursorType(cursor);
	CXString typeStr = clang_getTypeSpelling(t);

	//CXCursor parentTest = clang_getCursorSemanticParent(cursor);
	//CXSourceRange test = clang_getCursorExtent(cursor);
	CXSourceRange test = clang_getCursorReferenceNameRange(cursor, CXNameRange_WantQualifier, 0);
	//CXSourceRange test = clang_Cursor_getCommentRange(cursor);
	unsigned int line, column, offset;
	
	//clang_getRangeStart(test);
	//clang_getRangeEnd(test);
	//clang_getSpellingLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);
	clang_getExpansionLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);
	//CXString spellStr = clang_getcursor

	/*
	printfDx("<%d,%d,%d> ", line, column, offset);//行列番号
	printfDx("%s ", clang_getCString(clang_getCursorDisplayName(parentTest)));//親要素
	printfDx("(%d, %d) ", test.begin_int_data, test.end_int_data);//要素サイズ
	*/
	string usrchar = clang_getCString(usrStr);
	/*
	printfDx("%s : ", usrchar);//定義出力
	printfDx("%s ", clang_getCString(typeStr));//型出力
	printfDx("%s ", clang_getCString(disStr));//変数名出力
	//printfDx("def: %s ", clang_getCString(clang_getCursorDisplayName(dif)));
	*/
	string text("");
	/*
	if (strcmp(usrchar, "BinaryOperator") == 0
		|| strcmp(usrchar, "IntegerLiteral") == 0
		|| strcmp(usrchar, "ParmDecl") == 0
		|| strcmp(usrchar, "ReturnStmt") == 0) {//見つかった場合
	*/
	/*その行のコードの生成*/
	//if (usrchar != "UnexposedExpr"){
		CXToken *tokens;
		unsigned numTokens;
		CXSourceRange range = clang_getCursorExtent(cursor);
		CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
		clang_tokenize(tu, range, &tokens, &numTokens);
		string type("");
		for (unsigned i = 0; i < numTokens; i++) {
			CXString s = clang_getTokenSpelling(tu, tokens[i]);
			//printfDx("[%s] ", clang_getCString(s));//中身の表示
			text += (clang_getCString(s) + string(" "));
			clang_disposeString(s);
		}
		if (numTokens > 0) {
			type = clang_getCString(clang_getTokenSpelling(tu, tokens[0]));
			text.pop_back();
		}
	//}
	if (usrchar == "DeclRefExpr") {
		map.AddVariableRelation(line, text);
	}

	string variableName(clang_getCString(disStr));
	map.AddVariableName(line, type, variableName);
	if (usrchar != "DeclStmt" &&
		usrchar != "BinaryOperator" &&
		usrchar != "ReturnStmt") {
		//text = "";
	}

	if (map.memory != line) {
		//if (usrchar != "ForStmt" && usrchar != "IfStmt") {
			Node node(line, test.begin_int_data, test.end_int_data, usrchar, text);
			map.AddMap(node);
			//map.memory = line;
		//}
	}

	clang_disposeString(usrStr);
	clang_disposeString(typeStr);
	clang_disposeString(disStr);
	return CXChildVisit_Recurse;
}

int i = 0;
int PrintAST(char* _filepath) 
{
	/*構文エラーを表示*/
	/*
	CXIndex Index = clang_createIndex(1, 1);
	CXTranslationUnit TU = clang_parseTranslationUnit(Index,
		_filepath, nullptr, 0,
		nullptr, 0, CXTranslationUnit_None);
	if (TU == nullptr)
	{
		printfDx("ERROR : [%s] not found.", _filepath);
		return 0;
	}
	printfDx("path\n");
	for (unsigned I = 0, N = clang_getNumDiagnostics(TU); I != N; ++I) {
		CXDiagnostic Diag = clang_getDiagnostic(TU, I);
		CXString String = clang_formatDiagnostic(Diag,
			clang_defaultDiagnosticDisplayOptions());
		printfDx("%d : %s\n", I, clang_getCString(String));
		//fprintf(stderr, "%s\n", clang_getCString(String));
		clang_disposeString(String);
	}
	printfDx("path2\n");
	clang_disposeTranslationUnit(TU);
	clang_disposeIndex(Index);
	return 1;

	*/
	
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		_filepath, nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None);
	if (unit == nullptr)
	{
		printfDx("ERROR : [%s] not found.", _filepath);
		//exit(-1);
		return 0;
	}
	file = clang_getFile(unit, _filepath);
	CXString str = clang_getFileName(file);
	printfDx("filename[%s]\n", clang_getCString(str));
	clang_disposeString(str);

	map.init();
	clang_visitChildren(clang_getTranslationUnitCursor(unit),
		visitChildrenCallback,
		NULL);
	map.Draw();
	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
	return 1;
}

/*
・Decl : Declare, 宣言
FunctionDecl : 関数宣言
ParamVarDecl : 関数の引数の宣言
VarDecl : 変数の宣言
・Stmt : Statement, 文
CompoundStmt : ブロック文
DeclStmt : 変数の宣言文(VarDeclから成る)
ForStmt : For文
IfStmt : If文
ReturnStmt : Return文
・Expr : Expression, 式
CStyleCastExpr : キャスト式
ImplicitCastExpr : 暗黙キャスト式
CallExpr : 関数呼び出し式
DeclRefExpr : 変数呼び出し式
BinaryOperation : 二項演算子
UnaryOperation : 単項演算子
ArraySubscriptExpr : 配列表現のための式
MemberExpr : 構造体などのメンバを指す式
・Literal : リテラル
IntegerLiteral : 整数
StringLiteral : 文字列
*/