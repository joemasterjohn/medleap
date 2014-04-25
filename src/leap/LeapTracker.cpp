#include "LeapTracker.h"

using namespace std::chrono;

LeapTracker::LeapTracker() : 
		tracking_(false),
		engage_delay_(0),
		disengage_delay_(0),
		total_elapsed_(0),
        last_update_(std::chrono::high_resolution_clock::now()),
		track_function_(nullptr),
		engage_function_(nullptr),
		disengage_function_(nullptr)
{
}

void LeapTracker::update(const Leap::Controller& controller)
{
	auto time = std::chrono::high_resolution_clock::now();
	milliseconds elapsed = duration_cast<milliseconds>(time - last_update_);
	last_update_ = time;

	if (tracking_) {
		if (total_elapsed_ < disengage_delay_) {
			total_elapsed_ += elapsed;
		}
		
		if (total_elapsed_ >= disengage_delay_ && shouldDisengage(controller)) {
			tracking_ = false;
			total_elapsed_ = milliseconds(0);
			disengage(controller);
		}
	}
	else {
		if (total_elapsed_ < engage_delay_) {
			total_elapsed_ += elapsed;
		}

		if (total_elapsed_ >= engage_delay_ && shouldEngage(controller)) {
			tracking_ = true;
			total_elapsed_ = milliseconds(0);
			engage(controller);
		}
	}

	if (tracking_) {
		track(controller);
	}
}

void LeapTracker::engage(const Leap::Controller& controller)
{
	std::cout << "EG" << std::endl;

	if (engage_function_)
		engage_function_(controller);
}

void LeapTracker::disengage(const Leap::Controller& controller)
{
	std::cout << "DE" << std::endl;
	if (disengage_function_)
		disengage_function_(controller);
}

void LeapTracker::track(const Leap::Controller& controller)
{
	if (track_function_)
		track_function_(controller);
}