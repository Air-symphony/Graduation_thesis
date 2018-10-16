#pragma once
#include "Graphics.cpp"
#include "Offset.cpp"
#include <string>
using namespace std;         //  名前空間指定

/*ノードの種類*/
enum ProcessType {
	NORMAL,
	BRANCH_STANDARD,
	BRANCH_DEFAULT,
	LOOP,
};

class Node {
private:
	static Graph processGraph, process_LeftGraph, process_RightGraph;
	static Graph loopProcess_RightGraph;
	static Graph branchProcess_StandardGraph, branchProcess_DefaultGraph;
	int id;
	string type;
	vector<string> output;
	vector<string> input;
	vector<Node*> block;
	ProcessType processType;
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
		processType = ProcessType::NORMAL;
	}

	static void SetGraph() {
		Node::processGraph.SetGraph(LoadGraph("picture\\Process.png"));
		Node::process_LeftGraph.SetGraph(LoadGraph("picture\\Process_Left.png"));
		Node::process_RightGraph.SetGraph(LoadGraph("picture\\Process_Right.png"));
		Node::loopProcess_RightGraph.SetGraph(LoadGraph("picture\\Process_Right_Loop.png"));
		Node::branchProcess_StandardGraph.SetGraph(LoadGraph("picture\\BranchProcess_Standard.png"));
		Node::branchProcess_DefaultGraph.SetGraph(LoadGraph("picture\\BranchProcess_Default.png"));
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

	bool ChangeProcessType(ProcessType _type) {
		processType = _type;
		return true;
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
	/*ノードの大きさを設定*/
	void SetNodeSize(MyDrawString* myDraw) {
		switch (processType)
		{
		case NORMAL:
			width = myDraw->GetTextWidth(text) + (process_LeftGraph.sizeX + process_RightGraph.sizeX);
			height = processGraph.sizeY;
			break;
		case BRANCH_STANDARD:
			width = myDraw->GetTextWidth(text) + (process_LeftGraph.sizeX + process_RightGraph.sizeX);
			height = branchProcess_StandardGraph.sizeY;
			break;
		case BRANCH_DEFAULT:
			width = myDraw->GetTextWidth(text) + (process_LeftGraph.sizeX + process_RightGraph.sizeX);
			height = branchProcess_DefaultGraph.sizeY;
			break;
		case LOOP:
			width = myDraw->GetTextWidth(text) + (process_LeftGraph.sizeX + loopProcess_RightGraph.sizeX);
			height = processGraph.sizeY;
			break;
		default:
			break;
		}
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
		
		switch (processType)
		{
		case NORMAL:
			processGraph.DrawExtend(x, y, width, height);
			process_LeftGraph.DrawExtend(x - width / 2, y, process_LeftGraph.sizeX, height, 6);
			process_RightGraph.DrawExtend(x + width / 2, y, process_RightGraph.sizeX, height, 4);
			break;
		case BRANCH_STANDARD:
			branchProcess_StandardGraph.DrawExtend(x, y, width, height);
			break;
		case BRANCH_DEFAULT:
			branchProcess_DefaultGraph.DrawExtend(x, y, width, height);
			break;
		case LOOP:
			processGraph.DrawExtend(x, y, width, height);
			process_LeftGraph.DrawExtend(x - width / 2, y, process_LeftGraph.sizeX, height, 6);
			loopProcess_RightGraph.DrawExtend(x + width / 2, y, loopProcess_RightGraph.sizeX, height, 4);
			break;
		default:
			break;
		}
		myDraw->Draw_String_Black(x, y, text);

		int inoutPos = 10;
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
		/*ノードの大きさを表示*/
		//myDraw->Draw_String_White(x + width / 2, y - height / 2, to_string(width) + "×" + to_string(height), 7);

		return true;
	}
};
