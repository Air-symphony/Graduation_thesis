#pragma once
#include "Graphics.cpp"
#include "Offset.cpp"
#include <string>
using namespace std;         //  ���O��Ԏw��

/*�m�[�h�̎��*/
enum ProcessType {
	NORMAL,
	BRANCH_STANDARD,
	BRANCH_DEFAULT,
	FORLOOP,
	WHILELOOP
};

class Node {
private:
	static Graph processGraph, process_LeftGraph, process_RightGraph;
	static Graph loopProcess_RightGraph;
	static Graph branchProcess_StandardGraph, branchProcess_DefaultGraph;
	string type;
	vector<string> output;
	vector<string> input;
	vector<int> block;
	ProcessType processType;
public:
	int id;
	int width, height;
	string text;
	Offset offset;
	int scope, state;
	string variableName;
	/*��̉������CDFD��ID*/
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

	static bool CopyVariableInOut(Node* copyTo, Node original) {
		for (int i = 0; i < original.input.size(); i++) {
			copyTo->AddInput(original.input[i]);
		}
		if (original.type != "VarDecl" &&
			original.type != "FieldDecl" &&
			original.type != "ParmDecl") {
			for (int i = 0; i < original.output.size(); i++) {
				copyTo->AddOutput(original.output[i]);
			}
		}
		return true;
	}

	bool ChangeProcessType(ProcessType _type) {
		processType = _type;
		return true;
	}

	bool SetconcreteCDFD_id(int id) {
		concreteCDFD_id = id;
		switch (processType)
		{
		case FORLOOP:
			text = "For_" + to_string(concreteCDFD_id) + "\n(" + text + ")";
			break;
		case WHILELOOP:
			text = "While_" + to_string(concreteCDFD_id) + "\n(" + text + ")";
			break;
		default:
			break;
		}
		return true;
	}

	bool AddNode(int id) {
		block.push_back(id);
		return true;
	}
	/*�m�[�h�̏��̕����f�[�^*/
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
		str += "] [";
		for (int i = 0; i < block.size(); i++) {
			str += to_string(block[i]) + ",";
		}
		str += "]";
		return str + "\n";
	}
	/*�m�[�h�̑傫����ݒ�*/
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
	/*�m�[�h�̐}��*/
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
			process_LeftGraph.DrawExtend(x - width / 2, y, process_LeftGraph.sizeX, height, 6);
			loopProcess_RightGraph.DrawExtend(x + width / 2, y, loopProcess_RightGraph.sizeX, height, 4);
			myDraw->Draw_String_Black(x, y, text, 8);
			break;
		default:
			break;
		}

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
		/*�m�[�h�̑傫����\��*/
		//myDraw->Draw_String_White(x + width / 2, y - height / 2, to_string(width) + "�~" + to_string(height), 7);

		return true;
	}
};
