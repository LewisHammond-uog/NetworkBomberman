#ifndef __TRANSFORM_HISTORY_H__
#define __TRANSFORM_HISTORY_H__

//GLM Incldues
#include <glm/mat4x4.hpp>

//RakNet Includes
#include <DS_Queue.h>
#include <RakNetTime.h>

/// <summary>
/// An Item in the transform history.
/// Stores the transform and the time
/// that we recevied it
/// </summary>
struct TransformHistoryItem
{
	TransformHistoryItem() = default;
	TransformHistoryItem(const RakNet::TimeMS a_fTime, const glm::mat4 a_m4Transform) : m_timeReceived(a_fTime), m_m4Transform(a_m4Transform) {};
	
	RakNet::TimeMS m_timeReceived;
	glm::mat4 m_m4Transform;
};

/// <summary>
/// A class to store the history of the positions of an object
/// Initalise the number of data points that we store using Init()
/// Data Read by using Read(), Read() will interpolate between two known points given a time between those points, or between the last known and current point.
/// Data Written by using Write()
/// </summary>
class TransformHistory
{
public:
	//Enum for which method we used for read interpolation
	enum class ReadResult
	{
		READ_OLDEST, //No data yet to interplate so chose the 1st queue item
		VALUES_UNCHANGED, //Values were not changed
		INTERPOLATED //We used values in the past and interpolated
	};
	
	TransformHistory();
	~TransformHistory() = default;
	
	//Initalisation
	void Init(RakNet::TimeMS a_fMaxWriteInterval, RakNet::TimeMS a_fMaxHistoryTime);

	//Read/Write Functions
	void Write(glm::mat4 a_m4TransformMatrix, RakNet::TimeMS a_fCurTimeMs);
	ReadResult Read(glm::mat4& a_pm4TransformMatrix, RakNet::TimeMS a_when, RakNet::TimeMS a_curTime) const;

	void Clear();
private:
	DataStructures::Queue<TransformHistoryItem> m_qTransformHistory;
	unsigned int m_iMaxHistoryLength; //Maximum number of allowed histroy items
	RakNet::TimeMS m_fWriteInterval; //Time between position writes
};


#endif //!__TRANSFORM_HISTORY_H__