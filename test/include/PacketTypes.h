#ifndef __PACKET_TYPES_H__
#define __PACKET_TYPES_H__
/// <summary>
/// File that contains definitions for all of the types of packets that
/// the server/client can receive/send
/// </summary>

struct GameDataPacket{
	CSGameMessages m_packetType; //Type of packet
	RakNet::NetworkID m_recepient; //Network ID of Intended Recepient
	GameDataPacket* m_packetData; //Actual Packet Data (i.e updated transform, etc.)
};

struct TestPacket : public GameDataPacket {

};


#endif // !__PACKET_TYPES_H__