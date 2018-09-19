#pragma once
#include "Graphics.cpp"
#include "Offset.cpp"
#include <string>
using namespace std;         //  名前空間指定

class Node {
private:
	static Graph nodeGraph;
	int x, y;
	int id;
	string type;
	vector<string> output;
	vector<string> input;
	vector<Node*> block;
public:
	string text;
	Offset offset;
	int scope, state;
	string variableName;

	Node(int _id, int _begin, int _end, string _type, string _text, string _variableName) {
		x = y = 0;
		id = _id;
		scope = state = -1;
		offset.Set(_begin, _end);
		type = _type;
		text = _text;
		variableName = _variableName;
		nodeGraph.SetGraph(LoadGraph("picture\\node.png"));
	}

	void addScope(int _scope) {
		scope = _scope;
	}

	void addState(int _state) {
		state = _state;
	}

	bool AddInput(string variable) {
		if (variable.size() == 0) return false;

		auto itr = std::find(input.begin(), input.end(), variable);//最初の要素のイテレータを返す
		if (itr == input.end()) {//新規の変数の場合
			input.push_back(variable);
			return true;
		}
		return false;
	}

	bool AddOutput(string variable) {
		if (variable.size() == 0) return false;

		auto itr = std::find(output.begin(), output.end(), variable);//最初の要素のイテレータを返す
		if (itr == output.end()) {//新規の変数の場合
			output.push_back(variable);
			return true;
		}
		return false;
	}

	bool AddNode(Node* node) {
		block.push_back(node);
		return true;
	}

	string DrawNode() {
		string str = "";
		//str += "(" + to_string(scope) + "," + to_string(state) + ") ";
		for (int i = 0; i < scope; i++) {
			str += "--";
		}
		for (int i = 0; i < state; i++) {
			str += ">";
		}
		str += to_string(id) + " : " + "(" + to_string(offset.begin) + " - " + to_string(offset.end) + ")";
		str += variableName + " ";
		str += "<" + type + ">";
		str += "[" + text + "]";

		str += "[";
		for (int i = 0; i < output.size(); i++) {
			str += output[i] + ",";
		}
		str += "<= ";
		for (int i = 0; i < input.size(); i++) {
			str += input[i] + ",";
		}
		str += "]";

		return str + "\n";
	}
	
	bool DrawNode(int i) {
		return true;
	}
};
