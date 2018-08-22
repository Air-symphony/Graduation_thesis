#pragma once
#include <vector>       // ヘッダファイルインクルード
#include <string>
using namespace std;         //  名前空間指定

class Node {
private:
	int id, start, end;
	string type;
	string text;
	vector<string> output;
	vector<string> input;
public:
	int level;

	Node(int _id, int _start, int _end, string _type, string _text) {
		id = _id;
		level = 0;
		start = _start;
		end = _end;
		type = _type;
		text = _text;
	}

	/*levelを加える*/
	void addLevel(int parentLevel, int parentBegin, int parentEnd) {
		if (parentBegin <= start && end <= parentEnd) {
			level = parentLevel;
			if ((parentBegin == start && end == parentEnd) == false) {
				level++;
			}
		}
	}

	void AddInput(string variable) {
		input.push_back(variable);
	}

	void AddOutput(string variable) {
		output.push_back(variable);
	}

	string DrawNode() {
		string str = "";
		for (int i = 0; i < level; i++) {
			str += "--";
		}
		str += std::to_string(id) + " : (" + to_string(start) + "-" + to_string(end) + ")";
		str += "<" + type + ">";
		str += " " + text + " ";
		str += "[";
		for (int i = 0; i < input.size(); i++) {
			str += input[i] + ",";
		}
		if (input.size() > 0) {
			//str.pop_back();
		}
		str += " => ";
		for (int i = 0; i < output.size(); i++) {
			str += output[i] + ",";
		}
		if (output.size() > 0) {
			//str.pop_back();
		}
		str += "]\n";

		return str;
	}
};
