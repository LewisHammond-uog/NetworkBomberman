#ifndef __AUTHENTICATOR_H__
#define __AUTHENTICATOR_H__

class Authenticator {
public:
	//Constructors/Destructors
	Authenticator();
	~Authenticator();


	//Authenticate an existing user from a username
	//password - Hashed passwords not yet implemented
	bool AuthenticateUser(const char* a_szUsername, const char* a_szPassword);

	//Register a new user to the system
	bool RegisterNewUser(const char* a_szUsername, const char* a_szPassword);

private:

	//Checks if a username exists within the login credentials file
	bool UsernameExists(const char* a_szUsername);
	//Checks if a username and password match an item within the file
	bool ValidUsernameAndPassword(const char* a_szUsername, const char* a_szPassword);


	//Const filename for the username/password file
	const char* mc_LoginDetailsFileName = "authDetails.txt";

	//Max Size of username and password
	const static int mc_iMaxUsernameLen = 25;
	const static int mc_iMaxPasswordLen = 25;


};

#endif // __AUTHENTICATOR_H__