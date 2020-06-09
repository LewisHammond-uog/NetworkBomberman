#include "stdafx.h"
#include "ConnectionUI.h"

//Project Includes
#include "Authenticator.h"

/// <summary>
/// Draws UI for waiting 
/// </summary>
/// <param name="a_szWaitingMessage"></param>
void ConnectionUI::DrawWaitingUI(const char* a_szWaitingMessage)
{
	//Start a window with the title of our message
	ImGui::Begin(a_szWaitingMessage);

	//Add the %c so we can display the waiting icon
	std::string messageStr(a_szWaitingMessage);
	messageStr.append("... %c"); //Append ... and waiting icon

	//Draw Text
	ImGui::Text(messageStr.c_str(), "|/-\\"[(int)(Utility::getTotalTime() / 0.05f) & 3]);

	//End UI
	ImGui::End();
}

/// <summary>
/// Draws a UI that requires acknowledgement with an "ok" button
/// </summary>
/// <param name="a_szWindowTitle">Title of the Window</param>
/// <param name="a_szContent">Content within the window</param>
/// <param name="a_szButtonText">Text to display on the button</param>
/// <returns>If the acknowlage button is pressed</returns>
bool ConnectionUI::DrawAcknowledgeUI(const char* a_szWindowTitle, const char* a_szContent, const char* a_szButtonText /* = "Ok"*/)
{
	//Start UI with titile
	ImGui::Begin(a_szWindowTitle);

	//Draw the content
	ImGui::Text(a_szContent);

	//Get the result of the button
	const bool bAckPressed = ImGui::Button(a_szButtonText);

	//End UI
	ImGui::End();

	//Return the result of the button press
	return bAckPressed;
}

/// <summary>
/// Draws the Login UI and changes the values that are given to this
/// function by reference
/// </summary>
/// <param name="a_sUsername">[REF] Username to fill</param>
/// <param name="a_sPassword">[REF] Password to fill</param>
/// <param name="a_bLogin">[REF] If the login button has been pressed</param>
/// <param name="a_bRegister">[REF] If the register button has been pressed</param>
void ConnectionUI::DrawLoginUI(char* a_sUsername,
	char* a_sPassword, bool& a_bLogin, bool& a_bRegister)
{
	ImGui::Begin("Login to Server");

	//Help Text
	ImGui::TextWrapped("To connect to the server you must enter login credentials. If you are new please register as a new user");
	ImGui::TextWrapped("Usernames must be between %i and %i characters long. Passwords must be %i and %i characters long",
		Authenticator::mc_iMinUsernameLen, Authenticator::mc_iMaxUsernameLen, Authenticator::mc_iMinPasswordLen, Authenticator::mc_iMaxPasswordLen);

	//Enter Credentials
	ImGui::InputText("Enter Username: ", a_sUsername, Authenticator::mc_iMaxUsernameLen);
	ImGui::InputText("Enter Password: ", a_sPassword, Authenticator::mc_iMaxPasswordLen);

	//Buttons
	a_bLogin = ImGui::Button("Login");
	a_bRegister = ImGui::Button("Register");

	ImGui::End();
}

/// <summary>
/// Shows the game over UI
/// </summary>
/// <param name="a_bContinue">[REF] If the continue button was pressed</param>
/// <param name="a_bDisconnect">[REF] If the disconnect button was pressed</param>
void ConnectionUI::DrawGameOverUI(bool& a_bContinue, bool& a_bDisconnect)
{
	ImGui::Begin("Game Over");

	//Flavour Text
	ImGui::TextWrapped("Game Over! Do you want to play another round or disconnect?");

	//buttons
	a_bContinue = ImGui::Button("Continue");
	a_bDisconnect = ImGui::Button("Disconnect");

	ImGui::End();
}

