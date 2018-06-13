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
public:
	int memory;
	Map() {
		init();
	}

	void init() {
		memory = 0;
		for (int i = (int)map.size(); i > 0; i--) {
			map.pop_back();
		}
		for (int i = (int)variableName.size(); i > 0; i--) {
			variableName.pop_back();
		}
		for (int i = (int)variableRelation.size(); i > 0; i--) {
			variableRelation.pop_back();
		}
	}

	void AddMap(Node node) {
		map.push_back(node);
	}

	int AddVariableName(int line, string type, string variable) {
		if (variable == "") return 0;

		auto itr = std::find(variableName.begin(), variableName.end(), variable);//最初の要素のイテレータを返す
		if (itr == variableName.end()) {//新規の変数の場合
			variableName.push_back(variable);
			variableType.push_back(type);
			vector<int> list{ line };
			variableRelation.push_back(list);
		}
		return 1;
	}

	int AddVariableRelation(int line, string variable) {
		auto itr = find(variableName.begin(), variableName.end(), variable);
		int index = (int)distance(variableName.begin(), itr);
		if (index != variableName.size()) { // 発見できたとき
			if (line - 1 < 0 || map.size() <= line - 1) {
				printfDx("");
			}
			//variableRelation[index].push_back(line);
			//map[line - 1].SetDefIndex(variableRelation[index][0]);
			return 1;
		}
		return 0;
	}

	int index = 0;//debug
	void Draw() {
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
		}
		for (int i = index; i < size; i++) {
			printfDx("%d : ", i + 1);
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
		printfDx("\n");
	}
};