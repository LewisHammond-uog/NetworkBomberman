#ifndef __NETWORK_CLIENT_H__
#define __NETWORK_CLIENT_H__

//Project includes
#include "Network.h"

//Forward declare for Raknet
namespace RakNet {
	class RakPeerInterface;
}

class NetworkClient : public Network
{
public:
	//Constructor/Destructor
	NetworkClient();
	~NetworkClient();

	void Update();

private:

};

#endif //!__NETWORK_CLIENT_H__

