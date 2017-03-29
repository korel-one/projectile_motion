#pragma once

#include <IModel.h>

/*----------------------------------------------------------------------
gets input from the console and puts it into the queue
----------------------------------------------------------------------*/
class InputWatcher {
public:
	InputWatcher(IModel* p_flight_model
		, IModel::callback i_callback_funk = IModel::callback());

	void run();

private:
	IModel* flight_model = nullptr;
	IModel::callback callback_funk;

	int alpha
		, v0
		, launcher_y;
};