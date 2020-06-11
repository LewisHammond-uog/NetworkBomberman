#ifndef __CONNECTION_UI_H__
#define __CONNECTION_UI_H__
#include <vector>


/// <summary>
/// Static class to draw different elements of the Connection UI
/// </summary>
class ConnectionUI
{
public:

	//Function to Draw UI that shows a waiting indicator
	static void DrawWaitingUI(const char* a_szWaitingMessage);
	//Function to Draw a UI that requires acknolagement
	static bool DrawAcknowledgeUI(const char * a_szWindowTitle, const char* a_szContent, const char* a_szButtonText = "Ok");

	//Function to draw Login UI
	static void DrawLoginUI(char* a_sUsername, char* a_sPassword, bool& a_bLogin, bool& a_bRegister);

	//Draw the connection UI
	static void DrawGameOverUI(bool& a_bContinue, bool& a_bDisconnect);

	//Draw the server settings UI
	static bool DrawServerSettingsUI(int& a_iMaxPlayerCount, int& a_iMinReadyPlayers, float& a_iWarmupTime, std::
	                                 vector<std::string>& a_vsSelectedLevels);

	//Draw the client disconnect UI
	static bool DrawClientDisconnectUI();
	
private:
	//Functions to init window types
	static void InitConnectionWindow();
	static void InitServerSettingsWindow();
	static void InitDisconnectWindow();

	//Settings for UI
	static const int sc_iMaxPlayers;
	static const int sc_iMinPlayers;
	static const int mc_iMinReadyPlayers;
	
};

#endif
