#pragma once
#include "DxLib.h"
#include <vector>       // ヘッダファイルインクルード
using namespace std;         //  名前空間指定

class Node {
private:
	int id;
	int range;
	char* text;
	int linkID;
public:
	Node(int _id, int _range, char* _text, int _linkID) {
		id = _id;
		range = _range;
		text = _text;
		linkID = _linkID;
	}

	int DrawNode() {
		printfDx("<%d,%d,%d> ", id, range, linkID);
		printfDx(text);
		printfDx("\n");
		return 1;
	}
};

class Map {
private:
	vector<Node> map;
public:
	Map() {
		
	}

	void init() {
		if (map.size() > 0) {
			for (int i = 0; i < map.size(); ++i) {
				map.pop_back();
			}
		}
	}
	void Add(Node node) {
		map.push_back(node);
	}

	void Draw() {
		for (int i = 0; i < map.size(); ++i) {
			printfDx("%d -> ", i);
			map[i].DrawNode();
		}
	}
};