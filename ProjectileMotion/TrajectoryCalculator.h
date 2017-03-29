#pragma once

#include "SharedModel.h"
#include "BallisticFlight.h"

#include <thread>

class TrajectoryCalculator {
	using callback = BOOL(*)();

public:
	TrajectoryCalculator(unsigned int i_points_number, HWND i_hwnd)
		//TODO: , callback callback_func)
		: points_number(i_points_number), hwnd(i_hwnd) {}

	void run() {
		while (true) {
			
			while (SharedModel::input_data.empty()) {
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}

			SharedModel::InputData data;
			{
				std::unique_lock<std::mutex> lock(SharedModel::input_mutex);

				data = SharedModel::input_data.front();
				SharedModel::input_data.pop();
			}

			flight.init(Gdiplus::PointF(0., (float)data.launcher_y)
				, data.alpha, data.v0);
			flight.calculate_trajectory(points_number);

			{
				std::unique_lock<std::mutex> lock(SharedModel::flight_mutex);
				SharedModel::curr_flight_data = flight.get_data();
			}

			printf("altitude = %.3f\n",flight.get_data().max_altitude);

			::InvalidateRect(hwnd, nullptr, TRUE);
		}
	}

private:
	BallisticFlight flight;
	unsigned int points_number;
	//TODO: callback invalidate_wnd;
	HWND hwnd;
};