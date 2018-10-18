#pragma once
#include "Arrow.cpp"
#include "Offset.cpp"
#include <string>
using namespace std;         //  名前空間指定

/*ノードの種類*/
enum ProcessType {
	NORMAL,
	BRANCH_STANDARD,
	BRANCH_DEFAULT,
	FORLOOP,
	WHILELOOP
};

class Pos {
public:
	int x, y;
	Pos() {
		x = y = 0;
	}
	Pos(int _x, int _y) {
		x = _x;
		y = _y;
	}
	void SetPos(int _x, int _y) {
		x = _x;
		y = _y;
	}
};
class Node {
private:
	static Graph processGraph, process_LeftGraph, process_RightGraph;
	static Graph loopProcess_RightGraph;
	static Graph branchProcess_StandardGraph, branchProcess_DefaultGraph;
	string type;
	vector<string> output;
	vector<string> input;
	vector<int> connectNodeID;
	vector<int> block;
	ProcessType processType;
public:
	int id;
	Pos pos;
	int width, height;
	string text;
	Offset offset;
	int scope, state;
	string variableName;
	/*具体化されるCDFDのID*/
	int concreteCDFD_id;
	//unsigned level;

	Node(int _id, int _begin, int _end, string _type, string _text, string _variableName, unsigned _level = 0) {
		pos.SetPos(1, 1);
		width = height = 0;
		id = _id;
		scope = state = -1;
		offset.Set(_begin, _end);
		type = _type;
		text = _text;
		variableName = _variableName;
		concreteCDFD_id = -1;
		//level = _level;
		processType = ProcessType::NORMAL;
		for (int i = (int)block.size(); i > 0; i--) {
			block.pop_back();
		}
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
		if ((int)variable.size() == 0) return false;

		auto itr = std::find(input.begin(), input.end(), variable);//最初の要素のイテレータを返す
		if (itr == input.end()) {//新規の変数の場合
			input.push_back(variable);
			return true;
		}
		return false;
	}

	bool AddOutput(string variable) {
		if ((int)variable.size() == 0) return false;

		auto itr = std::find(output.begin(), output.end(), variable);//最初の要素のイテレータを返す
		if (itr == output.end()) {//新規の変数の場合
			output.push_back(variable);
			return true;
		}
		return false;
	}

	static bool CopyVariableInOut(Node* copyTo, Node original) {
		for (int i = 0; i < (int)original.input.size(); i++) {
			copyTo->AddInput(original.input[i]);
		}
		if (original.type != "VarDecl" &&
			original.type != "FieldDecl" &&
			original.type != "ParmDecl") {
			for (int i = 0; i < (int)original.output.size(); i++) {
				copyTo->AddOutput(original.output[i]);
			}
		}
		return true;
	}

	static bool CopyConditionText(Node* copyTo, Node original) {
		switch (copyTo->processType)
		{
		case FORLOOP:
			copyTo->text = "For_" + to_string(copyTo->concreteCDFD_id) + "\n(" + original.text + ")";
			break;
		case WHILELOOP:
			copyTo->text = "While_" + to_string(copyTo->concreteCDFD_id) + "\n(" + original.text + ")";
			break;
		default:
			break;
		}
		return true;
	}

	bool ChangeProcessType(ProcessType _type) {
		processType = _type;
		return true;
	}

	bool SetconcreteCDFD_id(int id) {
		concreteCDFD_id = id;
		return true;
	}

	bool AddNode(int id) {
		block.push_back(id);
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
		//str += variableName + " ";
		//str += ":" + to_string(concreteCDFD_id) + ": ";
		//str += "<" + type + ">";
		str += "[" + text + "]";

		for (int i = 0; i < (int)connectNodeID.size(); i++) {
			str += to_string(connectNodeID[i]) + ",";
		}
		str += "[";
		for (int i = 0; i < (int)output.size(); i++) {
			str += output[i] + ",";
		}
		str += "<= ";
		for (int i = 0; i < (int)input.size(); i++) {
			str += input[i] + ",";
		}
		str += "]";
		str += "(" + to_string(pos.x) + "," + to_string(pos.y) + ")";
		/*
		str += " [";
		for (int inIndex = 0; inIndex < block.size(); inIndex++) {
			str += to_string(block[inIndex]) + ",";
		}
		str += "]";
		*/
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
		case FORLOOP:
		case WHILELOOP:
			width = myDraw->GetTextWidth(text) + (process_LeftGraph.sizeX + loopProcess_RightGraph.sizeX);
			if (processGraph.sizeY < myDraw->GetTextHeight() * 2) {
				height = myDraw->GetTextHeight() * 2;
			}
			else {
				height = processGraph.sizeY;
			}
			break;
		default:
			break;
		}
	}
	
	int GetPosX(vector<int>* maxWidthList) {
		int x = maxWidthList->at(0);
		for (int i = 1; i <= pos.x; i++) {
			if (i == pos.x) {
				x += maxWidthList->at(i) / 2;
			}
			else {
				x += maxWidthList->at(i) + 100;
			}
		}
		return x;
	}

	int GetPosY() {
		return pos.y * 100;
	}
	/*ノードの図示*/
	bool DrawNode(vector<int>* maxWidthList, int cursor_x, int cursor_y, MyDrawString* myDraw) {
		int x = GetPosX(maxWidthList) - cursor_x, y = GetPosY() - cursor_y;
		switch (processType)
		{
		case NORMAL:
			processGraph.DrawExtend(x, y, width, height);
			process_LeftGraph.DrawExtend(x - width / 2, y, process_LeftGraph.sizeX, height, 5);
			process_RightGraph.DrawExtend(x + width / 2, y, process_RightGraph.sizeX, height, 5);
			myDraw->Draw_String_Black(x, y, text);
			break;
		case BRANCH_STANDARD:
			branchProcess_StandardGraph.DrawExtend(x, y, width, height);
			myDraw->Draw_String_Black(x, y, text);
			break;
		case BRANCH_DEFAULT:
			branchProcess_DefaultGraph.DrawExtend(x, y, width, height);
			myDraw->Draw_String_Black(x, y, text);
			break;
		case FORLOOP:
		case WHILELOOP:
			processGraph.DrawExtend(x, y, width, height);
			process_LeftGraph.DrawExtend(x - width / 2, y, process_LeftGraph.sizeX, height, 5);
			loopProcess_RightGraph.DrawExtend(x + width / 2, y, loopProcess_RightGraph.sizeX, height, 5);
			myDraw->Draw_String_Black(x, y, text, 8);
			break;
		default:
			break;
		}
		/*ノードの大きさを表示*/
		//myDraw->Draw_String_White(x + width / 2, y - height / 2, to_string(width) + "×" + to_string(height), 7);

		return true;
	}

	/*変数の矢印を描画*/
	bool DrawArrow(vector<int>* maxWidthList, int cursor_x, int cursor_y, MyDrawString* myDraw, vector<Node> nodes) {
		int inputSize = (int)input.size(), outputSize = (int)output.size();
		int limitSize = 3;
		if (limitSize < inputSize || limitSize < outputSize) {
			int size = (inputSize < outputSize) ? outputSize : inputSize;
			height += myDraw->GetTextHeight() * (size - limitSize);
		}

		int inoutPos = 20;
		for (int inIndex = 0; inIndex < (int)connectNodeID.size(); inIndex++) {
			vector<string> connectOutput = nodes[connectNodeID[inIndex]].output;
			int size = (int)connectOutput.size();
			int output_x = 0, output_y = 0;
			for (int outIndex = 0; outIndex < size; outIndex++) {
				if (connectOutput[outIndex] == input[inIndex]) {
					Node* node = &nodes[connectNodeID[inIndex]];
					int dy = node->height / (int)(size + 1);
					output_x = node->GetPosX(maxWidthList) + node->width / 2 - cursor_x;
					output_y = node->GetPosY() - node->height / 2 + dy * (outIndex + 1) - cursor_y;
					myDraw->Draw_String_White(output_x + inoutPos, output_y - inoutPos, connectOutput[outIndex], 4);
					break;
				}
			}
			int dy = height / (int)(inputSize + 1);
			int input_x = GetPosX(maxWidthList) - width / 2  - cursor_x;
			int input_y = GetPosY() - height / 2 + dy * (inIndex + 1) - cursor_y;
			Arrow::Draw(output_x, output_y, input_x, input_y);
			myDraw->Draw_String(input_x - inoutPos, input_y - inoutPos, input[inIndex],GetColor(255,255,0), 6);
		}
		/*
		int maxOutPutWidth = 0;
		for (int i = 0; i < outputSize; i++) {
			int dy = height / (int)(outputSize + 1);
			//outputPos.push_back(Pos(x + width / 2 + inoutPos, y - height / 2 + dy * (inIndex + 1)));
			myDraw->Draw_String_White(x + width / 2 + inoutPos, y - height / 2 + dy * (i + 1), output[i], 4);

			int w = myDraw->GetTextWidth(output[i]);
			maxOutPutWidth = (w > maxOutPutWidth) ? w : maxOutPutWidth;
		}
		*/
		return true;
	}

	bool SetPosX(vector<Node> nodes) {
		bool check = false;
		for (int inIndex = 0; inIndex < (int)input.size(); inIndex++) {
			bool skip = true;
			for (int nodeID = id - 1; nodeID >= 0 && skip; nodeID--) {
				int size = (int)nodes[nodeID].output.size();
				for (int outIndex = 0; outIndex < size && skip; outIndex++) {
					if (nodes[nodeID].output[outIndex] == input[inIndex]) {
						if (pos.x < nodes[nodeID].pos.x + 1) {
							pos.x = nodes[nodeID].pos.x + 1;
							check = true;
						}
						connectNodeID.push_back(nodeID);
						skip = false;
					}
				}
			}
		}
		return check;
	}
	void SetPosY(vector<Node> nodes) {
		for (int nodeID = id - 1; nodeID >= 0; nodeID--) {
			if (pos.x == nodes[nodeID].pos.x) {
				pos.y = nodes[nodeID].pos.y + 1;
				break;
			}
		}
	}
	int GetWidth(vector<int>* maxWidthList) {
		for (int i = maxWidthList->size() - 1; i < pos.x; i++) {
			maxWidthList->push_back(0);
		}
		if (maxWidthList->at(pos.x) < width) {
			return width;
		}
		return maxWidthList->at(pos.x);
	}
};
