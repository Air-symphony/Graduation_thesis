#pragma once

#include "Node.cpp"
#include "DxLib.h"
#include "clang-c\Index.h"

CXFile file;
Map map;
CXChildVisitResult visitChildrenCallback(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	
	CXString usrStr = clang_getCursorKindSpelling(clang_getCursorKind(cursor));
	CXString disStr = clang_getCursorDisplayName(cursor);
	CXType t = clang_getCursorType(cursor);
	CXString typeStr = clang_getTypeSpelling(t);

	CXCursor parentTest = clang_getCursorSemanticParent(cursor);
	//CXSourceRange test = clang_getCursorExtent(cursor);
	CXSourceRange test = clang_getCursorReferenceNameRange(cursor, CXNameRange_WantQualifier, 0);
	//CXSourceRange test = clang_Cursor_getCommentRange(cursor);
	unsigned int line, column, offset;
	
	clang_getRangeStart(test);
	clang_getRangeEnd(test);
	//clang_getSpellingLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);
	clang_getExpansionLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);
	//CXString spellStr = clang_getcursor

	/*
	printfDx("<%d,%d,%d> ", line, column, offset);//行列番号
	printfDx("%s ", clang_getCString(clang_getCursorDisplayName(parentTest)));//親要素
	printfDx("(%d, %d) ", test.begin_int_data, test.end_int_data);//要素サイズ
	*/
	const char* usrchar = clang_getCString(usrStr);
	/*
	printfDx("%s : ", usrchar);//定義出力
	printfDx("%s ", clang_getCString(typeStr));//型出力
	printfDx("%s ", clang_getCString(disStr));//変数名出力
	//printfDx("def: %s ", clang_getCString(clang_getCursorDisplayName(dif)));
	*/
	char* text = (char*)malloc(100);
	*text = '\0';
	//strcat_s(text, 100, "");

	if (strcmp(usrchar, "BinaryOperator") == 0
		|| strcmp(usrchar, "IntegerLiteral") == 0
		|| strcmp(usrchar, "ParmDecl") == 0
		|| strcmp(usrchar, "ReturnStmt") == 0) {//見つかった場合
	
		CXToken *tokens;
		unsigned numTokens;
		CXSourceRange range = clang_getCursorExtent(cursor);
		CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
		clang_tokenize(tu, range, &tokens, &numTokens);
		for (unsigned i = 0; i < numTokens; i++) {
			CXString s = clang_getTokenSpelling(tu, tokens[i]);
			//printfDx("[%s] ", clang_getCString(s));//中身の表示
			strcat_s(text, 100, clang_getCString(s));
			clang_disposeString(s);
		}
	}
	//printfDx("\n");

	clang_disposeString(usrStr);
	clang_disposeString(typeStr);
	clang_disposeString(disStr);

	Node node(line, offset, text, 0);
	map.Add(node);
	return CXChildVisit_Recurse;
}

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