#include "Pose.h"

using namespace std::chrono;
using namespace Leap;

Pose::Pose() : 
	enabled_(true),
	tracking_(false),
	min_valid_frames_(0),
	valid_frames_(0),
	frames_tracked_(0),
	engage_delay_(0),
	disengage_delay_(0),
	total_elapsed_(0),
    last_update_(std::chrono::high_resolution_clock::now()),
	track_function_(nullptr),
	engage_function_(nullptr),
	disengage_function_(nullptr)
{
}

Pose::~Pose()
{
}

void Pose::update(const Frame& frame)
{
	if (!enabled_) {
		return;
	}

	auto time = std::chrono::high_resolution_clock::now();
	milliseconds elapsed = duration_cast<milliseconds>(time - last_update_);
	last_update_ = time;

	if (tracking_) {
		if (total_elapsed_ < disengage_delay_) {
			total_elapsed_ += elapsed;
		}
		
		if (total_elapsed_ >= disengage_delay_ && shouldDisengage(frame)) {
			tracking_ = false;
			total_elapsed_ = milliseconds(0);
			disengage(frame);
			if (disengage_function_) {
				disengage_function_(frame);
			}
		}
	} else {
		if (total_elapsed_ < engage_delay_) {
			total_elapsed_ += elapsed;
		}

		if (total_elapsed_ >= engage_delay_) {
			if (shouldEngage(frame)) {
				if (++valid_frames_ >= min_valid_frames_) {
					valid_frames_ = 0;
					tracking_ = true;
					total_elapsed_ = milliseconds(0);
					frames_tracked_ = 0;
					engage(frame);
					if (engage_function_) {
						engage_function_(frame);
					}
				}
			} else {
				valid_frames_ = 0;
			}
		}
	}

	if (tracking_) {
		if (frames_tracked_ < std::numeric_limits<uint64_t>::max()) {
			frames_tracked_++;
		}

		track(frame);

		if (track_function_) {
			track_function_(frame);
		}
	}
}

void Pose::tracking(bool tracking)
{
	tracking_ = tracking;
	total_elapsed_ = milliseconds(0);
	valid_frames_ = 0;
}

void Pose::enabled(bool enabled)
{
	enabled_ = enabled;
	if (!enabled_) {
		tracking(false);
	}
}

void Pose::engage(const Frame& frame)
{
}

void Pose::disengage(const Frame& frame)
{
}

void Pose::track(const Frame& frame)
{
}