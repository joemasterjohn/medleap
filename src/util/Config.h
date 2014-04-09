#ifndef __medleap_Config__
#define __medleap_Config__

#include <string>
#include <unordered_map>
#include <vector>
#include "math/Vector3.h"

/** Stores/loads configuration values in a file. Doubles, Itnegers, Booleans, Strings */
class Config
{
public:
	enum Event
	{
		BG_COLOR,
		DCM_DIRECTORY
	};

	class Listener
	{
	public:
		virtual ~Listener() {}
		virtual void configChanged(Event evt, const Config& config) = 0;
	};

	void addListener(Event evt, Listener& listener);

	const std::string& getDicomDir();
	const Vec3& getBackgroundColor();

	void setDicomDir(const std::string& dir);
	void setBackgroundColor(float r, float g, float b);

	// config.getValue<float>(

	template <typename T> T getValue<T>(const std::string& paramName);

	std::string getValue(const std::string& paramName);

	void getValue(const std::string& paramName);




private:
	std::unordered_map<Event, std::vector<Listener*>> listeners;
	std::string dcmDirectory;
	Vec3 bgColor;

	// multisampling, numSamples, sRGB, 

	void fireEvent(Event evt);
};

#endif // __medleap_Config__