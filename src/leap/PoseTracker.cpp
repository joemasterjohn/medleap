#include "PoseTracker.h"

using namespace Leap;

PoseTracker::PoseTracker()
{
	enableAll(false);
}

void PoseTracker::enableAll(bool enabled)
{
	carry_.enabled(enabled);
	fist_.enabled(enabled);
	l_.enabled(enabled);
	palms_face_.enabled(enabled);
	pinch_.enabled(enabled);
	point_.enabled(enabled);
	point2_.enabled(enabled);
	v_.enabled(enabled);
}

void PoseTracker::update(const Frame& frame)
{
	carry_.update(frame);
	fist_.update(frame);
	l_.update(frame);
	palms_face_.update(frame);
	pinch_.update(frame);
	point_.update(frame);
	point2_.update(frame);
	v_.update(frame);
}