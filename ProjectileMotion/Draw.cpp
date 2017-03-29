#include "Draw.h"

#include <gdiplus.h>
#include <sstream>
#include <ios>
#include <iomanip>
#include <tchar.h>

namespace Draw {

	using namespace Gdiplus;

	void coord_sys(Graphics& graphics, const Point& origin, int mx, int my) {

		Pen pen(Gdiplus::Color::Black, 1);
		Font font(&FontFamily(L"Arial"), 12);
		SolidBrush brush(Color::Red);

		//axis x
		graphics.DrawLine(&pen, origin, Point(origin.X + mx + 20, origin.Y));

		//arrow
		graphics.DrawLine(&pen, Point(origin.X + mx + 20, origin.Y), Point(origin.X + mx + 5, origin.Y - 7));
		graphics.DrawLine(&pen, Point(origin.X + mx + 20, origin.Y), Point(origin.X + mx + 5, origin.Y + 7));
		graphics.DrawString(_T("X, m."), 5, &font, PointF(origin.X + 1.f*mx, origin.Y+30.f), &brush);


		//axis y
		graphics.DrawLine(&pen, origin, Point(origin.X, origin.Y - my - 20));

		//arrow
		graphics.DrawLine(&pen,Point(origin.X, origin.Y - my - 20), Point(origin.X - 7, origin.Y - my - 5));
		graphics.DrawLine(&pen,Point(origin.X, origin.Y - my - 20), Point(origin.X + 7, origin.Y - my - 5));
		graphics.DrawString(_T("Y, m."), 5, &font, PointF(origin.X - 60.f, origin.Y - my - 20.f), &brush);
	}

	void trajectory(Graphics& graphics, const std::vector<Tr_PointF>& points
		, float scale, const Point& origin) {

		Pen blue_pen(Color::Blue, 1);

		std::vector<PointF> gdi_trajectory;
		gdi_trajectory.reserve(points.size());

		for (auto& point : points) {

			gdi_trajectory.emplace_back(origin.X + scale*point.X
				, origin.Y - scale*point.Y);
		}

		graphics.DrawCurve(&blue_pen, gdi_trajectory.data(), gdi_trajectory.size());
	}

	void axis_gradation(Graphics& graphics, const Point& origin
		, float max_data_x, float max_data_y, float scale) {

		Font font(&FontFamily(L"Arial"), 12);
		SolidBrush brush(Color::Red);
		Pen blue_pen(Color::Red, 1);

		//x axis's number of divisions 
		int num_points_x = 10;
		if (scale*max_data_x < 750) num_points_x = 5;
		if (scale*max_data_x < 300) num_points_x = 2;
		if (scale*max_data_x < 150) num_points_x = 1;

		//y axis's number of divisions 
		int num_points_y = 10;
		if (scale*max_data_y < 250) num_points_y = 5;
		if (scale*max_data_y < 100) num_points_y = 2;
		if (scale*max_data_y < 50) num_points_y = 1;

		float x_axis_step = scale*max_data_x / num_points_x;
		float y_axis_step = scale*max_data_y / num_points_y;

		printf("scale: 1 meter = %.2lf pixels\n", scale);

		std::wstringstream ss;
		for (int i = 1; i <= num_points_x; ++i) {
			graphics.DrawLine(&blue_pen, PointF(origin.X + x_axis_step*i, origin.Y - 5.f)
				, PointF(origin.X + x_axis_step*i, origin.Y + 5.f));

			auto mark_val = round((max_data_x / num_points_x) * i * 100) / 100;
			ss.str(std::wstring());
			ss << std::fixed << std::setprecision(2) << mark_val;
			std::wstring mark_str = ss.str();

			graphics.DrawString(mark_str.c_str(), mark_str.size(), &font
				, PointF(origin.X + x_axis_step*i - 10.f, origin.Y + 10.f)
				, &brush);
		}

		for (int i = 1; i <= num_points_y; ++i) {
			graphics.DrawLine(&blue_pen, PointF(origin.X - 5.f, origin.Y - y_axis_step*i)
				, PointF(origin.X + 5.f, origin.Y - y_axis_step*i));

			auto mark_val = ceil((max_data_y / num_points_y) * i * 100) / 100;
			ss.str(std::wstring());
			ss << std::fixed << std::setprecision(2) << mark_val;
			std::wstring mark_str = ss.str();

			graphics.DrawString(mark_str.c_str(), mark_str.size(), &font
				, PointF(origin.X - 60.f, origin.Y - y_axis_step*i - 10)
				, &brush);
		}
	}
}//namespace Draw