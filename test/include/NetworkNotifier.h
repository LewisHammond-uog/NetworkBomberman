#ifndef __NETWORK_NOTIFIER_H__
#define __NETWORK_NOTIFIER_H__

//C++ Includes
#include <vector>
#include <type_traits>

//RakIncludes
#include "NetworkIDObject.h"
#include "BitStream.h"

//Project Includes
#include "ClientServerBase.h"
#include "PacketTypes.h"

/// <summary>
/// Struct that contains infomation collected from
/// a network packet
/// PACKET TYPE
/// RECEPIENT
/// ACUTAL PACKET DATA
/// </summary>
struct ReceivedObjectPacket {
	//Intended Recepient of the packet
	CSGameMessages m_packetType; //Type
	RakNet::NetworkID m_recepient; //Recepient
	GamePacket* m_packetData; //Actual Packet Data (i.e updated transform, etc.)
};

struct ReceivedMngrPacketInfo {
	CSGameMessages m_packetType; //Type
	//MANAGER TYPE GOES HERE
	RakNet::BitStream m_packetData; //Actual Packet Data (i.e updated transform, etc.)
};

class NetworkNotifier
{
public:

	//---RECEIVED PACKETS----//
	//Get the packets relating to the the given object ID
	std::vector<ReceivedObjectPacket*> GetReceivedObjectPackets(RakNet::NetworkIDObject* a_objectID);
	//Add a packet to the Object Packet list
	static void AddReceivedObjectPacket(/*RakNet::BitStream& a_receivedPacket*/);

	//---SENDING PACKETS---//
	//Get all of the packets to send from an object
	

private:
	std::vector<ReceivedObjectPacket*> m_vUnproccessedPackets;

};

#endif // !__NETWORK_NOTIFIER_H__
