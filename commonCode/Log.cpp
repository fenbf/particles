/** @file log.cpp
*  @brief loging functionalities
*
*	@author Bartlomiej Filipek 
*/

#include "commonCode.h"
#include "Log.h"
#include <sstream> 

static std::vector<std::string> messageList;

namespace logger
{
    std::vector<std::string> getMessageList()
    {
        return messageList;
    }

    const char *fileNameFromPath(const char *filePath)
    {
        const char *s = strrchr(filePath, '\\');
        if (s == NULL) 
            s = strrchr(filePath, '/');
        return s == NULL ? filePath : s+1;
    }

#ifdef WIN32

#ifdef _MSC_VER
	#define my_strcat strcat_s
	#define my_sprintf sprintf_s
	#define my_vsprintf vsprintf_s
#else
	#define my_strcat strcat
	#define my_sprintf sprintf
	#define my_vsprintf vsprintf
#endif

    void winOutDebugString(bool addToMsgList, const char *func, const char * file, int line, const char *msg, const char *userMsg, ...)
    {
        static char strBuf[512];
        static char strTemp[512];

        strBuf[0] = '\0';
        if (msg)
        {
            my_strcat(strBuf, msg);
            my_strcat(strBuf, " ");
            if (func)
            {
                my_strcat(strBuf, "in ");
                my_strcat(strBuf, func);
                my_strcat(strBuf, ", ");
            }
            if (file && line > 0)
            {
                my_sprintf(strTemp, "%s (%d)", fileNameFromPath(file), line);
                my_strcat(strBuf, strTemp);
                my_strcat(strBuf, ": ");
            }
            OutputDebugString(strBuf);

            if (addToMsgList)
                messageList.push_back(strBuf);
        }

        va_list		ap;

        if (userMsg == NULL) return; 

        strBuf[0] = '\0';
        va_start(ap, userMsg);					// Parses The String For Variables
        my_vsprintf(strBuf, userMsg, ap);		// And Converts Symbols To Actual Numbers
        va_end(ap);
        my_strcat(strBuf, "\n");

        OutputDebugString(strBuf);

        if (addToMsgList)
                messageList.push_back(strBuf);
    }
#endif // WIN32

#if defined (UTILS_LOG_WITH_OUTPUT_DBG_STRING) && defined(WIN32)

    void presentLastMessagesToUser()
    {
        std::stringstream str;
        for (auto &s : messageList)
            str << s << std::endl;

        MessageBox(GetActiveWindow(), str.str().c_str(), "Log", MB_OK);
    }
#else
	void presentLastMessagesToUser()
	{
		getchar();
	}
#endif
} // namespace utils
