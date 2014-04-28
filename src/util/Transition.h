#ifndef __MEDLEAP_TRANSITION_H__
#define __MEDLEAP_TRANSITION_H__

#include "gl/math/Math.h"
#include <chrono>

/** Tracks a value (progress) that transitions linearly from 0 to 1 or 1 to 0. */
class Transition
{
public:
	enum class State { empty, full, increase, decrease };

	Transition(std::chrono::milliseconds time_to_complete);

	/** Progress percent in [0,1] */
	float progress() const;

	/** State == empty */
	bool empty() const;

	/** State == full */
	bool full() const;

	/** State == increase */
	bool increasing() const;

	/** State == decrease */
	bool decreasing() const;

	/** State == empty || State == full */
	bool idle() const;

	/** Current state */
	State state() const;

	/** Set state */
	void state(State state);

	/** Update transition */
	void update(std::chrono::milliseconds elapsed);

private:
	std::chrono::milliseconds time_to_complete_;
	float progress_;
	State state_;
};

#endif // __MEDLEAP_TRANSITION_H__