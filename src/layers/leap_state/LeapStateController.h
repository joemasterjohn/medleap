#ifndef __medleap_LeapStateController__
#define __medleap_LeapStateController__

#include "layers/Controller.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include <vector>
#include <set>

class LeapStateController : public Controller
{
public:
	enum class State
	{
		h1f1_point,
		h1f2_point,
		h1f2_trigger,
		h1f5_spread,
		h2f1_point,
		none
	};

	LeapStateController();
	void draw() override;
	void availableStates(std::set<State> states);
	void availableState(State state, bool available);
	void activeState(State state);

private:
	struct StateView
	{
		std::string label_;
		gl::Texture texture_;
		bool available_;
	};
	
	std::vector<StateView> state_views_;
	StateView* active_state_;
	gl::Buffer icon_vbo_;
	gl::Program icon_prog_;
};

#endif // __medleap_LeapStateController__
