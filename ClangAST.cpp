#pragma once

#include "Map.cpp"
#include "DxLib.h"
#include "clang-c\Index.h"
using namespace std;

CXFile file;
Map map;
CXChildVisitResult visitChildrenCallback(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	/*clang内での型の文字列*/
	CXCursorKind kind = clang_getCursorKind(cursor);
	string clangVariableType = clang_getCString(clang_getCursorKindSpelling(kind));

	/*変数名とコード*/
	string variableName(clang_getCString(clang_getCursorDisplayName(cursor)));

	/*コード内での型*/
	CXType clangType = clang_getCursorType(cursor);
	string typeStr = clang_getCString(clang_getTypeSpelling(clangType));

	//CXString str = clang_getCursorSpelling(cursor);
	//CXCursor parentTest = clang_getCursorSemanticParent(cursor);
	//CXSourceRange test = clang_getCursorExtent(cursor);
	CXSourceRange nameRange = clang_getCursorReferenceNameRange(cursor, CXNameRange_WantQualifier, 0);
	//CXSourceRange test = clang_Cursor_getCommentRange(cursor);

	unsigned int line, column, offset;
	clang_getExpansionLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);

	/*
	printfDx("<%d,%d,%d> ", line, column, offset);//行列番号
	printfDx("%s ", clang_getCString(clang_getCursorDisplayName(parentTest)));//親要素
	printfDx("(%d, %d) ", test.begin_int_data, test.end_int_data);//要素サイズ
	
	printfDx("%s : ", usrchar);//定義出力
	printfDx("%s ", clang_getCString(typeStr));//型出力
	printfDx("%s ", clang_getCString(disStr));//変数名出力
	//printfDx("def: %s ", clang_getCString(clang_getCursorDisplayName(dif)));
	*/
	
	/*その行のコードの生成*/
	string codeStr("");
	CXToken *tokens;
	unsigned numTokens;
	CXSourceRange range = clang_getCursorExtent(cursor);
	CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
	clang_tokenize(tu, range, &tokens, &numTokens);
	for (unsigned i = 0; i < numTokens; i++) {
		string str = clang_getCString(clang_getTokenSpelling(tu, tokens[i]));
		//printfDx("[%s] ", clang_getCString(s));//中身の表示
		codeStr += (str + string(" "));
	}
	if (numTokens > 0) {
		codeStr.pop_back();
	}
	
	if (//kind != CXCursorKind::CXCursor_DeclStmt &&
		kind != CXCursorKind::CXCursor_VarDecl &&
		kind != CXCursorKind::CXCursor_BinaryOperator &&
		kind != CXCursorKind::CXCursor_UnaryOperator &&
		kind != CXCursorKind::CXCursor_ReturnStmt) {
		
		if (map.ifStmt == false) {
			codeStr = "";
		}
	}
	if (map.offset > (int)nameRange.begin_int_data) {
		codeStr = "";
	}

	if (codeStr != ""){
		map.id++;
		Node node(map.id, nameRange.begin_int_data, nameRange.end_int_data, clangVariableType, codeStr);
		map.AddMap(node);

		map.ifStmt = false;
		map.offset = nameRange.end_int_data;
		//printfDx("%s", node.DrawNode().c_str());
	}
	if (kind == CXCursorKind::CXCursor_DeclRefExpr) {
		map.AddVariableRelation(map.id, variableName);
	}
	map.AddVariableName(map.id, typeStr, variableName);
	
	if (kind == CXCursorKind::CXCursor_IfStmt) {
		map.ifStmt = true;
	}
	
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