#ifndef __CONSOLE_LOG_H__
#define __CONSOLE_LOG_H__

#include "Application_Log.h"

class ConsoleLog
{
public:
	static void LogConsoleMessage(const char* m_Message) {
		Application_Log* log = Application_Log::Get();

		if (log != nullptr) {
			log->addLog(LOG_LEVEL::LOG_INFO, m_Message);
		}
	}
};

#endif //!__CONSOLE_LOG_H__