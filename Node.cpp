#pragma once
#include "Offset.cpp"
#include <string>
using namespace std;         //  ���O��Ԏw��

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
	string variableName;

	Node(int _id, int _begin, int _end, string _type, string _text, string _variableName) {
		id = _id;
		parentID = scope = state = -1;
		offset.Set(_begin, _end);
		type = _type;
		text = _text;
		variableName = _variableName;
	}

	void addScope(int _scope) {
		scope = _scope;
	}

	void addState(int _state) {
		state = _state;
	}

	bool AddInput(string variable) {
		if (variable.size() == 0) return false;

		auto itr = std::find(input.begin(), input.end(), variable);//�ŏ��̗v�f�̃C�e���[�^��Ԃ�
		if (itr == input.end()) {//�V�K�̕ϐ��̏ꍇ
			input.push_back(variable);
			return true;
		}
		return false;
	}

	bool AddOutput(string variable) {
		if (variable.size() == 0) return false;

		auto itr = std::find(output.begin(), output.end(), variable);//�ŏ��̗v�f�̃C�e���[�^��Ԃ�
		if (itr == output.end()) {//�V�K�̕ϐ��̏ꍇ
			output.push_back(variable);
			return true;
		}
		return false;
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
		str += to_string(id) + " : ";// (" + to_string(offset.begin) + " - " + to_string(offset.end) + ")";
		str += variableName + " ";
		str += "<" + type + ">";
		str += "[" + text + "]";
		
		str += "[";
		for (int i = 0; i < input.size(); i++) {
			str += input[i] + ",";
		}
		if (input.size() > 0) {
			//str.pop_back();
		}
		str += "=> ";
		for (int i = 0; i < output.size(); i++) {
			str += output[i] + ",";
		}
		if (output.size() > 0) {
			//str.pop_back();
		}
		str += "]";
		
		return str + "\n";
	}
};
