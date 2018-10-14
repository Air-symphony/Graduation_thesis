#pragma once
#include "CDFD.cpp"

class Map {
public:
	/*�����\���̒��ۉ��ȂǂɎg�p*/
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

	/*���ۉ�����͈͂�ۑ��A�u��*/
	CDFD AddCDFD(int begin, int end) {
		scopeOffset.AddOffset(begin, end);
		int id = map.size();
		CDFD cdfd(id);
		map.push_back(cdfd);
		return map[id - 1];
	}

	/*���ۉ�����ׂ��͈͂𔲂��Ă��邩�ǂ���*/
	bool CheckNodeInOffset(Node node) {
		int this_scope = scopeOffset.CheckOffset(node.offset);
		return (this_scope < scope);
	}
};