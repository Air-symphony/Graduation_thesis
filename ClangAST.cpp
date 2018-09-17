#pragma once

#include "Map.cpp"
#include "clang-c\Index.h"
using namespace std;

Graph Arrow::arrowBase;
Graph Arrow::arrowTip;
Graph Node::nodeGraph;
CXFile file;
Map map;

/*���̍s�̃R�[�h�̐���*/
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

	string codeStr = GetCode(cursor);

	if (kind != CXCursorKind::CXCursor_CompoundStmt &&
		kind != CXCursorKind::CXCursor_DeclStmt) 
	{	
		if (kind == CXCursorKind::CXCursor_ClassDecl ||
			kind == CXCursorKind::CXCursor_CXXAccessSpecifier ||
			kind == CXCursorKind::CXCursor_ForStmt ||
			kind == CXCursorKind::CXCursor_WhileStmt ||
			kind == CXCursorKind::CXCursor_IfStmt || 
			kind == CXCursorKind::CXCursor_FunctionDecl || 
			kind == CXCursorKind::CXCursor_CXXMethod)
		{
			codeStr = "";
		}
		Node node(map.id, nameRange.begin_int_data, nameRange.end_int_data, clangVariableType, codeStr, variableName);

		map.CheckScope(&node);
		map.CheckExpression(&node);
		map.OpenLock(node);
		/*�ϐ�,�����o�ϐ��錾*/
		if (kind == CXCursorKind::CXCursor_VarDecl ||
			kind == CXCursorKind::CXCursor_FieldDecl)
		{
			node.AddOutput(variableName);
			map.Save_id_state(node.state);
			map.DeclLock = true;
		}
		/*�֐�,�����o�֐��錾*/
		else if (kind == CXCursorKind::CXCursor_FunctionDecl ||
			kind == CXCursorKind::CXCursor_CXXMethod) {
			map.Save_id_state(node.state);
		}
		/*�֐��̈����錾*/
		else if (kind == CXCursorKind::CXCursor_ParmDecl) {
			map.AddInOut_PreNode(node, INPUT);
		}
		/*return��*/
		else if (kind == CXCursorKind::CXCursor_ReturnStmt) {
			map.Save_id_state(node.state);
			map.DeclLock = map.inoutputFlag = true;
		}
		/*i++�Ȃ�*/
		else if (kind == CXCursorKind::CXCursor_UnaryOperator) {
			map.Save_id_state(node.state);
			map.inoutputFlag = true;
		}
		/*i+=a�Ȃ�*/
		else if (kind == CXCursorKind::CXCursor_CompoundAssignOperator) {
			map.Save_id_state(node.state);
			map.DeclLock = true;
		}
		/*�֐��Ăяo��*/
		else if (kind == CXCursorKind::CXCursor_CallExpr) {
			if (!map.DeclLock && !map.binaryLock) {
				map.Save_id_state(node.state);
			}
			map.inputFlag = true;
		}
		/*���̎��̏ڍׂ𒲂ׂ�*/
		else if (kind == CXCursorKind::CXCursor_BinaryOperator)
		{
			//��r���Z�q���܂ގ��̏ꍇ
			if ((codeStr.find("<") != std::string::npos) ||
				(codeStr.find(">") != std::string::npos) ||
				(codeStr.find("==") != std::string::npos)
				) {
				if (!map.DeclLock) {
					map.Save_id_state(node.state);
				}
				map.inputFlag = true;
			}
			//a = b = c, a = b + c�̂Ȃǂ̌��m
			else if(!map.binaryLock) {
				if (!map.DeclLock) {
					map.Save_id_state(node.state);
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
				if (!map.assignmentFlag) {
					//map.operatorCount += 1;//a + b [2=>0]
				}
			}
		}

		if (!map.SetNodeAbility(node)) //{}
			map.AddMap(node);

		//node.AddOutput(variableName);

	
		/*��ԏ�̊K�w�݂̂�\��*/
		if (kind == CXCursorKind::CXCursor_ForStmt ||
			kind == CXCursorKind::CXCursor_WhileStmt || 
			kind == CXCursorKind::CXCursor_IfStmt)
		{
			map.scopeOffset.AddOffset(nameRange.begin_int_data, nameRange.end_int_data);
		}
		else {
			map.exprOffset.AddOffset(nameRange.begin_int_data, nameRange.end_int_data);
		}
	}
	
	/*
	if (kind == CXCursorKind::CXCursor_DeclRefExpr) {
		map.AddVariableRelation(map.id - 1, variableName);
	}
	map.AddVariableName(map.id - 1, typeStr, variableName);
	*/
	return CXChildVisit_Recurse;
}

int PrintAST(char* _filepath) 
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

	/*�\���G���[��\��*/
	bool error = false;
	for (unsigned I = 0, N = clang_getNumDiagnostics(unit); I != N; ++I) {
		CXDiagnostic Diag = clang_getDiagnostic(unit, I);
		CXString String = clang_formatDiagnostic(Diag,
			clang_defaultDiagnosticDisplayOptions());
		printfDx("�E%d : %s\n", I, clang_getCString(String));
		clang_disposeString(String);
		error = true;
	}
	if (error) {
		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);
		return -1;
	}
	/*�m�[�h�̕\��*/
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