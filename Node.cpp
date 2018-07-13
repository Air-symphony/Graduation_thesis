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
	Node(int _id, int _start, int _end, string _type, string _text) {
		id = _id;
		start = _start;
		end = _end;
		type = _type;
		text = _text;
	}

	void AddInput(string variable) {
		input.push_back(variable);
	}

	void AddOutput(string variable) {
		output.push_back(variable);
	}

	string DrawNode() {
		string str = std::to_string(id) + " : (" + to_string(start) + "-" + to_string(end) + ")";
		//str += "def(" + to_string(defIndex) + ")";
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
