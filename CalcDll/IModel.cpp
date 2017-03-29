#include "stdafx.h"
#include "IModel.h"
#include "TrajectoryCalculator.h"

#include <mutex>
#include <queue>
#include <thread>

IModel::~IModel() {}

class SharedModel : public IModel {
public:
	void enqueue_input(const InputData& data) override {
		std::unique_lock<std::mutex> lock(input_mutex);
		input_data.push(data);
	}

	bool input_empty() const override {
		//single consumer - only one thread removes items from the queue: no lock
		return input_data.empty();
	}

	void clone_flight_data_to(FlightData& o_flight_data) const override {
		std::unique_lock<std::mutex> lock(flight_mutex);
		o_flight_data = curr_flight_data;
	}

	InputData get_input_data_item() override {
		InputData data;
		{
			std::unique_lock<std::mutex> lock(input_mutex);
			data = input_data.front();
			input_data.pop();
		}

		return data;
	}

	void set_flight_data(const FlightData& data) override {
		std::unique_lock<std::mutex> lock(flight_mutex);
		curr_flight_data = data;
	}

	void run_trajectory_calculator(int points_number, callback i_callback_func /*=callback()*/) override {
		std::thread(&TrajectoryCalculator::run, TrajectoryCalculator(points_number
            , *this), i_callback_func).detach();
	}

protected:
	mutable std::mutex input_mutex;
	std::queue<InputData> input_data;

	mutable std::mutex flight_mutex;
	FlightData curr_flight_data;
};


#if defined(_MSC_VER)
#define CALCDLL_API __declspec(dllexport)
#else
#define CALCDLL_API __attribute__ ((visibility ("default")))
#endif

extern "C" CALCDLL_API IModel& flight_model_instance() {
	static SharedModel model;
	return model;
}