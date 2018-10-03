#pragma once

#include "Map.cpp"
#include "clang-c\Index.h"
using namespace std;

Graph Arrow::arrowBase;
Graph Arrow::arrowTip;
Graph Node::nodeGraph, Node::nodeGraph_Left, Node::nodeGraph_Right;
CXFile file;
Map map;

/*その行のコードの生成*/
string GetCode(CXCursor cursor) {
	string codeStr("");
	CXToken *tokens;
	unsigned numTokens;
	CXSourceRange range = clang_getCursorExtent(cursor);
	CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
	clang_tokenize(tu, range, &tokens, &numTokens);
	for (unsigned i = 0; i < numTokens; i++) {
		string str = clang_getCString(clang_getTokenSpelling(tu, tokens[i]));
		codeStr += (str + string(" "));
	}
	if (numTokens > 0) {
		codeStr.pop_back();
	}
	return codeStr;
}

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

	string codeStr = GetCode(cursor);

	if (kind == CXCursorKind::CXCursor_DeclStmt)
	{
		return CXChildVisit_Recurse;
	}
	if (kind == CXCursorKind::CXCursor_CompoundStmt || 
		kind == CXCursorKind::CXCursor_ClassDecl ||
		//kind == CXCursorKind::CXCursor_CXXAccessSpecifier ||
		kind == CXCursorKind::CXCursor_ForStmt ||
		kind == CXCursorKind::CXCursor_WhileStmt ||
		//kind == CXCursorKind::CXCursor_IfStmt ||
		kind == CXCursorKind::CXCursor_FunctionDecl ||
		kind == CXCursorKind::CXCursor_CXXMethod)
	{
		codeStr = "";
	}
	Node node(map.id, nameRange.begin_int_data, nameRange.end_int_data, clangVariableType, codeStr, variableName);

	bool ifstmt = kind == CXCursorKind::CXCursor_IfStmt;
	map.CheckScope(&node, ifstmt);
	map.CheckExpression(&node);
	map.OpenLock(node);
	/*変数,メンバ変数宣言*/
	if (kind == CXCursorKind::CXCursor_VarDecl ||
		kind == CXCursorKind::CXCursor_FieldDecl)
	{
		node.AddOutput(variableName);
		map.Save_id_state_scope(node.state, node.scope);
		map.DeclLock = true;
	}
	/*関数,メンバ関数宣言*/
	else if (kind == CXCursorKind::CXCursor_FunctionDecl ||
		kind == CXCursorKind::CXCursor_CXXMethod) {
		map.Save_id_state_scope(node.state, node.scope);
	}
	/*関数の引数宣言*/
	else if (kind == CXCursorKind::CXCursor_ParmDecl) {
		map.AddInOut_PreNode(node, INPUT);
	}
	/*return文*/
	else if (kind == CXCursorKind::CXCursor_ReturnStmt) {
		map.Save_id_state_scope(node.state, node.scope);
		map.DeclLock = map.inoutputFlag = true;
	}
	/*i++など*/
	else if (kind == CXCursorKind::CXCursor_UnaryOperator) {
		map.Save_id_state_scope(node.state, node.scope);
		map.inoutputFlag = true;
	}
	/*i+=aなど*/
	else if (kind == CXCursorKind::CXCursor_CompoundAssignOperator) {
		map.Save_id_state_scope(node.state, node.scope);
		map.DeclLock = map.inputFlag = map.compoundAssignFlag = true;
	}
	/*i+=aの詳細など*/
	else if (kind == CXCursorKind::CXCursor_MemberRefExpr &&
		map.compoundAssignFlag) {
		map.AddInOut_PreNode(node, INOUTPUT);
		map.compoundAssignFlag = false;
	}
	/*関数呼び出し*/
	else if (kind == CXCursorKind::CXCursor_CallExpr) {
		if (!map.DeclLock && !map.binaryLock) {
			map.Save_id_state_scope(node.state, node.scope);
		}
		map.inputFlag = true;
	}
	/*その式の詳細を調べる*/
	else if (kind == CXCursorKind::CXCursor_BinaryOperator)
	{
		//比較演算子を含む式の場合
		if ((codeStr.find("<") != std::string::npos) ||
			(codeStr.find(">") != std::string::npos) ||
			(codeStr.find("==") != std::string::npos)
			) {
			if (!map.DeclLock) {
				map.Save_id_state_scope(node.state, node.scope);
			}
			map.inputFlag = true;
		}
		//a = b = c, a = b + cのなどの検知
		else if(!map.binaryLock) {
			if (!map.DeclLock) {
				map.Save_id_state_scope(node.state, node.scope);
			}
			map.binaryLock = true;
			int equalCount = 0, operatorCount = 0;
			char key = '=';
			char keys[4] = { '+', '-', '*', '/' };
			for (int i = 0; i < codeStr.size(); i++) {
				char target = codeStr[i];
				if (key == target) {
					equalCount++;
				}
				for (int i = 0; i < 4; i++) {
					if (keys[i] == target) {
						operatorCount++;
					}
				}
			}
			map.equalCount = equalCount;
			map.operatorCount = operatorCount + equalCount + 1;
			map.assignmentFlag = (equalCount > 0);
		}
	}
	/*if(),else if()などの条件式の回収*/
	if (map.ifstmtFlag) {
		map.SetText_PreNode(node);
		map.DeclLock = map.ifstmtFlag = false;
	}
	/*if、else ifの場合*/
	if (kind == CXCursorKind::CXCursor_IfStmt ||
		kind == CXCursorKind::CXCursor_WhileStmt) {
		map.Save_id_state_scope(node.state, node.scope);
		map.DeclLock = map.ifstmtFlag = true;
	}

	if (!map.SetNodeAbility(node))//{}
		map.AddMap(node);
	
	/*一番上の階層のみを表示*/
	if (kind == CXCursorKind::CXCursor_ClassDecl || 
		kind == CXCursorKind::CXCursor_FunctionDecl ||
		kind == CXCursorKind::CXCursor_CXXMethod || 
		kind == CXCursorKind::CXCursor_ForStmt ||
		kind == CXCursorKind::CXCursor_WhileStmt || 
		kind == CXCursorKind::CXCursor_IfStmt)
	{
		map.scopeOffset.AddOffset(nameRange.begin_int_data, nameRange.end_int_data);
	}
	else {
		map.exprOffset.AddOffset(nameRange.begin_int_data, nameRange.end_int_data);
	}
	
	/*
	if (kind == CXCursorKind::CXCursor_DeclRefExpr) {
		map.AddVariableRelation(map.id - 1, variableName);
	}
	map.AddVariableName(map.id - 1, typeStr, variableName);
	*/
	return CXChildVisit_Recurse;
}

/*構文解析*/
int ParsingNode(char* _filepath) 
{
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		_filepath, nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None);

	if (unit == nullptr)
	{
		printfDx("ERROR : [%s] not found.", _filepath);
		return -1;
	}
	file = clang_getFile(unit, _filepath);
	CXString str = clang_getFileName(file);
	printfDx("filename[%s]\n", clang_getCString(str));
	clang_disposeString(str);

	/*構文エラーを表示*/
	bool error = false;
	for (unsigned I = 0, N = clang_getNumDiagnostics(unit); I != N; ++I) {
		CXDiagnostic Diag = clang_getDiagnostic(unit, I);
		CXString String = clang_formatDiagnostic(Diag,
			clang_defaultDiagnosticDisplayOptions());
		printfDx("・%d : %s\n", I, clang_getCString(String));
		clang_disposeString(String);
		error = true;
	}
	if (error) {
		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);
		map.error = true;
		return -1;
	}
	map.init();
	clang_visitChildren(clang_getTranslationUnitCursor(unit),
		visitChildrenCallback,
		NULL);
	map.Draw();
	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
	return 1;
}

/*マップの表示*/
bool PrintMap(char* _filepath) {
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		_filepath, nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None);

	bool error = false;
	for (unsigned I = 0, N = clang_getNumDiagnostics(unit); I != N; ++I) {
		CXDiagnostic Diag = clang_getDiagnostic(unit, I);
		CXString String = clang_formatDiagnostic(Diag,
			clang_defaultDiagnosticDisplayOptions());
		printfDx("・%d : %s\n", I, clang_getCString(String));
		clang_disposeString(String);
		error = true;
	}
	if (error) {
		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);
		map.error = true;
		return false;
	}
	return map.Draw();
}