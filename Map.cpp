#pragma once
#include "CDFD.cpp"

class Map {
public:
	/*反復構造の抽象化などに使用*/
	vector<CDFD> map;
	int cdfd_id;
	OffsetList scopeOffset;
	int scope;

	Map() {
		init();
	}

	void init() {
		cdfd_id = scope = 0;
		int i = 0;
		for (i = (int)map.size(); i > 0; i--) {
			map.pop_back();
		}
		scopeOffset.init();
	}

	/*抽象化する範囲を保存、隔離*/
	CDFD AddCDFD(int begin, int end) {
		scopeOffset.AddOffset(begin, end);
		int id = map.size();
		CDFD cdfd(id);
		map.push_back(cdfd);
		return map[id - 1];
	}

	/*抽象化するべき範囲を抜けているかどうか*/
	bool CheckNodeInOffset(Node node) {
		int this_scope = scopeOffset.CheckOffset(node.offset);
		return (this_scope < scope);
	}
};