#include "stdafx.h"
#include "TransformComponent.h"

//Raknet Includes
#include "GetTime.h"

//Project Includes
#include "ConsoleLog.h"
#include "ServerClientBase.h"
#include "TestProject.h"
#include "TransformHistory.h"

//Typedefs
typedef Component PARENT;

TransformComponent::TransformComponent(Entity* a_pOwner) : 
	PARENT(a_pOwner),
	m_m4EntityMatrix(glm::mat4(1.0f))
{
	m_eComponentType = COMPONENT_TYPE::TRANSFORM;

	//Intialise our Transform History
	m_pTransformHistory = new TransformHistory();
	m_pTransformHistory->Init(m_fMaxWriteInterval, m_fMaxHistoryTime);
}

TransformComponent::~TransformComponent()
{
	//Destroy our history
	delete m_pTransformHistory;
}

/// <summary>
/// Update the transform
/// </summary>
/// <param name="a_fDeltaTime"></param>
void TransformComponent::Update(float a_fDeltaTime)
{
	//Read data from the transform history to interpolate between this and the 
	if(!TestProject::isServer /*Is Client*/)
	{
		//Calculate our when time to be the current time minus DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES
		//so the we get the position very slightly in the last, without this we wouldn't have a node to interpolate to,
		//and wouldn't know where to go
		//Matrix is passed by reference so it is updated by this function call
		RakNet::TimeMS interpolateTime = RakNet::GetTimeMS() - DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES;
		m_pTransformHistory->Read(m_m4EntityMatrix, interpolateTime, RakNet::GetTimeMS());
	}
}

/// <summary>
/// Sets a row within the transform matrix
/// </summary>
/// <param name="a_eRow">Row to set</param>
/// <param name="a_v3Vec">Vector 3 to fill row with</param>
void TransformComponent::SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec)
{
	float finalVec4Value = a_eRow == POSTION_VECTOR ? 1.f : 0.0f;
	m_m4EntityMatrix[a_eRow] = glm::vec4(a_v3Vec, finalVec4Value);
}

/// <summary>
/// Gets the row of the matrix (i.e FORWARD, RIGHT, UP, POSITION)
/// </summary>
/// <param name="a_eRow">Row of the matrix to get</param>
/// <returns>Matrix Row as Vector 3</returns>
glm::vec3 TransformComponent::GetEntityMatrixRow(const MATRIX_ROW a_eRow)
{
	return m_m4EntityMatrix[a_eRow];
}

///<summary>
///Gets the current position of the transform
///</summary>
///<returns>Current Transform Position</returns>
glm::vec3 TransformComponent::GetCurrentPosition()
{
	return GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);
}

/// <summary>
/// Serialise the Data
/// </summary>
/// <param name="serializeParameters"></param>
/// <returns></returns>
RakNet::RM3SerializationResult TransformComponent::Serialize(RakNet::SerializeParameters* serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;

	//Write reliability type
	serializeParameters->pro[0].reliability = PacketReliability::RELIABLE;

	//Begin Serialization of the data that we are going to send
	m_variableDeltaSerializer.BeginIdenticalSerialize(
		&serializationContext,
		serializeParameters->whenLastSerialized == 0,
		&serializeParameters->outputBitstream[0]
	);

	m_variableDeltaSerializer.SerializeVariable(&serializationContext, m_m4EntityMatrix);

	//Send only if the data has changed
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void TransformComponent::Deserialize(RakNet::DeserializeParameters* deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;

	//Deserialise the data
	m_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);

	//Get the received matrix from the server and add to our transform history for interplotion
	glm::mat4 m4ServerReceivedTransform = m_m4EntityMatrix;
	m_variableDeltaSerializer.DeserializeVariable(&deserializationContext, m4ServerReceivedTransform);
	m_pTransformHistory->Write(m4ServerReceivedTransform, RakNet::GetTimeMS());

	m_variableDeltaSerializer.EndDeserialize(&deserializationContext);
}


/// <summary>
/// Orthogonalize the matrix so that the forward, right and up vectors are facing in their respective
/// directions.
/// This uses the Gram-Schmidt process (https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process)
/// </summary>
void TransformComponent::Orthogonalize()
{
	//Get the forward and up of the input vector
	glm::vec3 v3InputFWD = GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 v3InputUP = GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);

	//Get the dot product of the forwards and up vector
	float fFwdUpDot = glm::dot(v3InputFWD, v3InputUP);

	//Calculate the real up vector of the the matrix
	//upAxis = inputUp - (inputFWD * fwdUpDot)
	glm::vec3 v3RealUp = v3InputUP - (v3InputFWD * fFwdUpDot);
	v3RealUp = glm::length(v3RealUp) != 0.f ? glm::normalize(v3RealUp) : glm::vec3(0.f, 1.f, 0.f);

	//Get the right vector as the cross product of the 
	//realUp and inputFwd
	glm::vec3 v3RealRight = glm::cross(v3RealUp, v3InputFWD);
	v3RealRight = glm::length(v3RealRight) != 0.f ? glm::normalize(v3RealRight) : glm::vec3(1.f, 0.f, 0.f);

	//Set new Real Right and Forward Values
	SetEntityMatrixRow(MATRIX_ROW::UP_VECTOR, v3RealUp); //Set Real UP
	SetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR, v3RealRight);
}

// ReSharper disable CppInconsistentNaming - for RakNet functions

/// <summary>
/// Serialize the construction of the transform component sending our starting position to clients
/// </summary>
/// <param name="constructionBitstream"></param>
/// <param name="destinationConnection"></param>
void TransformComponent::SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection)

{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call adds another remote system to track
	m_variableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

	//Call base function
	Component::SerializeConstruction(constructionBitstream, destinationConnection);

	/*
	 * CONSTRUCTION DATA LAYOUT
	 * glm::vec3 StartPosition
	 */
	
	//Serialize the position we have
	glm::vec3 pos = GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);
	constructionBitstream->Write(pos);

}

/// <summary>
/// Serialize the construction of the transform component setting our start position to the one sent by the server
/// </summary>
/// <param name="constructionBitstream"></param>
/// <param name="sourceConnection"></param>
/// <returns></returns>
bool TransformComponent::DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//If bitstream is empty then early out
	if (constructionBitstream->GetNumberOfUnreadBits() == 0) {
		return false;
	}

	/*
	 * CONSTRUCTION DATA LAYOUT
	 * glm::vec3 StartPosition
	 */

	//Call base function
	Component::DeserializeConstruction(constructionBitstream, sourceConnection);

	//Get position value
	glm::vec3 receivedPosition;
	constructionBitstream->Read(receivedPosition);
	SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, receivedPosition);
	
	
	return true;
}

// ReSharper restore CppInconsistentNaming - for RakNet functions