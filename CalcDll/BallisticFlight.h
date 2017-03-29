#pragma once

#include "IModel.h"

extern const float g;// gravitational acceleration

class BallisticFlight  {
public:
	void init(float i_launcher_x, float i_launcher_y
		, int i_alpha // degrees
		, int i_v0); // [m/s]

	void calculate_trajectory(int points_number);

	FlightData get_data() const {
		return data;
	}
	
private:
	float get_x(float i_t) const {
		return launch_x + data.vx*i_t;
	}

	float get_y(float i_t) const {
		return launch_y + (data.vy*i_t - 0.5f * g * i_t *i_t);
	}

private:
	float launch_x, launch_y;
	FlightData data;
};