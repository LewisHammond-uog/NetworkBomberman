#include "stdafx.h"
#include "NetworkNotifier.h"

/// <summary>
/// Add a received object packet to the list that can be accessed by 
/// game objects
/// </summary>
/// <param name="a_packetToAdd">Raw Packet to add to the list</param>
void NetworkNotifier::AddReceivedObjectPacket(/*RakNet::BitStream& a_receivedPacket*/)
{
	//Create new struct
	ReceivedObjectPacket newPacket;
	//a_receivedPacket.Read(newPacket.m_packetType);
	//a_receivedPacket.Read(newPacket.m_recepient);
	
	//Create a test packet and it pointer to the 
	//TestPacket (bigger than the GamePacket) and set a pointer
	//to it to the m_packetData that is just a pointer to 
	//a GamePacket (the smaller data structure)
	TestPacket p;
	p.x = 2;
	newPacket.m_packetData = &p;

	//This can then be coverted back to a test packet like so
	TestPacket tp = *static_cast<TestPacket*>(newPacket.m_packetData);
	int i = tp.x;
	
}
