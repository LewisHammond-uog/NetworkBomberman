#include "stdafx.h"
#include "Authenticator.h"

//C++ Includes
#include <fstream>
#include <iostream>

//Blank Default Constructor
Authenticator::Authenticator()
{
}

//Blank Default Destructor
Authenticator::~Authenticator()
{
}

/// <summary>
/// Login to the server from a raw bit stream sent from the client (withough ignoring the message ID).
/// Either logging in as an existing user or registering as a new one
/// </summary>
/// <param name="a_loginData">Bitstream Login Data</param>
/// <param name="a_bRegisterNewUser">Whether to register as a new user</param>
/// <returns>If login details are correct</returns>
bool Authenticator::LoginFromBitstream(RakNet::BitStream& a_loginData, const bool a_bRegisterNewUser = false)
{
	//Strip Message ID From Bitstream
	a_loginData.IgnoreBytes(sizeof(RakNet::MessageID));

	//Read in given username and password
	char username[Authenticator::mc_iMaxUsernameLen];
	char password[Authenticator::mc_iMaxPasswordLen];
	a_loginData.Read(username, Authenticator::mc_iMaxUsernameLen * sizeof(char));
	a_loginData.Read(password, Authenticator::mc_iMaxPasswordLen * sizeof(char));

	//Call other Function with new extracted info based on whether to register
	//a new user or not
	if (a_bRegisterNewUser) {
		//Register a new user
		return RegisterNewUser(username, password);
	}
	else {
		//Authenticate an existing user
		return AuthenticateExistingUser(username, password);
	}
}

/// <summary>
/// Authenticates if a username/password exists within the username/password file
/// </summary>
/// <param name="a_szUsername">Username to check</param>
/// <param name="a_szPassword">Password to check</param>
/// <returns>If login details are correct</returns>
bool Authenticator::AuthenticateExistingUser(const char* a_szUsername, const char* a_szPassword)
{
	//Check that username/password does not exceed size limits
	if (std::strlen(a_szUsername) > mc_iMaxUsernameLen || std::strlen(a_szPassword) > mc_iMaxPasswordLen) {
		//Over max size
		return false;
	}

	bool bAuthDetailsValid = ValidUsernameAndPassword(a_szUsername, a_szPassword);

	//Return if the auth details were valid
	return bAuthDetailsValid;
}

bool Authenticator::RegisterNewUser(const char* a_szUsername, const char* a_szPassword)
{
	//Check that username/password does not exceed size limits
	if (std::strlen(a_szUsername) > mc_iMaxUsernameLen || std::strlen(a_szPassword) > mc_iMaxPasswordLen) {
		//Over max size
		return false;
	}

	//Check if the username already exists within the file, if 
	//it does reject registration
	if (UsernameExists(a_szUsername)) {
		return false;
	}

	//Open auth details file
	std::fstream sLoginDetailsFile = std::fstream(mc_LoginDetailsFileName, std::ios_base::app);
	//If we cannot open the file then return false
	//- cannot add new username/password
	if (!sLoginDetailsFile.is_open()) {
		//File Failed to open
		return false;
	}

	//Username does not already exist within the file
	//Write new username and password to file with a space seperator
	sLoginDetailsFile << a_szUsername << " " << a_szPassword << std::endl;

	//Close File
	sLoginDetailsFile.flush();
	sLoginDetailsFile.close();
	
	//Return true because we have now written the new username/password
	return true;
}

/// <summary>
/// Checks if a username already exists within the auth details file
/// Assumes that username is not over max limits
/// </summary>
/// <param name="a_szUsername">Username to check</param>
/// <returns>If username exists</returns>
bool Authenticator::UsernameExists(const char* a_szUsername) const
{
	//Open auth details file
	std::fstream sLoginDetailsFile = std::fstream(mc_LoginDetailsFileName, std::ios_base::in);

	//If we cannot open the file then return false
	//- cannot check username/password
	if (!sLoginDetailsFile.is_open()) {
		//File Failed to open
		return false;
	}

	//Init Char Array
	char storedUsername[mc_iMaxUsernameLen] = "";
	char storedPassword[mc_iMaxUsernameLen] = "";
	while (!sLoginDetailsFile.eof()) {

		//Load in username/password
		sLoginDetailsFile >> storedUsername >> storedPassword;

		//Check that the input username does not equal the current username,
		//if it does return true
		if (!strcmp(a_szUsername, storedUsername)) {
			//Flush and close file
			sLoginDetailsFile.flush();
			sLoginDetailsFile.close();
			return true;
		}
	}

	//Did not find username in file
	//Flush and close file
	sLoginDetailsFile.flush();
	sLoginDetailsFile.close();
	return false;
}

bool Authenticator::ValidUsernameAndPassword(const char* a_szUsername, const char* a_szPassword) const
{

	//Open auth details file
	std::fstream sLoginDetailsFile = std::fstream(mc_LoginDetailsFileName, std::ios_base::in);

	//If we cannot open the file then return false
	//- cannot check username/password
	if (!sLoginDetailsFile.is_open()) {
		//File Failed to open
		return false;
	}

	//Init Char Array
	char storedUsername[mc_iMaxUsernameLen] = "";
	char storedPassword[mc_iMaxUsernameLen] = "";

	//Loop through usernames until we either get to
	//the given username or 
	while (strcmp(a_szUsername, storedUsername) != 0) {
		//Check that we are not at the end of the file
		if (sLoginDetailsFile.eof()) {
			//At end of file without username being found - username invalid
			//Flush and close file
			sLoginDetailsFile.flush();
			sLoginDetailsFile.close();
			return false;
		}

		//Store the next username and password in the file
		sLoginDetailsFile >> storedUsername >> storedPassword;
	}
	
	//Loop completed - username was found
	//Compare the password for that username to the inputted
	//password
	if (!strcmp(a_szPassword, storedPassword)) {
		//Username and password match, return true
		//Flush and close file
		sLoginDetailsFile.flush();
		sLoginDetailsFile.close();
		return true;
	}

	//Flush and close file
	sLoginDetailsFile.flush();
	sLoginDetailsFile.close();

	//Username and password did not match, return false
	return false;
}

