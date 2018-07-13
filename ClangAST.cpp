#pragma once

#include "Map.cpp"
#include "DxLib.h"
#include "clang-c\Index.h"
using namespace std;

CXFile file;
Map map;
CXChildVisitResult visitChildrenCallback(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	/*clang���ł̌^�̕�����*/
	CXCursorKind kind = clang_getCursorKind(cursor);
	string clangVariableType = clang_getCString(clang_getCursorKindSpelling(kind));

	/*�ϐ����ƃR�[�h*/
	string variableName(clang_getCString(clang_getCursorDisplayName(cursor)));

	/*�R�[�h���ł̌^*/
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
	printfDx("<%d,%d,%d> ", line, column, offset);//�s��ԍ�
	printfDx("%s ", clang_getCString(clang_getCursorDisplayName(parentTest)));//�e�v�f
	printfDx("(%d, %d) ", test.begin_int_data, test.end_int_data);//�v�f�T�C�Y
	
	printfDx("%s : ", usrchar);//��`�o��
	printfDx("%s ", clang_getCString(typeStr));//�^�o��
	printfDx("%s ", clang_getCString(disStr));//�ϐ����o��
	//printfDx("def: %s ", clang_getCString(clang_getCursorDisplayName(dif)));
	*/
	
	/*���̍s�̃R�[�h�̐���*/
	string codeStr("");
	CXToken *tokens;
	unsigned numTokens;
	CXSourceRange range = clang_getCursorExtent(cursor);
	CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
	clang_tokenize(tu, range, &tokens, &numTokens);
	for (unsigned i = 0; i < numTokens; i++) {
		string str = clang_getCString(clang_getTokenSpelling(tu, tokens[i]));
		//printfDx("[%s] ", clang_getCString(s));//���g�̕\��
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
	/*�\���G���[��\��*/
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
�EDecl : Declare, �錾
FunctionDecl : �֐��錾
ParamVarDecl : �֐��̈����̐錾
VarDecl : �ϐ��̐錾
�EStmt : Statement, ��
CompoundStmt : �u���b�N��
DeclStmt : �ϐ��̐錾��(VarDecl���琬��)
ForStmt : For��
IfStmt : If��
ReturnStmt : Return��
�EExpr : Expression, ��
CStyleCastExpr : �L���X�g��
ImplicitCastExpr : �ÖكL���X�g��
CallExpr : �֐��Ăяo����
DeclRefExpr : �ϐ��Ăяo����
BinaryOperation : �񍀉��Z�q
UnaryOperation : �P�����Z�q
ArraySubscriptExpr : �z��\���̂��߂̎�
MemberExpr : �\���̂Ȃǂ̃����o���w����
�ELiteral : ���e����
IntegerLiteral : ����
StringLiteral : ������
*/