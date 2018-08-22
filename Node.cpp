#pragma once
#include "Offset.cpp"
#include <string>
using namespace std;         //  –¼‘O‹óŠÔŽw’è

class Node {
private:
	int id;
	string type;
	string text;
	vector<string> output;
	vector<string> input;
public:
	Offset offset;
	int scope, state;
	int parentID;

	Node(int _id, int _begin, int _end, string _type, string _text) {
		id = _id;
		parentID = scope = state = -1;
		offset.Set(_begin, _end);
		type = _type;
		text = _text;
	}

	void addScope(int _scope) {
		scope = _scope;
	}

	void addState(int _state) {
		state = _state;
	}

	void AddInput(string variable) {
		input.push_back(variable);
	}

	void AddOutput(string variable) {
		output.push_back(variable);
	}

	string DrawNode() {
		string str = "";
		for (int i = 0; i < scope; i++) {
			str += "--";
		}
		for (int i = 0; i < state; i++) {
			str += ">";
		}
		str += to_string(id) + " : ";// (" + to_string(offset.begin) + " - " + to_string(offset.end) + ")";
		str += "<" + type + ">";
		str += "[" + text + "]";
		/*
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
		str += "]";
		*/
		return str + "\n";
	}
};
