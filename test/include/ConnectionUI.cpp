#include "stdafx.h"
#include "ConnectionUI.h"

//C++ Includes
#include <vector>

//Project Includes
#include "Authenticator.h"
#include "LevelManager.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

//Static Initalistations
const int ConnectionUI::sc_iMinPlayers = 2;
const int ConnectionUI::sc_iMaxPlayers = 6;

/// <summary>
/// Draws UI for waiting 
/// </summary>
/// <param name="a_szWaitingMessage"></param>
void ConnectionUI::DrawWaitingUI(const char* a_szWaitingMessage)
{
	InitConnectionWindow();
	//Start a window with the title of our message
	ImGui::Begin(a_szWaitingMessage);

	//Add the %c so we can display the waiting icon
	std::string messageStr(a_szWaitingMessage);
	messageStr.append("... %c"); //Append ... and waiting icon

	//Draw Text
	ImGui::TextWrapped(messageStr.c_str(), "|/-\\"[(int)(Utility::getTotalTime() / 0.05f) & 3]);

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
	InitConnectionWindow();
	//Start UI with titile
	ImGui::Begin(a_szWindowTitle);

	//Draw the content
	ImGui::TextWrapped(a_szContent);

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
	InitConnectionWindow();
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

/// <summary>
/// Draw the server settings UI
/// </summary>
/// <returns>If the Start Server button is pressed</returns>
bool ConnectionUI::DrawServerSettingsUI(int& a_iMaxPlayerCount, int& a_iMinReadyPlayers, float& a_iWarmupTime, std::vector<std::string>& a_vsSelectedLevels)
{
	static std::vector<std::string> vsAvailableLevels = LevelManager::GetLoadableLevelNames(); //List of levels that can be loaded

	//Bool for if the start server UI
	bool bStartPressed = false;
	
	//Force Min Ready Player Count to be under or equal to max players
	a_iMinReadyPlayers = a_iMinReadyPlayers > a_iMaxPlayerCount ? a_iMaxPlayerCount : a_iMinReadyPlayers;

	InitServerSettingsWindow();
	ImGui::Begin("Configure Server");

	//Player Count Settings
	if (ImGui::CollapsingHeader("Player Count Settings")) {
		
		ImGui::TextWrapped("Select the Maximum number of players that connect to the server");
		ImGui::SliderInt("Player Count", &a_iMaxPlayerCount, sc_iMinPlayers, sc_iMaxPlayers);
		
		ImGui::TextWrapped("Select the number of players that must ready up before the game starts");
		ImGui::SliderInt("Ready Player Count", &a_iMinReadyPlayers, sc_iMinPlayers, a_iMaxPlayerCount);
	}

	//Level Settings
	if(ImGui::CollapsingHeader("Timing Settings"))
	{
		ImGui::TextWrapped("The time between %i client(s) being ready and the game starting", a_iMinReadyPlayers);
		ImGui::InputFloat("Warmup Time", &a_iWarmupTime);
	}

	//Level Settings
	if(ImGui::CollapsingHeader("Level Selection"))
	{
		//Text for describing how to add levels
		ImGui::TextWrapped("Select Levels in the left hand columns to add them to the rotation");
		
		
		//List of levels we can add to the rotation
		ImGui::Columns(2);
		ImGui::TextWrapped("Select Levels to add");
		ImGui::Spacing();
		std::vector<std::string> vsAvaliableLevels = LevelManager::GetLoadableLevelNames();
		for(int i = 0; i < vsAvaliableLevels.size(); ++i)
		{
			if(ImGui::Button(vsAvaliableLevels[i].c_str()))
			{
				a_vsSelectedLevels.push_back(vsAvaliableLevels[i]);
			}
		}


		//---SELECTED LEVELS-------//
		ImGui::NextColumn();
		ImGui::TextWrapped("Level Rotation");
		ImGui::Spacing();
		if (!a_vsSelectedLevels.empty()) {
			
			//Draw a list of selected levels
			for (int i = 0; i < a_vsSelectedLevels.size(); ++i)
			{
				ImGui::Text(a_vsSelectedLevels[i].c_str());
			}

			//Buttons for clearing list and removing last
			if (ImGui::Button("Remove Last"))
			{
				a_vsSelectedLevels.pop_back();
			}
			if (ImGui::Button("Clear"))
			{
				a_vsSelectedLevels.clear();
			}
		}

		ImGui::Columns(1);
	}

	//Start Button
	//Don't allow start if there are no levels
	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	if(!a_vsSelectedLevels.empty())
	{
		bStartPressed = ImGui::Button("Start Server");

	}else
	{
		ImGui::TextDisabled("Cannot Start Server with no levels");
	}

	ImGui::End();
	return bStartPressed;
}

/// <summary>
/// Draws the client Disconnect UI
/// </summary>
/// <returns>If the disconnect button has been pressed</returns>
bool ConnectionUI::DrawClientDisconnectUI()
{
	//Bool for if we have pressed disconnect
	bool bDisconnectPressed = false;
	
	InitDisconnectWindow();

	ImGui::Begin("Disconnect from Server");
	bDisconnectPressed = ImGui::Button("Disconnect from Server");
	ImGui::End();

	return bDisconnectPressed;
}

/// <summary>
/// Intalises the ImGUI window for client connection
/// </summary>
void ConnectionUI::InitConnectionWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 windowSize = ImVec2(400.f, 250.f);
	const ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
}

/// <summary>
/// Intalises the ImGUI window for client connection
/// </summary>
void ConnectionUI::InitDisconnectWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 windowSize = ImVec2(180.f, 60.f);
	const ImVec2 windowPos = ImVec2(0,0);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
}

/// <summary>
/// Intalises the ImGUI window for server settings
/// </summary>
void ConnectionUI::InitServerSettingsWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 windowSize = ImVec2(900.f, 900.f);
	const ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
}
