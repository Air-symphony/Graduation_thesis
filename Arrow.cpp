#pragma once
#include "Graphics.cpp"
#include <string>

class Arrow {
private:
	static Graph arrowTip;
	static Graph arrowBase;
public:
	Arrow() {
		SetGraph();
	}
	static void SetGraph() {
		arrowBase.SetGraph(LoadGraph("picture\\Arrow_base.png"));
		arrowTip.SetGraph(LoadGraph("picture\\Arrow_Tip.png"));
	}
	/*x1y1èoÇæÇµÅAx2y2ñµêÊ*/
	static void Draw(int input_x, int input_y, int output_x, int output_y) {
		double y = output_y - input_y;
		double x = output_x - input_x;
		double z = sqrt(y * y + x * x); 

		double ratex = z / ((double)arrowBase.sizeX * 2.0f);
		double angle = std::atan2(output_y - input_y, output_x - input_x);

		/*
		DrawString(input_x + (int)(x / 2.0f), input_y + (int)(y / 2.0f) - 20,
			text.c_str(), GetColor(255, 255, 255));
		*/
		
		//arrowBase.DrawArrowBase(input_x, input_y, ratex, angle);
		DrawLine(input_x, input_y, output_x, output_y, GetColor(255,255,255));
		arrowTip.DrawArrowTip(output_x, output_y, ratex, angle);

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
