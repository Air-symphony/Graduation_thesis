#pragma once
#include <vector>       // ヘッダファイルインクルード

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

	int CheckOffset(Offset offset, bool ifstmt = false) {
		return CheckOffset(offset.begin, offset.end, ifstmt);
	}

	int CheckOffset(int begin, int end, bool ifstmt = false) {
		int count = 0;
		for (int i = (int)list.size() - 1; i >= 0; i--) {
			if (list[i].begin <= begin &&
				end <= list[i].end) {
				//if(){}else if(){}を並列させるため
				if (ifstmt && end == list[i].end) {
					list.pop_back();
				}
				//scope内
				else {
					count++;
				}
			}
			else
				if (list[i].end < begin) {
					list.pop_back();
				}
		}
		return count;
	}

	bool CheckOffset_ElseStmt(Offset offset) {
		for (int i = (int)list.size() - 1; i >= 0; i--) {
			if (list[i].begin <= offset.begin &&
				offset.end == list[i].end) {
					return true;
			}
		}
		return false;
	}
};