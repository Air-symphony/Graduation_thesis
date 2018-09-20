#pragma once
#include "Graphics.cpp"
#include <string>

#define PI    3.1415926535897932384626433832795f

class Arrow {
private:
	static Graph arrowTip;
	static Graph arrowBase;
	int input_x, input_y;
	int output_x, output_y;
	float dir;
	std::string text;
public:
	Arrow() {
		init();
	}
	void init() {
		arrowBase.SetGraph(LoadGraph("picture\\Arrow_base.png"));
		arrowTip.SetGraph(LoadGraph("picture\\Arrow_Tip.png"));
	}
	/*x1y1èoÇæÇµÅAx2y2ñµêÊ*/
	void SetState(int x1, int y1, int x2, int y2, std::string _text) {
		input_x = x1;
		input_y = y1;
		output_x = x2;
		output_y = y2;
		text = _text;
	}
	void Draw() {
		double y = output_y - input_y;
		double x = output_x - input_x;
		double z = sqrt(y * y + x * x); 

		double ratex = z / ((double)arrowBase.sizeX * 2.0f);
		double angle = std::atan2(output_y - input_y, output_x - input_x);

		//printfDx("arrowBase : %p\n", &arrowBase);
		//printfDx("text : %p\n", &text);
		DrawString(input_x + (int)(x / 2.0f), input_y + (int)(y / 2.0f) - 20,
			text.c_str(), GetColor(255, 255, 255));
		/*
		DrawRotaGraph3(input_x, input_y - arrowBase.sizeY,
			0, arrowBase.sizeY, ratex, 1.0f,
			angle, arrowBase.graphID, TRUE);
		
		DrawRotaGraph3(output_x, output_y,
			0, arrowTip.sizeY, 1.0f, 1.0f,
			angle + PI, arrowTip.graphID, TRUE);
			*/
	}
};
