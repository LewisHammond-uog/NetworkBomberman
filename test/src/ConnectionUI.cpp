#include "stdafx.h"
#include "ConnectionUI.h"

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
