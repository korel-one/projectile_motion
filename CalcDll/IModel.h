#pragma once

#include <vector>
#include <functional>

struct Tr_PointF {
	float X, Y;
};

struct FlightData {
	float vx, vy;

	float total_flight_time;
	float range;
	float max_altitude;

	std::vector<Tr_PointF> trajectory;
};

struct InputData {
	int alpha, v0, launcher_y;
};

class IModel{
public:
    using callback = std::function<void()>;

	virtual void enqueue_input(const InputData& data) = 0;
	virtual bool input_empty() const = 0;

	virtual void clone_flight_data_to(FlightData& o_flight_data) const = 0;
    virtual void run_trajectory_calculator(int points_number, callback callback_func = callback()) = 0;

	virtual InputData get_input_data_item() = 0;
	virtual void set_flight_data(const FlightData& data) = 0;

protected:
	virtual ~IModel() = 0;
};