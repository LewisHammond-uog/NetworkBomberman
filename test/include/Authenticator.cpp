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

//TO DO - Coding Standards Check

/// <summary>
/// Authenticates if a username/password exists within the username/password file
/// </summary>
/// <param name="a_username">Username to check</param>
/// <param name="a_password">Password to check</param>
/// <returns>If login details are correct</returns>
bool Authenticator::AuthenticateUser(const char* a_szUsername, const char* a_szPassword)
{
	//Check that username/password does not exceed size limits
	if (std::strlen(a_szUsername) > mc_iMaxUsernameLen || std::strlen(a_szPassword) > mc_iMaxPasswordLen) {
		//Over max size
		return false;
	}

	//Open auth details file
	std::fstream sLoginDetailsFile = std::fstream(mc_LoginDetailsFileName, std::ios_base::in);

	//If we cannot open the file then return false
	//- cannot check username/password
	if (!sLoginDetailsFile.is_open()) {
		//File Failed to open
		return false;
	}
	
	bool bAuthDetailsValid = ValidUsernameAndPassword(a_szUsername, a_szPassword, sLoginDetailsFile);

	//Flush and close file
	sLoginDetailsFile.flush();
	sLoginDetailsFile.close();

	//Return if the auth details were valid
	return bAuthDetailsValid;
}

/// <summary>
/// Checks if a username already exists within the auth details file
/// Assumes that username is not over max limits
/// </summary>
/// <param name="a_username">Username to check</param>
/// <param name="a_fStream">File stream of file to check</param>
/// <returns>If username exists</returns>
bool Authenticator::UsernameExists(const char* a_szUsername, std::fstream& a_rFileStream)
{
	//Init Char Array
	char storedUsername[mc_iMaxUsernameLen] = "";
	char storedPassword[mc_iMaxUsernameLen] = "";
	while (a_rFileStream >> storedUsername >> storedPassword) {
		//Check that the input username does not equal the current username
		if (!strcmp(a_szUsername, storedUsername)) {
			return false;
		}
	}

	//Did not find username in file
	return true;
}

bool Authenticator::ValidUsernameAndPassword(const char* a_szUsername, const char* a_szPassword, std::fstream& a_rFileStream)
{
	//Init Char Array
	char storedUsername[mc_iMaxUsernameLen] = "";
	char storedPassword[mc_iMaxUsernameLen] = "";

	//Loop through usernames until we either get to
	//the given username or 
	while (strcmp(a_szUsername, storedUsername)) {
		//Check that we are not at the end of the file
		if (a_rFileStream.eof()) {
			//At end of file without username being found - username invalid
			return false;
		}

		//Store the next username and password in the file
		a_rFileStream >> storedUsername >> storedPassword;
	}
	
	//Loop completed - username was found
	//Compare the password for that username to the inputted
	//password
	if (!strcmp(a_szPassword, storedPassword)) {
		//Username and password match, return true
		return true;
	}

	//Username and password did not match, return false
	return false;

}

