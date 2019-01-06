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
	int countDateStoreProcess = 0;
	OffsetList scopeOffset;
	vector<int> scopeOffset_nodeID;
	OffsetList exprOffset;
	int preId, preScope, preState;
	bool inoutputFlag, inputFlag, outputFlag;
	bool assignmentFlag, compoundAssignFlag, getConditionFlag, ifStmtFlag;
	int equalCount, operatorCount;
	bool binaryLock, DeclLock;
	bool forLoopStmt, whileLoopStmt;
	bool forCheckStmt[3] = { false , false, false};

	/*そのif文内で実行されるプロセスを回収するため*/
	vector<int> scopeOffset_nodeIDInIfStmt;

	CDFD(int id = 0) {
		init();
		cdfd_id = id;
	}

	void init() {
		node_id = 0;
		preId = preState = preScope = -1;
		countDateStoreProcess = equalCount = operatorCount = 0;
		outputFlag = inoutputFlag = inputFlag = assignmentFlag = compoundAssignFlag = getConditionFlag = ifStmtFlag = false;
		binaryLock = DeclLock = forLoopStmt = whileLoopStmt = false;
		for (int i = 0; i < 3; i++) {
			forCheckStmt[i] = false;
		}
		int i = 0;
		for (i = (int)nodes.size(); i > 0; i--) {
			nodes.pop_back();
		}
		/*
		for (index = (int)variableName.size(); index > 0; index--) {
			variableName.pop_back();
		}
		for (index = (int)variableRelation.size(); index > 0; index--) {
			variableRelation.pop_back();
		}
		*/
		for (i = (int)scopeOffset_nodeIDInIfStmt.size(); i > 0; i--) {
			scopeOffset_nodeIDInIfStmt.pop_back();
		}
		for (i = (int)scopeOffset_nodeID.size(); i > 0; i--) {
			scopeOffset_nodeID.pop_back();
		}
		for (i = (int)maxWidthList.size(); i > 0; i--) {
			maxWidthList.pop_back();
		}
		scopeOffset.init();
		exprOffset.init();
	}

	/*ifのインデントの深さ*/
	void CheckScope(Node *node, bool ifstmt = false) {
		if (ifstmt && scopeOffset.CheckOffset_ElseStmt(node->offset)) {
			node->connectIfStmt_id = scopeOffset_nodeID[scopeOffset_nodeID.size() - 1];
		}
		int scope = scopeOffset.CheckOffset(node->offset, ifstmt);
		node->addScope(scope);
		for (int i = scopeOffset_nodeID.size(); i > scopeOffset.size(); i--) {
			scopeOffset_nodeID.pop_back();
		}
	}

	/*コードに対する詳細の深さ*/
	void CheckExpression(Node *node) {
		int state = exprOffset.CheckOffset(node->offset);
		node->addState(state);
	}

	void AddDateStoreNode(CDFD pre) {
		for (int i = 0; i < pre.nodes.size(); i++) {
			if (pre.nodes[i].CheckDeclStmt()) {
				pre.nodes[i].ChangeProcessType(DATESTORE);
				pre.nodes[i].id = node_id;
				nodes.push_back(pre.nodes[i]);
				node_id++;
			}
		}
		countDateStoreProcess = node_id;
	}
	void CheckLoopStmt(bool* copyforCheckStmt, bool isWhileStmt) {
		whileLoopStmt = isWhileStmt;
		for (int i = 0; i < 3; i++) {
			forCheckStmt[i] = copyforCheckStmt[i];
			if (forCheckStmt[i]) {
				forLoopStmt = true;
			}
		}
	}

	void CreateElseNode(Node node) {
		if (scopeOffset.CheckOffset_ElseStmt(node.offset) && nodes[node_id - 1].processType != BRANCH_STANDARD) {
			/*elseNodeの作成*/
			Node elseNode(node_id, node.offset.begin, node.offset.end, "elseStmt", "", "");
			elseNode.addScope(node.scope - 1);
			elseNode.addState(node.state);
			elseNode.ChangeProcessType(BRANCH_DEFAULT);
			elseNode.connectIfStmt_id = scopeOffset_nodeID[scopeOffset_nodeID.size() - 1];
			/*input情報の入力先をずらす*/
			if (node_id == preId) {
				preId++;
			}
			nodes.push_back(elseNode);
			AddProcess_IfStmtNode(elseNode, true);
			node_id++;
			node.id++;
		}
	}

	void AddNode(Node node, bool ifstmt) {
		nodes.push_back(node);
		AddProcess_IfStmtNode(node, ifstmt);
		node_id++;
	}

	void AddProcess_IfStmtNode(Node node, bool ifstmt) {
		
		for (int i = scopeOffset_nodeIDInIfStmt.size() - 1; i >= 0; i--) {
			int id = scopeOffset_nodeIDInIfStmt[i];
			if (nodes[id].offset.end < node.offset.begin) {
				scopeOffset_nodeIDInIfStmt.pop_back();
			}
			
			if (nodes[id].offset.end == node.offset.end) {
				if (nodes[id].scope >= node.scope) {
					if (forLoopStmt || whileLoopStmt) {
						forLoopStmt = whileLoopStmt = false;
					}
					else {
						scopeOffset_nodeIDInIfStmt.pop_back();
					}
				}
			}
		}
		
		/*プロセスがif文内に存在する場合*/
		if (0 < scopeOffset_nodeIDInIfStmt.size()) {
			int id = scopeOffset_nodeIDInIfStmt[scopeOffset_nodeIDInIfStmt.size() - 1];
			nodes[node_id].AddDoIfStmt_id(id);
		}
		if (ifstmt) {
			scopeOffset_nodeIDInIfStmt.push_back(node_id);
		}
	}

	void PushBackNodes(int index) {
		Node node = nodes[index];
		nodes.erase(nodes.begin() + index);
		int size = nodes.size();
		for (int i = index; i < size; i++) {
			nodes[i].EditNode(index, size);
		}
		node.id = size;
		nodes.push_back(node);
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

	static void CopyVariableInOut(CDFD* copyTo, CDFD* original) {
		copyTo->nodes[copyTo->nodes.size() - 1].CopyVariableInOut(&original->nodes);
	}

	static void CopyConditionText(CDFD* copyTo, CDFD* original, bool* forCondition) {
		Node* copyNodeTo = &copyTo->nodes[copyTo->nodes.size() - 1];
		int index = 0;
		switch (copyNodeTo->processType)
		{
		case FORLOOP:
			if (forCondition == NULL) {
				
			}
			else if (forCondition[1]) {
				for (index = 0; index < original->nodes.size(); index++) {
					if (original->nodes[index].processType != DATESTORE)
						break;
				}
				if (forCondition[0]) {
					index++;
				}
				Node::CopyConditionText(&copyTo->nodes[copyTo->nodes.size() - 1], original->nodes[index]);
			}
			else {
				Node::CopyConditionText_NoCondition(&copyTo->nodes[copyTo->nodes.size() - 1]);
			}
			break;
		case WHILELOOP:
			for (index = 0; index < original->nodes.size(); index++) {
				if (original->nodes[index].processType != DATESTORE)
					break;
			}
			Node::CopyConditionText(&copyTo->nodes[copyTo->nodes.size() - 1], original->nodes[index]);
			break;
		default:
			break;
		}
	}

	/*具体化されるCDFDのIDをnodeにset*/
	void SetConcreteCDFD(int id) {
		nodes[node_id - 1].SetconcreteCDFD_id(id);
	}

	Offset GetNodeOffset(int id) {
		return nodes[id].offset;
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
			nodes[i].SetPosX(&nodes);
			nodes[i].SetNodeSize(&nodes, &myDraw);
			maxWidthList[nodes[i].pos.x] = nodes[i].GetWidth(&maxWidthList);
		}
		for (int i = 0; i < nodes.size(); i++) {
			nodes[i].SetPosY(nodes);
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
			for (int index = 0; index < variableName.size(); ++index) {
				printfDx("%s ", variableType[index].c_str());
				printfDx("%s => [", variableName[index].c_str());
				for (int j = 0; j < variableRelation[index].size(); j++) {
					printfDx("%d, ", variableRelation[index][j]);
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
				nodes[i].DrawNode(&maxWidthList, cursor_x, cursor_y, &myDraw, &nodes);
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