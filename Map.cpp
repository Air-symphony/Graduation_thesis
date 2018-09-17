#pragma once
#include "Arrow.cpp"
#include "Node.cpp"

class Map {
private:
	/*Node�̂܂Ƃ�*/
	vector<Node> map;
	/*�ϐ����X�g*/
	vector<string> variableType;
	vector<string> variableName;
	vector<vector<int>> variableRelation;
	/*Debug*/
	int index = 0; bool debug = false;
	int count = 0;
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
		/*debug���[�h�ؑ�*/
		if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
			count++;
			if (count == 1)
				debug = !debug;
		}
		else {
			count = 0;
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
	int id = 0;
	OffsetList scopeOffset;
	OffsetList exprOffset;
	int mid, mstate;
	int equalCount, operatorCount;
	bool unaryFlag;
	bool comparisonFlag;
	bool assignmentFlag;
	bool callFlag;
	bool binaryLock, varLock, varFlag;

	Map() {
		init();
	}

	void init() {
		id = 0;
		mid = mstate = -1;
		equalCount = operatorCount = 0;
		unaryFlag = comparisonFlag = assignmentFlag = callFlag = false;
		binaryLock = varLock = varFlag = false;
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

	void CheckScope(Node *node) {
		int scope = scopeOffset.CheckOffset(node->offset);
		node->addScope(scope);
	}

	void CheckExpression(Node *node) {
		int state = exprOffset.CheckOffset(node->offset);
		node->addState(state);
	}

	void AddMap(Node node) {
		map.push_back(node);
		id++;
	}

	bool OpenLock(Node node) {
		if (node.state <= mstate) {
			mstate = -1;
			binaryLock = varLock = unaryFlag = callFlag = 
				varFlag = comparisonFlag = assignmentFlag = false;
			return true;
		}
		return false;
	}

	bool SetNodeAbility(Node *node, bool input = true) {
		if (mid == id || mstate < 0) return false;

		/*������艺�̏��̏ꍇ*/
		if (varFlag) {
			map[mid].AddInput(node->variableName);
			return true;
		}
		else if (unaryFlag) {
			map[mid].AddInput(node->variableName);
			map[mid].AddOutput(node->variableName);
			//unaryFlag = false;
			return true;
		}
		else if (comparisonFlag) {
			map[mid].AddInput(node->variableName);
			return true;
		}
		else if (callFlag) {
			map[mid].AddInput(node->variableName);
			return true;
		}
		else if (operatorCount > 0) {
			bool insert = false;
			if (assignmentFlag) {
				insert = map[mid].AddOutput(node->variableName);
				assignmentFlag = (equalCount > 1);
			}
			else {
				insert = map[mid].AddInput(node->variableName);
			}
			if (insert) {
				operatorCount--;
				equalCount--;
			}
			return true;
		}
		return true;
	}

	int AddVariableName(int _id, string type, string variable) {
		if (variable == "") return 0;

		auto itr = std::find(variableName.begin(), variableName.end(), variable);//�ŏ��̗v�f�̃C�e���[�^��Ԃ�
		if (itr == variableName.end()) {//�V�K�̕ϐ��̏ꍇ
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
		if (index != variableName.size()) { // �����ł����Ƃ�
			if (_id - 1 < 0 || map.size() <= _id - 1) {
				return -1;
			}
			variableRelation[index].push_back(_id);
			//map[_id - 1].SetDefIndex(variableRelation[index][0]);
			return 1;
		}
		return 0;
	}

	void Draw() {
		int size = getDebugSize();
		if (!debug) {
			for (int i = index; i < size; i++) {
				//printfDx("%d : ", i);
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
			Arrow arrow;
			arrow.SetState(100, 100, 100, 300, "test");
			arrow.Draw();
			Arrow arrow2;
			arrow2.SetState(100, 100, 300, 300, "test2");
			arrow2.Draw();
			Arrow arrow3;
			arrow3.SetState(100, 100, 300, 100, "test3");
			arrow3.Draw();
		}
	}
};