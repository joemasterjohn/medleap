#ifndef __medleap_MaskController__
#define __medleap_MaskController__

#include "layers/Controller.h"
#include "leap/GrabTracker.h"
#include "gl/math/Math.h"

class MaskController : public Controller
{
public:
	MaskController();

	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;

private:
	class Tool
	{
	public:
		void apply(gl::Vec3 position);
	};

	class Edit
	{
		Tool tool;
		std::vector<gl::Vec3> points;
		void apply(); // for each point: tool.apply(p)
		void undo();
	};


	// History<Edit> history_;
	// list<Edit> history;

	// swipe left = undo

	GrabTracker tracker_;
	gl::Vec3 cursor_;
	float tool_radius_;
	// erase OR restore (or lower alpha?)
	// tool shape: sphere, flat disk

	// reset all
	// undo

	void scrubVoxels();

	void apply(const Edit& edit);
};

#endif // __medleap_MaskController__
