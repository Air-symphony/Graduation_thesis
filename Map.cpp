#pragma once
#include "DxLib.h"
#include "Node.cpp"

class Map {
private:
	/*Nodeのまとめ*/
	vector<Node> map;
	/*変数リスト*/
	vector<string> variableType;
	vector<string> variableName;
	vector<vector<int>> variableRelation;
	/*Debug*/
	int index = 0;
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
	int id = 0;
	int skipOffset;
	OffsetList scopeOffset;
	OffsetList exprOffset;

	Map() {
		init();
	}

	void init() {
		id = 0;
		skipOffset = -1;

		for (int i = (int)map.size(); i > 0; i--) {
			map.pop_back();
		}
		for (int i = (int)variableName.size(); i > 0; i--) {
			variableName.pop_back();
		}
		for (int i = (int)variableRelation.size(); i > 0; i--) {
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

	void Draw() {
		int size = getDebugSize();
		for (int i = index; i < size; i++) {
			//printfDx("%d : ", i);
			printfDx(map[i].DrawNode().c_str());
		}
		printfDx("\n");
		
		for (int i = 0; i < variableName.size(); ++i) {
			printfDx("%s ", variableType[i].c_str());
			printfDx("%s => [", variableName[i].c_str());
			for (int j = 0; j < variableRelation[i].size(); j++) {
				printfDx("%d, ", variableRelation[i][j]);
			}
			printfDx("]\n");
		}
	}
};