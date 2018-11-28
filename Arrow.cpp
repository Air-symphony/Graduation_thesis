#pragma once
#include "Graphics.cpp"
#include <string>

class Arrow {
private:
	static Graph arrowTip;
	static Graph arrowBase;
	static Graph arrowBase_dot;
public:
	Arrow() {
		SetGraph();
	}
	static void SetGraph() {
		arrowBase.SetGraph(LoadGraph("picture\\Arrow_base.png"));
		arrowBase_dot.SetGraph(LoadGraph("picture\\Arrow_base_dot.png"));
		arrowTip.SetGraph(LoadGraph("picture\\Arrow_Tip.png"));
	}
	/*x1y1èoÇæÇµÅAx2y2ñµêÊ*/
	static void DrawArrow(int input_x, int input_y, int output_x, int output_y) {
		double y = output_y - input_y;
		double x = output_x - input_x;
		double z = sqrt(y * y + x * x);

		double ratex = z / ((double)arrowBase.sizeX);
		double angle = std::atan2(output_y - input_y, output_x - input_x);

		arrowBase.DrawArrowBase(input_x, input_y, ratex, angle);
		arrowTip.DrawArrowTip(output_x, output_y, ratex, angle);
	}

	static void DrawArrow_dot(int input_x, int input_y, int output_x, int output_y) {
		double y = output_y - input_y;
		double x = output_x - input_x;
		double z = sqrt(y * y + x * x);

		double ratex = z / ((double)arrowBase_dot.sizeX);
		double angle = std::atan2(output_y - input_y, output_x - input_x);

		arrowBase_dot.DrawArrowBase(input_x, input_y, ratex, angle);
		arrowTip.DrawArrowTip(output_x, output_y, ratex, angle);

	}
};
