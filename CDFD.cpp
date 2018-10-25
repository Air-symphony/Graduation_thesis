#pragma once
#include "Node.cpp"

enum Command{
	INPUT_,
	OUTPUT,
	INOUTPUT
};

class CDFD {
private:
	/*Nodeのまとめ*/
	vector<Node> nodes;
	/*変数リスト*/
	/*
	vector<string> variableType;
	vector<string> variableName;
	vector<vector<int>> variableRelation;
	*/
	vector<int> maxWidthList;
	/*Debugモード切替*/
	bool ChangeDebugMode() {
		if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
			count++;
			if (count == 1) {
				debug = !debug;
				cursor_x = 0;
			}
		}
		else {
			count = 0;
		}
		return debug;
	}
	/*Debug*/
	int getDebugSize(){
		int range = 25;
		if (CheckHitKey(KEY_INPUT_UP) != 0) {
			if (cursor_x > 0) {
				cursor_x--;
			}
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
			if (cursor_x < nodes.size() - range) {
				cursor_x++;
			}
		}
		printfDx("[%d]\n", nodes.size());

		int size = cursor_x + range;
		if (nodes.size() < size) {
			size = (int)nodes.size();
			cursor_x = size - range;
			if (cursor_x < 0) cursor_x = 0;
		}
		return size;
	}
public:
	/*CDFDの番号*/
	int cdfd_id;
	/*Debug*/
	int cursor_x = 0, cursor_y = 0;
	int count = 0;
	static bool debug;

	int node_id = 0;
	OffsetList scopeOffset;
	OffsetList exprOffset;
	int preId, preScope, preState;
	bool inoutputFlag, inputFlag, outputFlag;
	bool assignmentFlag, compoundAssignFlag, getConditionFlag, ifStmtFlag;
	int equalCount, operatorCount;
	bool binaryLock, DeclLock;

	/*そのif文内で実行されるプロセスを回収するため*/
	vector<int> scopeOffset_nodeId;

	CDFD(int id = 0) {
		init();
		cdfd_id = id;
	}

	void init() {
		node_id = 0;
		preId = preState = preScope = -1;
		equalCount = operatorCount = 0;
		outputFlag = inoutputFlag = inputFlag = assignmentFlag = compoundAssignFlag = getConditionFlag = ifStmtFlag = false;
		binaryLock = DeclLock = false;
		int i = 0;
		for (i = (int)nodes.size(); i > 0; i--) {
			nodes.pop_back();
		}
		/*
		for (i = (int)variableName.size(); i > 0; i--) {
			variableName.pop_back();
		}
		for (i = (int)variableRelation.size(); i > 0; i--) {
			variableRelation.pop_back();
		}
		*/
		for (i = (int)scopeOffset_nodeId.size(); i > 0; i--) {
			scopeOffset_nodeId.pop_back();
		}
		for (i = (int)maxWidthList.size(); i > 0; i--) {
			maxWidthList.pop_back();
		}
		scopeOffset.init();
		exprOffset.init();
	}

	/*ifのインデントの深さ*/
	void CheckScope(Node *node, bool ifstmt = false) {
		int scope = scopeOffset.CheckOffset(node->offset, ifstmt);
		node->addScope(scope);
	}
	/*コードに対する詳細の深さ*/
	void CheckExpression(Node *node) {
		int state = exprOffset.CheckOffset(node->offset);
		node->addState(state);
	}

	void AddNode(Node node, bool ifstmt) {
		if (scopeOffset.CheckOffset_ElseStmt(node.offset)) {
			/*elseNodeの作成*/
			Node elseNode(node_id, node.offset.begin, node.offset.end, "elseStmt", "", "");
			elseNode.addScope(node.scope - 1);
			elseNode.addState(node.state);
			elseNode.ChangeProcessType(BRANCH_DEFAULT);
			/*input情報の入力先をずらす*/
			if (node_id == preId) {
				preId++;
			}
			nodes.push_back(elseNode);
			AddProcess_IfStmtNode(elseNode, true);
			node_id++;
			node.id++;
		}
		nodes.push_back(node);
		AddProcess_IfStmtNode(node, ifstmt);
		node_id++;
	}

	void AddProcess_IfStmtNode(Node node, bool ifstmt) {
		int size = (int)scopeOffset_nodeId.size();
		for (int i = node.scope; i < size; i++) {
			scopeOffset_nodeId.pop_back();
		}
		/*if文内に存在する場合*/
		if (0 < scopeOffset_nodeId.size()) {
			int id = scopeOffset_nodeId[scopeOffset_nodeId.size() - 1];
			nodes[id].AddNode(node_id);
		}
		if (ifstmt) {
			scopeOffset_nodeId.push_back(node_id);
		}
	}
	/*今から作業するコードが、以前のノードに内包されていなければ、全てのフラグを解除*/
	bool OpenLock(Node node) {
		if (node.scope <= preScope && 
			node.state <= preState) {
			preState = -1;
			binaryLock = DeclLock = inoutputFlag = inputFlag = 
				outputFlag = assignmentFlag = getConditionFlag = false;
			return true;
		}
		return false;
	}
	/*
	preId = node_id;
	preState = state;
	preScope = scope;
	*/
	void Save_id_state_scope(int state, int scope) {
		preId = node_id;
		preState = state;
		preScope = scope;
	}
	/*command = INPUT_, OUTPUT, INOUTPUT*/
	bool AddInOut_PreNode(Node node, Command command) {
		switch (command) {
		case Command::INPUT_: 
			return nodes[preId].AddInput(node.variableName);
		
		case Command::OUTPUT:
			return nodes[preId].AddOutput(node.variableName);
		
		case Command::INOUTPUT: 
			return (nodes[preId].AddInput(node.variableName) &&
				nodes[preId].AddOutput(node.variableName));
		default:
			return false;
		}
	}

	bool SetText_PreNode(Node node) {
		nodes[preId].text = node.text;
		return true;
	}

	/*
	*各ノードに入出力される変数を指定
	*指定できたということは、そのノードは表示する必要がない
	*/
	bool SetNodeAbility(Node node, bool input = true) {
		if (preId == node_id || preState < 0) return false;

		/*自分より下の情報の場合*/
		if (inoutputFlag) {
			AddInOut_PreNode(node, Command::INOUTPUT);
		}
		else if (outputFlag) {
			AddInOut_PreNode(node, Command::OUTPUT);
		}
		else if (inputFlag) {
			AddInOut_PreNode(node, Command::INPUT_);
		}
		else if (operatorCount > 0) {
			bool insert = false;
			if (assignmentFlag) {
				insert = AddInOut_PreNode(node, Command::OUTPUT);
			}
			else {
				insert = AddInOut_PreNode(node, Command::INPUT_);
			}
			if (insert) {
				equalCount--;
				assignmentFlag = (equalCount > 0);
				operatorCount--;
			}
		}
		return true;
	}

	static void CopyVariableInOut(CDFD* copyTo, CDFD original) {
		for (int i = 0; i < original.nodes.size(); i++) {
			copyTo->nodes[copyTo->nodes.size() - 1].CopyVariableInOut(original.nodes[i]);
		}
	}

	static void CopyConditionText(CDFD* copyTo, CDFD original, bool* forCondition) {
		Node* copyNodeTo = &copyTo->nodes[copyTo->nodes.size() - 1];
		switch (copyNodeTo->processType)
		{
		case FORLOOP:
			if (forCondition == NULL) {
				
			}
			else if (forCondition[1]) {
				int index = 0;
				if (forCondition[0]) {
					index++;
				}
				Node::CopyConditionText(&copyTo->nodes[copyTo->nodes.size() - 1], original.nodes[index]);
			}

			else {
				Node::CopyConditionText_NoCondition(&copyTo->nodes[copyTo->nodes.size() - 1]);
			}
			break;
		case WHILELOOP:
			Node::CopyConditionText(&copyTo->nodes[copyTo->nodes.size() - 1], original.nodes[0]);
			break;
		default:
			break;
		}
	}

	/*具体化されるCDFDのIDをnodeにset*/
	void SetConcreteCDFD(int id) {
		nodes[node_id - 1].SetconcreteCDFD_id(id);
	}
	/*
	int AddVariableName(int _id, string type, string variable) {
		if (variable == "") return 0;

		auto itr = std::find(variableName.begin(), variableName.end(), variable);//最初の要素のイテレータを返す
		if (itr == variableName.end()) {//新規の変数の場合
			variableName.push_back(variable);
			variableType.push_back(type);
			vector<int> list{ _id };
			variableRelation.push_back(list);
		}
		return 1;
	}

	int AddVariableRelation(int _id, string variable) {
		auto itr = find(variableName.begin(), variableName.end(), variable);
		int index = (int)distance(variableName.begin(), itr);
		if (index != variableName.size()) { // 発見できたとき
			if (_id - 1 < 0 || nodes.size() <= _id - 1) {
				return -1;
			}
			variableRelation[index].push_back(_id);
			//nodes[_id - 1].SetDefIndex(variableRelation[cursor_x][0]);
			return 1;
		}
		return 0;
	}
	*/

	/*ノードの位置関係を決める*/
	void SetPosition() {
		Node::SetGraph();
		Arrow::SetGraph();
		MyDrawString myDraw(15);
		maxWidthList.push_back(50);
		for (int i = 0; i < nodes.size(); i++) {
			nodes[i].SetNodeSize(&myDraw);
			nodes[i].SetPosX(nodes);
			nodes[i].SetPosY(nodes);
			maxWidthList[nodes[i].pos.x] = nodes[i].GetWidth(&maxWidthList);
		}
	}

	/*マップの表示*/
	bool Draw() {
		if (nodes.size() <= 0) {
			printfDx("Error[cdfd:%d] : nodes size = %d\n", cdfd_id, nodes.size());
			return false;
		}

		ChangeDebugMode();
		if (debug) {
			int size = getDebugSize();
			for (int i = cursor_x; i < size; i++) {
				printfDx(nodes[i].DrawNode().c_str());
			}
			printfDx("\n");
			
			/*
			for (int i = 0; i < variableName.size(); ++i) {
				printfDx("%s ", variableType[i].c_str());
				printfDx("%s => [", variableName[i].c_str());
				for (int j = 0; j < variableRelation[i].size(); j++) {
					printfDx("%d, ", variableRelation[i][j]);
				}
				printfDx("]\n");
			}
			*/
		}
		else {
			Node::SetGraph();
			Arrow::SetGraph();
			MyDrawString myDraw(15);
			int max_x = 0, max_y = 0;
			for (int i = 0; i < nodes.size();i++) {
				nodes[i].DrawArrow(&maxWidthList, cursor_x, cursor_y, &myDraw, nodes);
				nodes[i].DrawNode(&maxWidthList, cursor_x, cursor_y, &myDraw);
				max_x += nodes[i].width;
				max_y += nodes[i].height;
			}

			int speed = 20;
			if (CheckHitKey(KEY_INPUT_UP) != 0) {
				if (cursor_y > 0) {
					cursor_y -= speed;
				}
			}
			else if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
				if (cursor_y < max_y) {
					cursor_y += speed;
				}
			}
			if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
				if (cursor_x > 0) {
					cursor_x -= speed;
				}
			}
			else if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
				if (cursor_x < max_x) {
					cursor_x += speed;
				}
			}
			printfDx("[%d/%d, %d/%d]\n", cursor_x, max_x, cursor_y, max_y);
		}
		return true;
	}
};