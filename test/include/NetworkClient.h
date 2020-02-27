#ifndef __NETWORK_CLIENT_H__
#define __NETWORK_CLIENT_H__

//Project includes
#include "ClientServerBase.h"

class NetworkClient : public ServerClientBase
{
public:

	//Client Running State
	typedef enum ClientLocalState {
		NOT_CONNECTED,
		PRE_GAME,
		GAME_PLAYING
	};

	//Client Connection States
	typedef enum ClientConnectionState {
		//Setup Connection to the server
		CLIENT_START_CONNECTION,
		CLIENT_WAITING_FOR_CONNECTION,
		CLIENT_ENTER_AUTH_DETAILS,
		CLIENT_WAITING_FOR_AUTHORISATION,

		//Setup Game
		CLIENT_INIT_PREGAME,
		CLIENT_SEND_READY,
		CLIENT_WAITING_FOR_GAME_START,

		//Start Game

		//Game Shutdown

		CLIENT_MAX_CONNECTION_STATES
	} ClientConnectionState;

	//Constructor/Destructor
	NetworkClient();
	~NetworkClient();

	void Init();
	void Update();

private:

	//Functions to run different network events based on 
	//the current game state
	void DoClientConnectionEvents();
	void DoClientPreGameEvents();

	//Local (i.e game running, loading) & Connection State
	ClientLocalState m_eClientGameState;
	ClientConnectionState m_eConnectionState;

};

#endif //!__NETWORK_CLIENT_H__

