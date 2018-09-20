#pragma once
#include "Arrow.cpp"
#include "Node.cpp"

#define INPUT 1
#define OUTPUT 2
#define INOUTPUT 3

class Map {
private:
	/*Nodeのまとめ*/
	vector<Node> map;
	/*変数リスト*/
	vector<string> variableType;
	vector<string> variableName;
	vector<vector<int>> variableRelation;
	/*Debugモード切替*/
	bool ChangeDebugMode() {
		if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
			count++;
			if (count == 1) {
				debug = !debug;
				index = 0;
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
			if (index > 0) {
				index--;
			}
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
			if (index < map.size() - range) {
				index++;
			}
		}
		printfDx("[%d]\n", map.size());

		int size = index + range;
		if (map.size() < size) {
			size = (int)map.size();
			index = size - range;
			if (index < 0) index = 0;
		}
		return size;
	}
public:
	/*Debug*/
	int index = 0, count = 0; bool debug = false;

	int id = 0;
	OffsetList scopeOffset;
	OffsetList exprOffset;
	int preId, preScope, preState;
	bool inoutputFlag, inputFlag, outputFlag;
	bool assignmentFlag, compoundAssignFlag, ifstmtFlag;
	int equalCount, operatorCount;
	bool binaryLock, DeclLock;

	Map() {
		init();
	}

	void init() {
		id = 0;
		preId = preState = preScope = -1;
		equalCount = operatorCount = 0;
		outputFlag = inoutputFlag = inputFlag = assignmentFlag = compoundAssignFlag = ifstmtFlag = false;
		binaryLock = DeclLock = false;
		int i = 0;
		for (i = (int)map.size(); i > 0; i--) {
			map.pop_back();
		}
		for (i = (int)variableName.size(); i > 0; i--) {
			variableName.pop_back();
		}
		for (i = (int)variableRelation.size(); i > 0; i--) {
			variableRelation.pop_back();
		}
		scopeOffset.init();
		exprOffset.init();
	}

	/*for{}、if{}などのインデントの深さ*/
	void CheckScope(Node *node, bool ifstmt = false) {
		int scope = scopeOffset.CheckOffset(node->offset, ifstmt);
		node->addScope(scope);
	}
	/*コードに対する詳細の深さ*/
	void CheckExpression(Node *node) {
		int state = exprOffset.CheckOffset(node->offset);
		node->addState(state);
	}

	void AddMap(Node node) {
		map.push_back(node);
		id++;
	}
	/*今から作業するコードが、以前のノードに内包されていなければ、全てのフラグを解除*/
	bool OpenLock(Node node) {
		if (node.scope <= preScope && 
			node.state <= preState) {
			preState = -1;
			binaryLock = DeclLock = inoutputFlag = inputFlag = 
				outputFlag = assignmentFlag = ifstmtFlag = false;
			return true;
		}
		return false;
	}
	/*
	preId = id;
	preState = state;
	preScope = scope;
	*/
	void Save_id_state_scope(int state, int scope) {
		preId = id;
		preState = state;
		preScope = scope;
	}
	/*
	command = INPUT, OUTPUT, INOUTPUT*/
	bool AddInOut_PreNode(Node node, int command) {
		if (command == INPUT) {
			return map[preId].AddInput(node.variableName);
		}
		else if(command == OUTPUT) {
			return map[preId].AddOutput(node.variableName);
		}
		else if (command == INOUTPUT) {
			return (map[preId].AddInput(node.variableName) &&
				map[preId].AddOutput(node.variableName));
		}
		return false;
	}

	bool SetText_PreNode(Node node) {
		map[preId].text = node.text;
		return true;
	}

	/*各ノードに入出力される変数を指定
	指定できたということは、そのノードは表示する必要がない*/
	bool SetNodeAbility(Node node, bool input = true) {
		if (preId == id || preState < 0) return false;

		/*自分より下の情報の場合*/
		if (inoutputFlag) {
			AddInOut_PreNode(node, INOUTPUT);
		}
		else if (outputFlag) {
			AddInOut_PreNode(node, OUTPUT);
		}
		else if (inputFlag) {
			AddInOut_PreNode(node, INPUT);
		}
		else if (operatorCount > 0) {
			bool insert = false;
			if (assignmentFlag) {
				insert = AddInOut_PreNode(node, OUTPUT);
			}
			else {
				insert = AddInOut_PreNode(node, INPUT);
			}
			if (insert) {
				equalCount--;
				assignmentFlag = (equalCount > 0);
				operatorCount--;
			}
		}
		return true;
	}

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
			if (_id - 1 < 0 || map.size() <= _id - 1) {
				return -1;
			}
			variableRelation[index].push_back(_id);
			//map[_id - 1].SetDefIndex(variableRelation[index][0]);
			return 1;
		}
		return 0;
	}

	/*マップの表示*/
	bool Draw() {
		if (map.size() <= 0) return false;

		ChangeDebugMode();
		if (debug) {
			int size = getDebugSize();
			for (int i = index; i < size; i++) {
				printfDx(map[i].DrawNode().c_str());
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
			MyDrawString myDraw(20);
			int max = 0, y = map[0].GetHeight();
			for (int i = 0; i < map.size();i++) {
				int x = map[i].GetWidth(&myDraw);
				map[i].DrawNode(&myDraw,
					100 + x / 2,
					50 + y / 2 + i * (y + 10) - index
				);
				max += y;
			}

			if (CheckHitKey(KEY_INPUT_UP) != 0) {
				if (index > 0) {
					index -= 10;
				}
			}
			else if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
				if (index < max) {
					index += 10;
				}
			}
			printfDx("[%d]\n", index);
		}
		return true;
	}
};