#pragma once
#include <vector>       // �w�b�_�t�@�C���C���N���[�h

class Offset {
public:
	int begin, end;
	Offset() {
		begin = end = -1;
	}
	Offset(int _begin, int _end) {
		begin = _begin;
		end = _end;
	}
	void Set(int _begin, int _end) {
		begin = _begin;
		end = _end;
	}
	~Offset() {
		begin = end = -1;
	}
};

class OffsetList {
private:
	std::vector<Offset> list;
public:
	OffsetList() {
		init();
	}

	void init() {
		for (int i = (int)list.size(); i > 0; i--) {
			list.pop_back();
		}
	}

	void AddOffset(Offset offset) {
		list.push_back(offset);
	}

	void AddOffset(int begin, int end) {
		Offset offset(begin, end);
		list.push_back(offset);
	}

	int CheckOffset(Offset offset) {
		return CheckOffset(offset.begin, offset.end);
	}

	int CheckOffset(int begin, int end) {
		int count = 0;
		for (int i = (int)list.size() - 1; i >= 0; i--) {
			if (list[i].begin <= begin &&
				end <= list[i].end) {
				count++;
			}
			if (list[i].end < begin) {
				list.pop_back();
			}
		}
		return count;
	}
};