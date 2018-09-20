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
		nodeGraph.SetGraph(LoadGraph("picture\\Process.png"));
	}

	static void SetGraph() {
		Node::nodeGraph.SetGraph(LoadGraph("picture\\Process.png"));
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
	/*ノードの情報の文字データ*/
	string DrawNode(bool debug = false) {
		string str = "";
		for (int i = 0; i < scope; i++) {
			str += "--";
		}
		for (int i = 0; i < state; i++) {
			str += ">";
		}
		str += to_string(id) + " : " + "(" + to_string(offset.begin) + " - " + to_string(offset.end) + ")";
		str += variableName + " ";
		if (debug) str += "\n";
		str += "<" + type + ">";
		str += "[" + text + "]";

		if (debug) str += "\n";
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
	/*表示するノードの幅*/
	int GetWidth(MyDrawString* myDraw) {
		return myDraw->GetTextSize(text);
	}
	/*表示するノードの高さ*/
	int GetHeight() {
		return nodeGraph.sizeY * 2;
	}
	/*ノードの図示*/
	bool DrawNode(MyDrawString* myDraw, int x, int y) {
		int width = myDraw->GetTextSize(text) + 40;
		int height = GetHeight();
		
		x += scope * 20;
		nodeGraph.DrawExtend(x, y, width, height);
		myDraw->Draw_String_Black(x, y, text);

		for (int i = 0; i < input.size(); i++) {
			int dy = height / (int)input.size();
			myDraw->Draw_String_White(x - width / 2 - 10, y + dy * (i - (int)input.size() / 2)  , input[i], 6);
		}
		int maxOutPutWidth = 0;
		for (int i = 0; i < output.size(); i++) {
			int dy = height / (int)output.size();
			myDraw->Draw_String_White(x + width / 2 + 10, y + dy * (i - (int)output.size() / 2), output[i], 4);
			
			int w = myDraw->GetTextSize(output[i]);
			maxOutPutWidth = (w > maxOutPutWidth) ? w : maxOutPutWidth;
		}
		myDraw->Draw_String_White(x + width / 2 + maxOutPutWidth + 25, y, DrawNode(true), 7);

		return true;
	}
};
