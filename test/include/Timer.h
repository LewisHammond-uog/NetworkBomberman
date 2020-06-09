#ifndef __TIMER_CONTROL_COMPONENT_H__
#define __TIMER_CONTROL_COMPONENT_H__

//C++ Includes
#include <chrono>

/// <summary>
/// Class to represent a timer, uses the steady clock so as not to be influenced
/// by system time changes
/// elapsed
/// </summary>
class Timer
{
public:
	Timer();
	~Timer() = default;

	void Start();
	void Stop();
	bool IsActive() const;
	double GetElapsedMS() const;
	double GetElapsedS() const;
	
private:
	bool m_bIsActive; //If the timer is active
	std::chrono::time_point<std::chrono::steady_clock> m_startTime; //Start Time of the timer
	std::chrono::time_point<std::chrono::steady_clock> m_endTime; //End Time of the timer, if stopped
	
	
};


#endif //!__TIMER_CONTROL_COMPONENT_H__