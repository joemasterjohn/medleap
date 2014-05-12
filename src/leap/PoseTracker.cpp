#include "PoseTracker.h"

using namespace std::chrono;

PoseTracker::PoseTracker() : 
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

PoseTracker::~PoseTracker()
{
}

void PoseTracker::update(const Leap::Controller& controller)
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
	} else {
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

void PoseTracker::tracking(bool tracking)
{
	tracking_ = tracking;
	total_elapsed_ = milliseconds(0);
}

void PoseTracker::engage(const Leap::Controller& controller)
{
	if (engage_function_)
		engage_function_(controller);
}

void PoseTracker::disengage(const Leap::Controller& controller)
{
	if (disengage_function_)
		disengage_function_(controller);
}

void PoseTracker::track(const Leap::Controller& controller)
{
	if (track_function_)
		track_function_(controller);
}

void PoseTracker::trackFunction(std::function<void(const Leap::Controller&)> f)
{ 
	track_function_ = f; 
}

void PoseTracker::engageFunction(std::function<void(const Leap::Controller&)> f)
{ 
	engage_function_ = f;
}

void PoseTracker::disengageFunction(std::function<void(const Leap::Controller&)> f)
{ 
	disengage_function_ = f; 
}

void PoseTracker::engageDelay(std::chrono::milliseconds delay)
{ 
	engage_delay_ = delay; 
}

void PoseTracker::disengageDelay(std::chrono::milliseconds delay)
{ 
	disengage_delay_ = delay;
}