#include "stdafx.h"
#include "BallisticFlight.h"

#define _USE_MATH_DEFINES
#include <math.h>

const float g = 9.8f;

void BallisticFlight::init(float i_launcher_x, float i_launcher_y, int i_alpha, int i_v0)
{
	launch_x = i_launcher_x;
	launch_y = i_launcher_y;

	data.vx = i_v0*cos(i_alpha / 180.0f*float(M_PI));
	data.vy = i_v0*sin(i_alpha / 180.0f*float(M_PI));

	if (data.vy > 0) {
		data.max_altitude = launch_y + data.vy*data.vy / (2 * g);
	}
	else {
		data.max_altitude = launch_y;
	}

	data.total_flight_time = (data.vy + sqrt(data.vy*data.vy + 2*launch_y*g)) / g;
	data.range = data.vx*data.total_flight_time;
}

void BallisticFlight::calculate_trajectory(int points_number) {
	data.trajectory.clear();
	data.trajectory.reserve(points_number+1);
	
	float time = 0.f;
	for (int i = 0; i < points_number+1; ++i) {
		time = data.total_flight_time * (i*1.f / points_number);

		auto x = get_x(time);
		auto y = get_y(time);
		data.trajectory.push_back(Tr_PointF{ x, y });
	}
}