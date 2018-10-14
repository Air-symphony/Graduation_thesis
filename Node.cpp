#pragma once
#include "Graphics.cpp"
#include "Offset.cpp"
#include <string>
using namespace std;         //  名前空間指定

class Node {
private:
	static Graph nodeGraph;
	static Graph nodeGraph_Left;
	static Graph nodeGraph_Right;
	int id;
	string type;
	vector<string> output;
	vector<string> input;
	vector<Node*> block;
public:
	int width, height;
	string text;
	Offset offset;
	int scope, state;
	string variableName;
	/*具体化されるCDFDのID*/
	int concreteCDFD_id;

	Node(int _id, int _begin, int _end, string _type, string _text, string _variableName) {
		width = height = 0;
		id = _id;
		scope = state = -1;
		offset.Set(_begin, _end);
		type = _type;
		text = _text;
		variableName = _variableName;
		concreteCDFD_id = -1;
		//nodeGraph.SetGraph(LoadGraph("picture\\Process.png"));
	}

	static void SetGraph() {
		Node::nodeGraph.SetGraph(LoadGraph("picture\\Process.png"));
		Node::nodeGraph_Left.SetGraph(LoadGraph("picture\\Process_Left.png"));
		Node::nodeGraph_Right.SetGraph(LoadGraph("picture\\Process_Right.png"));
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
		str += ":" + to_string(concreteCDFD_id) + ": ";
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
	void SetNodeSize(MyDrawString* myDraw) {
		width = 10 + myDraw->GetTextWidth(text) + (nodeGraph_Left.sizeX + nodeGraph_Right.sizeX) * 2;
		height = nodeGraph.sizeY * 2;
	}
	/*ノードの図示*/
	bool DrawNode(MyDrawString* myDraw, int x, int y) {
		x += scope * 20;
		int inputSize = (int)input.size(), outputSize = (int)output.size();
		int limitSize = 3;
		if (limitSize < inputSize || limitSize < outputSize) {
			int size = (inputSize < outputSize) ? outputSize : inputSize;
			height += myDraw->GetTextHeight() * (size - limitSize);
		}
		
		nodeGraph.DrawExtend(x, y, width, height);
		nodeGraph_Left.DrawExtend(x - width / 2, y, nodeGraph_Left.sizeX * 2, height, 6);
		nodeGraph_Right.DrawExtend(x + width / 2, y, nodeGraph_Right.sizeX * 2, height, 4);
		myDraw->Draw_String_Black(x, y, text);

		int inoutPos = 20;
		for (int i = 0; i < inputSize; i++) {
			int dy = height / (int)(inputSize + 1);
			myDraw->Draw_String_White(x - width / 2 - inoutPos, y - height / 2 + dy * (i + 1), input[i], 6);
		}
		int maxOutPutWidth = 0;
		for (int i = 0; i < outputSize; i++) {
			int dy = height / (int)(outputSize + 1);
			myDraw->Draw_String_White(x + width / 2 + inoutPos, y - height / 2 + dy * (i + 1), output[i], 4);
			
			int w = myDraw->GetTextWidth(output[i]);
			maxOutPutWidth = (w > maxOutPutWidth) ? w : maxOutPutWidth;
		}

		return true;
	}
};
