#ifndef __CONNECTION_UI_H__
#define __CONNECTION_UI_H__


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
	
};

#endif
