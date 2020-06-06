#ifndef __NETWORK_ORDERING_CHANNELS_H__
#define __NETWORK_ORDERING_CHANNELS_H__

/// <summary>
/// Contains a list of defines for ordering channels used within RakNet,
/// this is so that ordered messages do not get in the way of eachoher
/// (i.e a control pressed notification does not block a disconnect notification)
/// </summary>

enum ORDERING_CHANNELS
{
	ORDERING_CHANNEL_GENERAL = 0, //Used for general messages
	ORDERING_CHANNEL_LOGIN = 1, //Used for Login]
	ORDERING_CHANNEL_CONNECTIONS = 2 //Used for Connections/Disconnection
};

#endif //!__NETWORK_ORDERING_CHANNELS_H__
