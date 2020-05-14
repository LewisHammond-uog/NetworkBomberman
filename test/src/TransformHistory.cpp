#include "stdafx.h"
#include "TransformHistory.h"

TransformHistory::TransformHistory() :
	m_iMaxHistoryLength(0),
	m_fWriteInterval(0.f)
{
}

/// <summary>
/// Initalise the Transform History
/// </summary>
/// <param name="a_fMaxWriteInterval">Min time between new data points being added</param>
/// <param name="a_fMaxHistoryTime">How long to store data points for</param>
void TransformHistory::Init(RakNet::TimeMS a_fMaxWriteInterval, RakNet::TimeMS a_fMaxHistoryTime)
{
	//Set variables
	m_fWriteInterval = a_fMaxWriteInterval;
	m_iMaxHistoryLength = a_fMaxHistoryTime / a_fMaxWriteInterval + 1;

	//Allocate Queue Memory
	m_qTransformHistory.ClearAndForceAllocation(m_iMaxHistoryLength + 1, _FILE_AND_LINE_);
}

/// <summary>
/// Write data to the transform history.
/// If less than the max interval has elapsed then data will not be written
/// </summary>
/// <param name="a_m4TransformMatrix"></param>
/// <param name="a_fCurTimeMs"></param>
void TransformHistory::Write(glm::mat4 a_m4TransformMatrix, RakNet::TimeMS a_fCurTimeMs)
{
	//If this is our first data just push it ignoring the inverval
	if(m_qTransformHistory.Size() == 0)
	{
		m_qTransformHistory.Push(TransformHistoryItem(a_fCurTimeMs, a_m4TransformMatrix), _FILE_AND_LINE_);
	}else
	{
		//Check that we have had more than the minimum update time since our last update,
		//if so write to the queue
		const TransformHistoryItem& lastUpdate = m_qTransformHistory.PeekTail();
		if((a_fCurTimeMs - lastUpdate.m_timeReceived) >= m_fWriteInterval)
		{
			m_qTransformHistory.Push(TransformHistoryItem(a_fCurTimeMs, a_m4TransformMatrix), _FILE_AND_LINE_);

			//If we are over the max queue size then remove the last one in the list
			if(m_qTransformHistory.Size() > m_iMaxHistoryLength)
			{
				m_qTransformHistory.Pop();
			}
			
		}
	}
	
}

/// <summary>
/// Read the interpolated transform
/// </summary>
/// <param name="a_pm4TransformMatrix">[INPUT] the current position of the object [OUTPUT] the position of the out point at *WHEN*</param>
/// <param name="a_when">Time we want the transform for</param>
/// <param name="a_curTime">Current Network Time</param>
TransformHistory::ReadResult TransformHistory::Read(glm::mat4& a_pm4TransformMatrix, RakNet::TimeMS a_when, RakNet::TimeMS a_curTime)
{
	const int historySize = m_qTransformHistory.Size();

	//Check that we have a history
	if(historySize == 0)
	{
		return ReadResult::VALUES_UNCHANGED;
	}

	//Loop until we find a history item that is after the time we
	//are looking for
	for(int i = historySize - 1; i >= 0; --i)
	{
		//bug - potentially should seperate rotation and position for lerp
		
		const TransformHistoryItem& currentItem = m_qTransformHistory[i];

		if (a_when >= currentItem.m_timeReceived) {
			//If our item is the 1st item in the queue
			if (i == (historySize - 1))
			{
				//If our current time is less than the cell time, we don't
				//need to change the values
				if (a_curTime < currentItem.m_timeReceived)
				{
					return ReadResult::VALUES_UNCHANGED;
				}

				//Otherwise calculate the lerped position based our the difference between the
				//current history item that we found and our current positon
				const float fDivisor = (float)(a_curTime - currentItem.m_timeReceived);
				const float fLerpVal = (float)(a_when - currentItem.m_timeReceived) / fDivisor;
				a_pm4TransformMatrix = currentItem.m_m4Transform + (a_pm4TransformMatrix - currentItem.m_m4Transform) * fLerpVal;
			}else
			{
				//Our item is not the first in the queue so we need to next the value after
				//this in the queue and interpolate between them
				const TransformHistoryItem& nextItem = m_qTransformHistory[i + 1];
				const float fDivisor = (float)(nextItem.m_timeReceived - currentItem.m_timeReceived);
				const float fLerpVal = (float)(a_when - currentItem.m_timeReceived) / fDivisor;
				a_pm4TransformMatrix = currentItem.m_m4Transform + (nextItem.m_m4Transform - currentItem.m_m4Transform) * fLerpVal;
			}
			return ReadResult::INTERPOLATED;
		}
	}

	//No value found just return the oldest item in the queue
	const TransformHistoryItem& oldestItem = m_qTransformHistory.Peek();
	a_pm4TransformMatrix = oldestItem.m_m4Transform;
	return READ_OLDEST;
}

/// <summary>
/// Clear the transform history
/// </summary>
void TransformHistory::Clear()
{
	m_qTransformHistory.Clear(_FILE_AND_LINE_);
}
