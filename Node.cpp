#pragma once
#include <vector>       // ヘッダファイルインクルード
#include <string>
using namespace std;         //  名前空間指定

class Node {
private:
	int id, start, end;
	string type;
	string text;
	int defIndex;
public:
	Node(int _id, int _start, int _end, string _type, string _text) {
		id = _id;
		start = _start;
		end = _end;
		type = _type;
		text = _text;
		defIndex = 0;
	}

	void SetText(string _text) {
		text = _text;
	}

	void SetDefIndex(int _defIndex) {
		defIndex = _defIndex;
	}

	string DrawNode() {
		string str = "(" + std::to_string(id) + "," + to_string(start) + "-" + to_string(end) + ")";
		str += "def(" + to_string(defIndex) + ")";
		str += "<" + type + ">";
		str += text + "\n";
		return str;
	}
};
