#ifndef __CONSOLE_LOG_H__
#define __CONSOLE_LOG_H__

#include "Application_Log.h"

/// <summary>
/// Class for logging to the console
/// </summary>
class ConsoleLog
{
public:

	/// <summary>
	/// Log a message to the console
	/// </summary>
	/// <param name="a_message">Message to Log</param>
	static void LogMessage(const char* a_message) {
		Application_Log* log = Application_Log::Get();

		if (log != nullptr) {
			log->addLog(LOG_LEVEL::LOG_INFO, a_message);
		}
	}

	/// <summary>
	/// Log an error to the console
	/// </summary>
	/// <param name="a_message">Error Message</param>
	static void LogError(const char* a_message) {
		Application_Log* log = Application_Log::Get();

		if (log != nullptr) {
			log->addLog(LOG_LEVEL::LOG_ERROR, a_message);
		}
	}
	
};

#endif //!__CONSOLE_LOG_H__