#pragma once

#include <vector>
#include <Windows.h>
#include <IModel.h>

namespace Gdiplus {
	class Point;
	class Graphics;
}


namespace Draw {
	
	void coord_sys(Gdiplus::Graphics& graphics, const Gdiplus::Point& origin, int mx, int my);

	void trajectory(Gdiplus::Graphics& graphics, const std::vector<Tr_PointF>& points
		, float scale, const Gdiplus::Point& origin);

	void axis_gradation(Gdiplus::Graphics& graphics, const Gdiplus::Point& origin
		, float max_data_x, float max_data_y, float scale);
}