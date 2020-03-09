#ifndef __PACKET_TYPES_H__
#define __PACKET_TYPES_H__
/// <summary>
/// File that contains definitions for all of the types of packets that
/// the server/client can receive/send
/// </summary>

class GamePacket{
};

class TestPacket : public GamePacket {
public:
	int x = 0;
	int y = 0;
	int z = 0;
};


#endif // !__PACKET_TYPES_H__