#include "stdafx.h"
#include "Timer.h"

Timer::Timer() : m_bIsActive(false)
{
}

/// <summary>
/// Start the timer from 0
/// </summary>
void Timer::Start()
{
	//Get the start time and set active
	m_startTime = std::chrono::steady_clock::now();
	m_bIsActive = true;
}


/// <summary>
/// Stops the timer, time is not reset until start is called
/// </summary>
void Timer::Stop()
{
	//Record end time and set inactive
	m_endTime = std::chrono::steady_clock::now();
	m_bIsActive = false;
}

/// <summary>
/// Gets if the timer is active
/// </summary>
/// <returns>If this timer is active</returns>
bool Timer::IsActive() const
{
	return m_bIsActive;
}

/// <summary>
/// Gets the milliseconds since the start of the timer,
/// if the timer has been stopped then it is the start - end time
/// otherwise it is the start time - now
/// </summary>
double Timer::GetElapsedMS() const
{
	std::chrono::time_point<std::chrono::steady_clock> endTime;

	//If the timer is running then use now as the end time, otherwise
	//use the end time when we stopped the timer
	if(m_bIsActive)
	{
		endTime = std::chrono::steady_clock::now();
	}else
	{
		endTime = m_endTime;
	}

	//Return the time difference
	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count();
}

/// <summary>
/// Gets the time elapsed in seconds
/// </summary>
/// <returns></returns>
double Timer::GetElapsedS() const
{
	//Get the milliseconds difference and just divide it by 1000 for the number
	//of milliseconds in a second
	return (GetElapsedMS() / 1000.f);
}
