#ifndef __AUTHENTICATOR_H__
#define __AUTHENTICATOR_H__

//Raknet Includes
#include "BitStream.h"

/// <summary>
/// Class used for Authenticating the user with a username or password,
/// or registering a new user
/// </summary>
class Authenticator {
public:
	//Constructors/Destructors
	Authenticator() = default;
	~Authenticator() = default;

	//Login or Register an existing user from a bitstream
	bool LoginFromBitstream(RakNet::BitStream& a_bitStream, bool a_bRegisterNewUser) const;

	//Authenticate an existing user from a username and password
	bool AuthenticateExistingUser(const char* a_szUsername, const char* a_szPassword) const;
	
	//Register a new user to the system
	bool RegisterNewUser(const char* a_szUsername, const char* a_szPassword) const;

	//Max/Min Size of username and password
	const static int mc_iMaxUsernameLen = 25;
	const static int mc_iMaxPasswordLen = 25;
	const static int mc_iMinUsernameLen = 3;
	const static int mc_iMinPasswordLen = 3;

private:

	//Checks if a username exists within the login credentials file
	bool UsernameExists(const char* a_szUsername) const;
	//Checks if a username and password match an item within the file
	bool ValidUsernameAndPassword(const char* a_szUsername, const char* a_szPassword) const;


	//Const filename for the username/password file
	const char* mc_sLoginDetailsFileName = "authDetails.txt";




};

#endif // __AUTHENTICATOR_H__