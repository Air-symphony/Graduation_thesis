#pragma once
#include "Dxlib.h"
/*
画像表示関係
int graphID;
int sizeX, sizeY;
*/
class Graph {
public:
	/*画像ハンドラ*/
	int graphID;
	/*中心からの画像サイズ*/
	int sizeX, sizeY;
	Graph() {
		graphID = NULL;
		sizeX = sizeY = 0;
	}
	/*
	Graph(LoadGraph("\\00.png"))
	*/
	Graph(int _graph) {
		SetGraph(_graph);
	}
	/*
	SetGraph(LoadGraph("\\00.png"))
	*/
	void SetGraph(int graph) {
		graphID = graph;
		GetGraphSize(graphID, &sizeX, &sizeY);
		sizeX = sizeX / 2;
		sizeY = sizeY / 2;
	}
	/*
	if (graph.graphID == -1) 
			return false;
	graphID = graph.graphID;
	sizeX = graph.GetsizeX();
	sizeY = graph.GetsizeY();
	return true;
		*/
	bool SetGraph(Graph graph) {
		if (graph.graphID == -1) 
			return false;

		graphID = graph.graphID;
		sizeX = graph.GetsizeX();
		sizeY = graph.GetsizeY();
		return true;
	}
	/*
	x, y = 画像を表示させたい座標
	number = 特定の位置を中心として表示(テンキー)
	*/
	void Draw_Graph(int x, int y, int number = 5) {
		if (number == 1)
			DrawGraph(x, y, graphID, TRUE);
		else if (number == 2)
			DrawGraph(x - sizeX, y, graphID, TRUE);
		else if (number == 3)
			DrawGraph(x - sizeX * 2, y, graphID, TRUE);
		else if (number == 4)
			DrawGraph(x, y - sizeY, graphID, TRUE);
		else if (number == 5)
			DrawGraph(x - sizeX, y - sizeY, graphID, TRUE);
		else if (number == 6)
			DrawGraph(x - sizeX * 2, y - sizeY, graphID, TRUE);
		else if (number == 7)
			DrawGraph(x, y - sizeY * 2, graphID, TRUE);
		else if (number == 8)
			DrawGraph(x - sizeX, y - sizeY * 2, graphID, TRUE);
		else if (number == 9)
			DrawGraph(x - sizeX * 2, y - sizeY * 2, graphID, TRUE);
	}
	/*
	拡大表示、背景用
	DrawExtendGraph(0, 0, display_X, display_Y, graphID, TRUE);*/
	void Draw_BackGround(int display_X, int display_Y) {
		DrawExtendGraph(0, 0, display_X, display_Y, graphID, TRUE);
	}
	/*中心からの画像サイズ*/
	int GetsizeX() {
		return sizeX;
	}
	/*中心からの画像サイズ*/
	int GetsizeY() {
		return sizeY;
	}
	void Delete() {
		DeleteGraph(graphID);
	}
};