#ifndef __AUTHENTICATOR_H__
#define __AUTHENTICATOR_H__

class Authenticator {
public:
	//Constructors/Destructors
	Authenticator();
	~Authenticator();


	//Authenticate an existing user from a username
	//password - Hashed passwords not yet implemented
	bool AuthenticateUser(const char* a_szUsername, const char* a_szPassword/*, bool a_passwordIsHashed = false*/);

	//Register a new user to the system
	bool RegisterNewUser(const char* a_username, const char* a_password/*, bool a_passwordIsHashed = false*/);

private:

	//Checks if a username exists within the login credentials file
	bool UsernameExists(const char* a_szUsername, std::fstream& a_rFileStream);
	//Checks if a username and password match an item within the file
	bool ValidUsernameAndPassword(const char* a_szUsername, const char* a_szPassword, std::fstream& a_rFileStream);


	//Const filename for the username/password file
	const char* mc_LoginDetailsFileName = "authDetails.txt";

	//Max Size of username and password
	const static int mc_iMaxUsernameLen = 25;
	const static int mc_iMaxPasswordLen = 25;


};

#endif // __AUTHENTICATOR_H__