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
/// Authenticates if a username/password exists within the username/password file
/// </summary>
/// <param name="a_username">Username to check</param>
/// <param name="a_password">Password to check</param>
/// <returns>If login details are correct</returns>
bool Authenticator::AuthenticateUser(const char* a_username, const char* a_password)
{
	//Check that username/password does not exceed size limits
	if (std::strlen(a_username) > mc_iMaxUsernameLen || std::strlen(a_password) > mc_iMaxPasswordLen) {
		//Over max size
		return false;
	}

	//Open File for reading
	std::fstream loginDetailsFile = std::fstream(mc_LoginDetailsFileName, std::ios_base::in);

	//If we cannot open the file then return false
	//- cannot authenticate
	if (!loginDetailsFile.is_open()) {
		//File Failed to open
		return false;
	}

	//Check that username is valid
	//Loop through the username file if the usernames match
	char storedUsername[mc_iMaxUsernameLen] = "/0";
	char storedPassword[mc_iMaxUsernameLen] = "/0";
	while (strcmp(storedUsername, a_username)) {

		//Check if we have reached the end of the file
		//if we have the username does not exist
		if (loginDetailsFile.eof()) {
			//End of file - username not found
			return false;
		}

		//Read in the username and password for the current line
		loginDetailsFile >> storedUsername;
		loginDetailsFile >> storedPassword;
	}

	//Check that password matches it's username pair,
	//if it doesn't return false - no authentication
	if (strcmp(storedPassword, a_password)) {
		return false;
	}

	//All checks passed - return true
	return true;
}

