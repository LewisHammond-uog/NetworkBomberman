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
	std::vector<GameDataPacket*> GetReceivedObjectPackets(RakNet::NetworkIDObject* a_objectID);
	//Add a packet to the Object Packet list
	static void AddReceivedObjectPacket(RakNet::Packet* a_receivedPacket);

	//---SENDING PACKETS---//
	//Get all of the packets to send from an object
	

private:
	std::vector<GameDataPacket*> m_vUnproccessedPackets;

};

#endif // !__NETWORK_NOTIFIER_H__
