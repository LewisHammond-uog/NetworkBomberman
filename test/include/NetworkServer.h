#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__

//Project Includes
#include "Network.h"

class NetworkServer : public ServerClientBase
{
public:
	//Constructor/Destructor
	NetworkServer();
	~NetworkServer();

	//Update and Init
	void Init();
	void Update();

private:
	//Functions for pre game connection of clients 
	//to the server
	void DoPreGameServerEvents();
};

#endif // !__NETWORK_SERVER_H__