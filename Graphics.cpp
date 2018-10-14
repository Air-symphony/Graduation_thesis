#pragma once
#include "DxLib.h"
#include <string>

/*
画像表示関係
int graphID;
int sizeX, sizeY;
*/
class Graph {
private:
	int graphID = -1;
public:
	int sizeX = 0, sizeY = 0;
	Graph() {
		graphID = NULL;
	}
	/*
	Graph(LoadGraph("\\00.png"))
	*/
	Graph(int _graphID) {
		SetGraph(_graphID);
	}
	/*
	setGraph(LoadGraph("\\00.png"))
	*/
	void SetGraph(int graph) {
		graphID = graph;
		GetGraphSize(graphID, &sizeX, &sizeY);
	}
	/*
	width, height = 画像を表示させたい座標
	number = 特定の位置を中心として表示(テンキー)
	DrawGraph()
	*/
	void Draw(int x, int y, int number = 5) {
		if (number == 1)
			DrawGraph(x, y, graphID, TRUE);
		else if (number == 2)
			DrawGraph(x - sizeX / 2, y, graphID, TRUE);
		else if (number == 3)
			DrawGraph(x - sizeX, y, graphID, TRUE);
		else if (number == 4)
			DrawGraph(x, y - sizeY / 2, graphID, TRUE);
		else if (number == 5)
			DrawGraph(x - sizeX / 2, y - sizeY / 2, graphID, TRUE);
		else if (number == 6)
			DrawGraph(x - sizeX, y - sizeY / 2, graphID, TRUE);
		else if (number == 7)
			DrawGraph(x, y - sizeY, graphID, TRUE);
		else if (number == 8)
			DrawGraph(x - sizeX / 2, y - sizeY, graphID, TRUE);
		else if (number == 9)
			DrawGraph(x - sizeX, y - sizeY, graphID, TRUE);
	}
	/*
	width, height = 画像を表示させたい座標
	sizeX, sizeY = 画像の大きさ
	number = 特定の位置を中心として表示(テンキー)
	DrawExtendGraph();
	*/
	void DrawExtend(int x, int y, int width, int height, int number = 5) {
		if (number == 1)
			DrawExtendGraph(x, y, x + width, y + height, graphID, TRUE);
		else if (number == 2)
			DrawExtendGraph(x - width / 2, y, x + width / 2, y + height, graphID, TRUE);
		else if (number == 3)
			DrawExtendGraph(x - width, y, x, y + height, graphID, TRUE);
		else if (number == 4)
			DrawExtendGraph(x, y - height / 2, x + width, y + height / 2, graphID, TRUE);
		else if (number == 5)
			DrawExtendGraph(x - width / 2, y - height / 2, x + width / 2, y + height / 2, graphID, TRUE);
		else if (number == 6)
			DrawExtendGraph(x - width, y - height / 2, x, y + height / 2, graphID, TRUE);
		else if (number == 7)
			DrawExtendGraph(x, y - height, x + width, y, graphID, TRUE);
		else if (number == 8)
			DrawExtendGraph(x - width / 2, y - height, x + width / 2, y, graphID, TRUE);
		else if (number == 9)
			DrawExtendGraph(x - width, y - height, x, y, graphID, TRUE);
	}
};

/*テキスト表示関係*/
class MyDrawString {
private:
	int defaultSize = GetFontSize();
	int mainFontSize;

	void Draw_String(int x, int y, int _color, std::string text, int number = 5) {
		int StrWidth = GetTextWidth(text);

		if (number == 1)
			DrawString(x, y, text.c_str(), _color);
		else if (number == 2)
			DrawString(x - StrWidth / 2, y, text.c_str(), _color);
		else if (number == 3)
			DrawString(x - StrWidth, y, text.c_str(), _color);
		else if (number == 4)
			DrawString(x, y - mainFontSize / 2, text.c_str(), _color);
		else if (number == 5)
			DrawString(x - StrWidth / 2, y - mainFontSize / 2, text.c_str(), _color);
		else if (number == 6)
			DrawString(x - StrWidth, y - mainFontSize / 2, text.c_str(), _color);
		else if (number == 7)
			DrawString(x, y - mainFontSize, text.c_str(), _color);
		else if (number == 8)
			DrawString(x - StrWidth / 2, y - mainFontSize, text.c_str(), _color);
		else if (number == 9)
			DrawString(x - StrWidth, y - mainFontSize, text.c_str(), _color);
	}
public:
	MyDrawString() {
		mainFontSize = defaultSize;
		SetFontSize(mainFontSize);
	}
	MyDrawString(int size) {
		mainFontSize = size;
		SetFontSize(mainFontSize);
	}
	~MyDrawString() {
		SetDefaultMainFontSize();
	}
	/*文字の横幅の取得*/
	int GetTextWidth(std::string text) {
		int len = (int)strlen(text.c_str());
		return GetDrawStringWidth(text.c_str(), (int)strlen(text.c_str()));
	}
	/*文字の縦幅の取得*/
	int GetTextHeight() {
		return mainFontSize;
	}
	/*mainFontSize = size;*/
	void SetMainFontSize(int size) {
		mainFontSize = size;
		SetFontSize(mainFontSize);
	}
	/*mainFontSize = defaultSize;*/
	void SetDefaultMainFontSize() {
		mainFontSize = defaultSize;
		SetFontSize(mainFontSize);
	}
	/*GetColor(255,255,255)*/
	void Draw_String_White(int x, int y, std::string str, int number = 5) {
		Draw_String(x, y, GetColor(255, 255, 255), str, number);
	}
	/*GetColor(0, 0, 0)*/
	void Draw_String_Black(int x, int y, std::string str, int number = 5) {
		Draw_String(x, y, GetColor(0, 0, 0), str, number);
	}
	/*color = GetColor(0, 0, 0)*/
	void Draw_String(int x, int y, std::string str, int color, int number = 5) {
		Draw_String(x, y, color, str, number);
	}
};