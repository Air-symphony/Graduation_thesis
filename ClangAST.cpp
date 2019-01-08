#pragma once

#include "Map.cpp"
#include "clang-c\Index.h"
using namespace std;

Graph Arrow::arrowBase, Arrow::arrowTip, Arrow::arrowBase_dot;
Graph Node::processGraph, Node::process_LeftGraph, Node::process_RightGraph;
Graph Node::process_Left_SeparatorGraph;
Graph Node::loopProcess_RightGraph;
Graph Node::branchProcess_StandardGraph, Node::branchProcess_DefaultGraph;
Graph Node::dateStore_Name, Node::dateStore_Number;
string type;
bool CDFD::debug;
CXFile file;
Map map;
CDFD* cdfd;

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
	unsigned level = *(unsigned *)client_data;
	unsigned next = level + 1;
	/*clang���ł̌^�̕�����*/
	CXCursorKind kind = clang_getCursorKind(cursor);
	string clangVariableType = clang_getCString(clang_getCursorKindSpelling(kind));

	/*�ϐ����ƃR�[�h*/
	string variableName(clang_getCString(clang_getCursorDisplayName(cursor)));

	/*�R�[�h���ł̌^*/
	CXType clangType = clang_getCursorType(cursor);
	string typeStr = clang_getCString(clang_getTypeSpelling(clangType));

	//CXCursor parentTest = clang_getCursorSemanticParent(cursor);
	//CXSourceRange test = clang_getCursorExtent(cursor);
	CXSourceRange nameRange = clang_getCursorReferenceNameRange(cursor, CXNameRange_WantQualifier, 0);
	//CXSourceRange test = clang_Cursor_getCommentRange(cursor);

	unsigned int line, column, offset;
	clang_getExpansionLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);

	string codeStr = GetCode(cursor);

	if (kind == CXCursorKind::CXCursor_CXXAccessSpecifier ||
		kind == CXCursorKind::CXCursor_DeclStmt
		)
	{
		return CXChildVisit_Recurse;
	}
	if (kind == CXCursorKind::CXCursor_CompoundStmt ||
		//kind == CXCursorKind::CXCursor_ForStmt ||
		kind == CXCursorKind::CXCursor_WhileStmt
		//kind == CXCursorKind::CXCursor_IfStmt ||
		)
	{
		codeStr = "";
	}
	else if (kind == CXCursorKind::CXCursor_ClassDecl ||
		kind == CXCursorKind::CXCursor_FunctionDecl ||
		kind == CXCursorKind::CXCursor_CXXMethod)
	{
		codeStr = variableName;
	}
	/*cdfd�̐؂�ւ�*/
	if (map.CheckNodeInOffset(nameRange.begin_int_data, nameRange.end_int_data)) {
		cdfd = map.ChangeBeforeCDFD();
	}
	/*�������A�������A�X�V���̗L��*/
	bool forCheckStmt[3] = { false,false,false };
	Node node(cdfd->node_id, nameRange.begin_int_data, nameRange.end_int_data, clangVariableType, codeStr, variableName, level);

	bool ifstmt = (kind == CXCursorKind::CXCursor_IfStmt);
	
	cdfd->CheckScope(&node, ifstmt);
	cdfd->CheckExpression(&node);
	cdfd->OpenLock(node);
	/*�ϐ�,�����o�ϐ�,�����錾*/
	if (kind == CXCursorKind::CXCursor_VarDecl ||
		kind == CXCursorKind::CXCursor_FieldDecl ||
		kind == CXCursorKind::CXCursor_ParmDecl)
	{
		node.AddOutput(variableName);
		cdfd->Save_id_state_scope(node.state, node.scope);
		cdfd->inputFlag = true;
		cdfd->DeclLock = true;
	}
	/*�֐�,�����o�֐��錾*/
	else if (kind == CXCursorKind::CXCursor_FunctionDecl ||
		kind == CXCursorKind::CXCursor_CXXMethod) {
		cdfd->Save_id_state_scope(node.state, node.scope);
	}
	/*return��*/
	else if (kind == CXCursorKind::CXCursor_ReturnStmt) {
		cdfd->Save_id_state_scope(node.state, node.scope);
		cdfd->DeclLock = cdfd->inoutputFlag = true;
	}
	/*i++�Ȃ�*/
	else if (kind == CXCursorKind::CXCursor_UnaryOperator) {
		cdfd->Save_id_state_scope(node.state, node.scope);
		cdfd->inoutputFlag = true;
	}
	/*i+=a�Ȃ�*/
	else if (kind == CXCursorKind::CXCursor_CompoundAssignOperator) {
		cdfd->Save_id_state_scope(node.state, node.scope);
		cdfd->DeclLock = cdfd->inputFlag = cdfd->compoundAssignFlag = true;
	}
	/*i+=a�̏ڍׂȂ�*/
	else if ((kind == CXCursorKind::CXCursor_MemberRefExpr || kind == CXCursorKind::CXCursor_DeclRefExpr)
		&& cdfd->compoundAssignFlag
		) {
		cdfd->AddInOut_PreNode(node, Command::INOUTPUT);
		cdfd->compoundAssignFlag = false;
	}
	/*�֐��Ăяo��*/
	else if (kind == CXCursorKind::CXCursor_CallExpr) {
		if (!cdfd->DeclLock && !cdfd->binaryLock) {
			cdfd->Save_id_state_scope(node.state, node.scope);
		}
		cdfd->inputFlag = true;
	}
	/*���̎��̏ڍׂ𒲂ׂ�*/
	else if (kind == CXCursorKind::CXCursor_BinaryOperator)
	{
		//��r���Z�q���܂ގ��̏ꍇ
		if ((codeStr.find("<") != std::string::npos) ||
			(codeStr.find(">") != std::string::npos) ||
			(codeStr.find("==") != std::string::npos)
			) {
			if (!cdfd->DeclLock) {
				cdfd->Save_id_state_scope(node.state, node.scope);
			}
			cdfd->inputFlag = true;
		}
		//a = b = c, a = b + c�̂Ȃǂ̌��m
		else if (!cdfd->binaryLock) {
			if (!cdfd->DeclLock) {
				cdfd->Save_id_state_scope(node.state, node.scope);
			}
			cdfd->inputFlag = false;
			cdfd->binaryLock = true;
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
			cdfd->equalCount = equalCount;
			cdfd->operatorCount = operatorCount + equalCount + 1;
			cdfd->assignmentFlag = (equalCount > 0);
		}
	}
	/*if,else if���̏ꍇ*/
	else if (kind == CXCursorKind::CXCursor_IfStmt) {
		node.ChangeProcessType(ProcessType::BRANCH_STANDARD);
		cdfd->ifStmtFlag = true;
	}
	/*for���̏ꍇ*/
	else if (kind == CXCursorKind::CXCursor_ForStmt) {
		char separator = ';', escapeChar = ' ';
		char keys[2] = { '(', ')' };
		int keysCount = 0, checkCount = 0;
		bool check = false;
		for (int i = 0; i < codeStr.size(); ++i) {
			char target = codeStr[i];
			if (target == keys[keysCount]) {
				keysCount++;
				if (keysCount == 2) {
					forCheckStmt[checkCount] = check;
					break;
				}
				check = false;
			}
			else if (target == separator) {
				forCheckStmt[checkCount] = check;
				checkCount++;
				check = false;
			}
			else if (target != escapeChar){
				check = true;
			}
		}
		node.ChangeProcessType(ProcessType::FORLOOP);
	}
	/*while���̏ꍇ*/
	else if (kind == CXCursorKind::CXCursor_WhileStmt) {
		node.ChangeProcessType(ProcessType::WHILELOOP);
	}


	/*if(),else if()�Ȃǂ̏������̉��*/
	if (cdfd->getConditionFlag) {
		cdfd->SetText_PreNode(node);
		cdfd->DeclLock = cdfd->getConditionFlag = false;
	}
	/*�����̏���������荞�ނ���*/
	if (kind == CXCursorKind::CXCursor_IfStmt
		//|| kind == CXCursorKind::CXCursor_WhileStmt
		) {
		cdfd->Save_id_state_scope(node.state, node.scope);
		cdfd->DeclLock = cdfd->getConditionFlag = true;
	}
	
	if (kind != CXCursorKind::CXCursor_CompoundStmt) {
		if (!cdfd->SetNodeAbility(node)) {
			/*for���̏�������if���Ƃ��Ĉ���*/
			int index = cdfd->node_id - cdfd->countDateStoreProcess;
			for (int i = 0; i < index + 1; i++) {
				if (cdfd->forCheckStmt[i] == false) {
					index++;
				}
			}
			bool postProcess = false;
			if (index == 1) {
				if (cdfd->forCheckStmt[index]) {
					CDFD* cdfd_c = map.ChangeBeforeCDFD(1);
					Offset offset_c = cdfd_c->GetNodeOffset(cdfd_c->node_id - 1);
					node.ChangeProcessType(ProcessType::BRANCH_STANDARD);
					node.offset = offset_c;
					ifstmt = true;
				}
			}
			//while
			else if (index == 3 && cdfd->whileLoopStmt) {
				CDFD* cdfd_c = map.ChangeBeforeCDFD(1);
				Offset offset_c = cdfd_c->GetNodeOffset(cdfd_c->node_id - 1);
				node.ChangeProcessType(ProcessType::BRANCH_STANDARD);
				node.offset = offset_c;
				ifstmt = true;
			}
			bool hasElse = cdfd->CreateElseNode(node);
			if (hasElse) {
				node.id++;
			}
			cdfd->AddNode(node, ifstmt);
		}
		//cdfd->AddNode(node, ifstmt);
	}
	else {
		cdfd->CreateElseNode(node);
	}

	/*��ԏ�̊K�w�݂̂�\��*/
	if (kind == CXCursorKind::CXCursor_ClassDecl ||
		kind == CXCursorKind::CXCursor_FunctionDecl ||
		kind == CXCursorKind::CXCursor_CXXMethod ||
		kind == CXCursorKind::CXCursor_ForStmt ||
		kind == CXCursorKind::CXCursor_WhileStmt ||
		kind == CXCursorKind::CXCursor_IfStmt)
	{
		cdfd->scopeOffset.AddOffset(node.offset);
		cdfd->scopeOffset_nodeID.push_back(node.id);
		/*���ۉ����s���ׂ��\��*/
		if (kind != CXCursorKind::CXCursor_IfStmt) {
			cdfd->SetConcreteCDFD(map.GetAbstractCDFD_id());
			cdfd = map.AddCDFD(nameRange.begin_int_data, nameRange.end_int_data, forCheckStmt, kind == CXCursorKind::CXCursor_WhileStmt);
		}
	}
	else {
		cdfd->exprOffset.AddOffset(nameRange.begin_int_data, nameRange.end_int_data);
	}

	/*
	if (kind == CXCursorKind::CXCursor_DeclRefExpr) {
		nodes.AddVariableRelation(nodes.node_id - 1, variableName);
	}
	nodes.AddVariableName(nodes.node_id - 1, typeStr, variableName);
	*/
	if (CXChildVisit_Break == clang_visitChildren(cursor, visitChildrenCallback, &next)){
		map.CheckNodeInOffset(nameRange.begin_int_data, nameRange.end_int_data);
		if (kind == CXCursorKind::CXCursor_WhileStmt) {
			map.SetCondition();
		}
		else if (kind == CXCursorKind::CXCursor_ForStmt) {
			CDFD* forPocessCDFD = map.ChangeBeforeCDFD();
			int postProcessIndex = forPocessCDFD->countDateStoreProcess;
			for (int i = 0; i < 2; i++) {
				if (forPocessCDFD->forCheckStmt[i]) {
					postProcessIndex++;
				}
			}
			forPocessCDFD->PushBackNodes(postProcessIndex);
			map.SetCondition(forCheckStmt);
		}
	}

	return CXChildVisit_Continue;
}

/*�\�����*/
int ParsingNode(char* _filepath) 
{
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		_filepath, nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None);

	map.error = false;
	if (unit == nullptr)
	{
		printfDx("ERROR : [%s] not found.", _filepath);
		map.error = true;
		return -1;
	}
	file = clang_getFile(unit, _filepath);
	/*�\���G���[�̏ꍇ�͉�͂��Ȃ�*/
	if (clang_getNumDiagnostics(unit) != 0) {
		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);
		map.error = true;
		return -1;
	}
	cdfd = map.init();
	unsigned level = 0;
	clang_visitChildren(clang_getTranslationUnitCursor(unit),
		visitChildrenCallback,
		&level);
	CDFD::debug = false;
	map.SetPosition();
	map.Draw();
	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
	return 1;
}

/*�}�b�v�̕\��*/
bool PrintMap(char* _filepath) {
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		_filepath, nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None);

	file = clang_getFile(unit, _filepath);
	CXString str = clang_getFileName(file);
	printfDx("filename[%s]\n", clang_getCString(str));
	clang_disposeString(str);

	if (map.error) {
		for (unsigned I = 0, N = clang_getNumDiagnostics(unit); I != N; ++I) {
			CXDiagnostic Diag = clang_getDiagnostic(unit, I);
			CXString String = clang_formatDiagnostic(Diag,
				clang_defaultDiagnosticDisplayOptions());
			printfDx("�E%d : %s\n", I, clang_getCString(String));
			clang_disposeString(String);
		}
		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);
		map.error = true;
		return false;
	}
	return map.Draw();
}