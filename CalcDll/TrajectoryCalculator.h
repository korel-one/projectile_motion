#pragma once

#include "BallisticFlight.h"
#include <thread>

class TrajectoryCalculator {
public:

	TrajectoryCalculator(int i_points
        , IModel& i_model)
		: model(i_model), points_number(i_points)
    {}

	void run(IModel::callback callback_func = IModel::callback());

private:
	IModel& model;
	int points_number;

	BallisticFlight flight;
};

void TrajectoryCalculator::run(IModel::callback callback_func /* = IModel::callback()*/) {

	while (true) {

		while ( model.input_empty()) { 
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}

		InputData data = model.get_input_data_item();
		flight.init(0., (float)data.launcher_y
			, data.alpha, data.v0);

		flight.calculate_trajectory(points_number);
		model.set_flight_data(flight.get_data());
		
		printf("altitude = %.3f\n", flight.get_data().max_altitude);
		printf("distance = %.3f\n", flight.get_data().range);

        if(callback_func)
            callback_func();
	}
}