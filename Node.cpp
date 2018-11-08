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
	WHILELOOP,
	DATESTORE
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
	static Graph dateStore_Name, dateStore_Number;
	string type;
	vector<string> output;
	vector<string> input;
	/*inputの変数を受け付ける、送信元のノードIDリスト*/
	vector<int> connectNodeID;
	/*trueの場合に実行される分岐構造ノードID*/
	int doIfStmt_id;
public:
	ProcessType processType;
	int id;
	string text;
	Offset offset;
	int scope, state;
	string variableName;
	/*具体化されるCDFDのID*/
	int concreteCDFD_id;
	/*if,else文のつながりを持つID*/
	int connectIfStmt_id;
	/*CDFD上でのマス目の座標*/
	Pos pos;
	/*ノードのサイズ*/
	int width, height;
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
		concreteCDFD_id = connectIfStmt_id = doIfStmt_id  = -1;
		processType = ProcessType::NORMAL;
	}

	/*画像素材の準備*/
	static void SetGraph() {
		Node::processGraph.SetGraph(LoadGraph("picture\\Process.png"));
		Node::process_LeftGraph.SetGraph(LoadGraph("picture\\Process_Left.png"));
		Node::process_RightGraph.SetGraph(LoadGraph("picture\\Process_Right.png"));
		Node::loopProcess_RightGraph.SetGraph(LoadGraph("picture\\Process_Right_Loop.png"));
		Node::branchProcess_StandardGraph.SetGraph(LoadGraph("picture\\BranchProcess_Standard.png"));
		Node::branchProcess_DefaultGraph.SetGraph(LoadGraph("picture\\BranchProcess_Default.png"));
		Node::dateStore_Name.SetGraph(LoadGraph("picture\\DateStore_Name.png"));
		Node::dateStore_Number.SetGraph(LoadGraph("picture\\DateStore_Number.png"));
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

	bool CheckDeclStmt() {
		return type == "VarDecl" || type == "FieldDecl";
	}

	vector<string> escapeVariable;
	bool CopyVariableInOut(Node original) {
		if (original.processType != DATESTORE) {
			if (original.type == "VarDecl" ||
				original.type == "FieldDecl" ||
				original.type == "ParmDecl") {
				escapeVariable.push_back(original.variableName);
			}
		}
		for (int i = 0; i < (int)original.input.size(); i++) {
			bool check = false;
			for (int j = 0; j < (int)escapeVariable.size(); j++) {
				if (escapeVariable[j] == original.input[i]){
					check = true;
				}
			}
			if (!check)
				AddInput(original.input[i]);
		}
		for (int i = 0; i < (int)original.output.size(); i++) {
			bool check = false;
			for (int j = 0; j < (int)escapeVariable.size(); j++) {
				if (escapeVariable[j] == original.output[i]) {
					check = true;
				}
			}
			if (!check)
				AddOutput(original.output[i]);
		}
		return true;
	}

	/**For、Whileに合わせて、テキストを変更する
	*type + concreteCDFD_id + "\n(" + text + ")"
	*/
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

	static bool CopyConditionText_NoCondition(Node* copyTo) {
		switch (copyTo->processType)
		{
		case FORLOOP:
			copyTo->text = "For_" + to_string(copyTo->concreteCDFD_id) + "\n( )";
			break;
		case WHILELOOP:
			copyTo->text = "While_" + to_string(copyTo->concreteCDFD_id) + "\n( )";
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

	bool AddDoIfStmt_id(int id) {
		doIfStmt_id = id;
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
		switch (processType)
		{
			case NORMAL:
				break;
			case BRANCH_STANDARD:
			case BRANCH_DEFAULT:
				str += ":" + to_string(connectIfStmt_id) + ": ";
				break;
			case FORLOOP:
			case WHILELOOP:
				str += ":" + to_string(concreteCDFD_id) + ": ";
				break;
			case DATESTORE:
				break;
			default:
				break;
		}
		str += "<" + type + ">";
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
		
		str += " [" + to_string(doIfStmt_id);
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
		case DATESTORE:
			width = myDraw->GetTextWidth(text) + (process_LeftGraph.sizeX + process_RightGraph.sizeX);
			height = dateStore_Name.sizeY;
		default:
			break;
		}
		int inputSize = (int)input.size(), outputSize = (int)output.size();
		int limitSize = 3;
		if (limitSize < inputSize || limitSize < outputSize) {
			int size = (inputSize < outputSize) ? outputSize : inputSize;
			height += myDraw->GetTextHeight() * (size - limitSize);
		}
	}
	
	int GetPosX(vector<int>* maxWidthList) {
		int x = maxWidthList->at(0);
		for (int i = 1; i < pos.x; i++) {
			x += maxWidthList->at(i) + 100;
		}
		x += maxWidthList->at(pos.x) / 2;
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
			if (text == "") {
				break;
			}
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
		case DATESTORE:
			dateStore_Name.DrawExtend(x, y, width, height);
			dateStore_Number.DrawExtend(x - width / 2, y, dateStore_Number.sizeX, height, 6);
			myDraw->Draw_String_Black(x - (width + dateStore_Number.sizeX) / 2, y, to_string(id));
			myDraw->Draw_String_Black(x, y, text);
		default:
			break;
		}
		/*ノードの大きさを表示*/
		//myDraw->Draw_String_White(x + width / 2, y - height / 2, to_string(width) + "×" + to_string(height), 7);

		return true;
	}

	/**変数の矢印を描画
	*connectNodeIDリストinput変数を見比べ
	*
	*/
	bool DrawArrow(vector<int>* maxWidthList, int cursor_x, int cursor_y, MyDrawString* myDraw, vector<Node> nodes) {
		int inputSize = (int)input.size();
		
		int inoutPos = 10;
		int connectSize = (int)connectNodeID.size();
		for (int inIndex = 0; inIndex < connectSize; inIndex++) {
			Node* outputNode = &nodes[connectNodeID[inIndex]];
			vector<string>* connectOutput = &outputNode->output;
			int size = (int)connectOutput->size();
			int output_x = 0, output_y = 0;

			if (outputNode->processType == FORLOOP || outputNode->processType == WHILELOOP) {
				for (int outIndex = 0; outIndex < size; outIndex++) {
					if (connectOutput->at(outIndex) == input[inIndex]) {
						int dy = (outputNode->height / 2) / (int)(size + 1);
						output_x = outputNode->GetPosX(maxWidthList) + outputNode->width / 2 - cursor_x;
						output_y = outputNode->GetPosY() + dy * (outIndex + 1) - cursor_y;
						myDraw->Draw_String_White(output_x + inoutPos, output_y - inoutPos, connectOutput->at(outIndex), 4);
						break;
					}
				}
			}
			else {
				for (int outIndex = 0; outIndex < size; outIndex++) {
					if (connectOutput->at(outIndex) == input[inIndex]) {
						int dy = outputNode->height / (int)(size + 1);
						output_x = outputNode->GetPosX(maxWidthList) + outputNode->width / 2 - cursor_x;
						output_y = outputNode->GetPosY() - outputNode->height / 2 + dy * (outIndex + 1) - cursor_y;
						myDraw->Draw_String_White(output_x + inoutPos, output_y - inoutPos, connectOutput->at(outIndex), 4);
						break;
					}
				}
			}
			int dy = height / (int)(inputSize + 1);
			int input_x = GetPosX(maxWidthList) - width / 2  - cursor_x;
			int input_y = GetPosY() - height / 2 + dy * (inIndex + 1) - cursor_y;
			Arrow::Draw(output_x, output_y, input_x, input_y);
			myDraw->Draw_String(input_x - inoutPos, input_y - inoutPos, input[inIndex],GetColor(255,255,0), 6);
		}
		if (doIfStmt_id > 0) {
			Node* ifStmtNode = &nodes[doIfStmt_id];
			int output_x = ifStmtNode->GetPosX(maxWidthList) + ifStmtNode->width / 2 - cursor_x;
			int output_y = ifStmtNode->GetPosY() - cursor_y;

			int input_x = GetPosX(maxWidthList) - width / 2 - cursor_x;
			int input_y = GetPosY() - cursor_y;

			Arrow::Draw(output_x, output_y, input_x, input_y);
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

	/*自身のinput変数が、自分以前のどのプロセスで更新されているか*/
	bool SetPosX(vector<Node> *nodes) {
		bool check = false;
		int connectID = id;
		switch (processType)
		{
		case NORMAL:
		case FORLOOP:
		case WHILELOOP:
		case BRANCH_STANDARD:
		case BRANCH_DEFAULT:
			/*通常nodeの場合*/
			if (doIfStmt_id > 0) {
				pos.x = nodes->at(doIfStmt_id).pos.x + 1;

				connectID = doIfStmt_id;
				if (nodes->at(connectID).connectIfStmt_id > 0) {
					connectID = nodes->at(connectID).connectIfStmt_id;
				}
			}
			else if (connectIfStmt_id > 0) {
				connectID = connectIfStmt_id;
			}
			for (int inIndex = 0; inIndex < (int)input.size(); inIndex++) {
				bool skip = true;
				for (int nodeID = connectID - 1; nodeID >= 0 && skip; nodeID--) {
					int size = (int)nodes->at(nodeID).output.size();
					for (int outIndex = 0; outIndex < size && skip; outIndex++) {
						if (nodes->at(nodeID).output[outIndex] == input[inIndex]) {
							if (pos.x < nodes->at(nodeID).pos.x + 1) {
								pos.x = nodes->at(nodeID).pos.x + 1;
								check = true;
							}
							connectNodeID.push_back(nodeID);
							skip = false;
						}
					}
				}
				/*
				if (skip) {
					connectNodeID.push_back(0);
				}
				*/
			}
			//return check;
			if (connectIfStmt_id > 0) {
				if (nodes->at(connectIfStmt_id).pos.x < pos.x) {
					nodes->at(connectIfStmt_id).pos.x = pos.x;
					Recursion_connectIfStmt_id(nodes, connectIfStmt_id);
				}
				else {
					pos.x = nodes->at(connectIfStmt_id).pos.x;
				}
			}
			return true;
		case DATESTORE:
			pos.x = id + 1;
			/*
			for (int nodeID = id - 1; nodeID >= 0; nodeID--) {
				if (nodes[nodeID].processType == DATESTORE) {
					pos.x += 1;
				}
			}
			*/
			return true;
		default:
			return false;
		}
	}

	void Recursion_connectIfStmt_id(vector<Node> *nodes, int checkID) {
		for (int nodeID = checkID + 1; nodeID < id; nodeID++) {
			if (nodes->at(nodeID).doIfStmt_id > 0) {
				nodes->at(nodeID).pos.x = nodes->at(nodes->at(nodeID).doIfStmt_id).pos.x + 1;
				Recursion_connectIfStmt_id(nodes, nodeID);
			}
		}
		return;
	}
	/*自分以前かつ同じ横軸のプロセスに対し、最大の縦軸+1を自身の位置にする*/
	void SetPosY(vector<Node> nodes) {
		if (processType == DATESTORE) {
			pos.y = 1;
			return;
		}
		if (connectIfStmt_id > 0) {
			pos.y = nodes[connectIfStmt_id].pos.y + 1;
			return;
		}
		for (int nodeID = id - 1; nodeID >= 0; nodeID--) {
			if (pos.x == nodes[nodeID].pos.x) {
				pos.y = nodes[nodeID].pos.y + 1;
				break;
			}
		}
		if (pos.y == 1 && nodes[0].processType == DATESTORE) {
			pos.y = nodes[0].pos.y + 1;
		}
	}
	/*横幅の中で最大の値を返す*/
	int GetWidth(vector<int>* maxWidthList) {
		for (int i = (int)maxWidthList->size() - 1; i < pos.x; i++) {
			maxWidthList->push_back(0);
		}
		if (maxWidthList->at(pos.x) < width) {
			return width;
		}
		return maxWidthList->at(pos.x);
	}
};
