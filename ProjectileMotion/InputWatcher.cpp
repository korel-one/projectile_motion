#include "InputWatcher.h"

#include <iostream>
#include <string>

InputWatcher::InputWatcher(IModel* p_flight_model, IModel::callback i_callback_funk /*= IModel::callback()*/)
: flight_model(p_flight_model), callback_funk(i_callback_funk) {

	std::cout << "Enter the projectile motion data (three integers separated by whitespace):\n" \
		"\t1) angle in degrees - [-89 .. 89]\n" \
		"\t2) initial velocity m/s - [10 .. 100]\n" \
		"\t3) launch height m - [0 .. 100].\n" \
		"Enter -100 to exit.\n" << std::endl;
}

void InputWatcher::run() {
	while (true) {
		std::cin >> alpha;
		if (alpha == -100) break;
		std::cin >> v0 >> launcher_y;

		std::string error_msg;
		if (alpha < -89 || alpha > 89) {
			error_msg = "error: angle(input param 1) should be in range [-89 .. 89]\n";
		}
		if (v0 < 10 || v0 > 100) {
			error_msg += "error: velocity(input param 2) should be in range [10 .. 100]\n";
		}
		if (launcher_y < 0 || launcher_y > 100) {
			error_msg += "error: height(input param 3) should be in range [0 .. 100]\n";
		}
		if (!error_msg.empty()){
			std::cout << error_msg << std::endl;
			continue;
		}

		flight_model->enqueue_input({ alpha, v0, launcher_y });
	}

	//finalize after exit
	if (callback_funk)
		callback_funk();
}
