/**
* @file DLog.h
*
* @author $Author
*
* @version $Version
**/

#ifndef DLOG_H
#define DLOG_H

#include <string>
#include <sstream>

//! Colors defines for printf
#define CL_RED     "\x1b[31m"
#define CL_GREEN   "\x1b[32m"
#define CL_YELLOW  "\x1b[33m"
#define CL_BLUE    "\x1b[34m"
#define CL_MAGENTA "\x1b[35m"
#define CL_CYAN    "\x1b[36m"
#define CL_DEFAULT "\x1b[0m"

//! Debug levels defines
#define DL_LEVEL_DEBUG      0
#define DL_LEVEL_INFO       1
#define DL_LEVEL_WARNING    2
#define DL_LEVEL_ERROR      3

/**
* @class DLog
*
* @brief C++ std class for logging to stdout, stderr, file.
*
* @author $Author
**/
class DLog
{
    public:
        //! Constructor
        DLog(std::string LogFilename = "", bool StdoutEnabled = true);
        //! Destructor
        ~DLog();
        //! Set an additional output stream for log output
        //void SetAdditionalStream(std::ostringstream *outputStream);
        //! Get all log content
        std::string Read(void);

        //! Debug log
        void d(const std::string fmt,...);
        //! Info log
        void i(const std::string fmt,...);
        //! Error log
        void e(const std::string fmt,...);
        //! Warning log
        void w(const std::string fmt,...);

    private:
        //! Write the the message on stdout, stderr, file
        void Write(unsigned char Level, std::string LogMsg);

        //! Create a formatted string
        template<typename ... Args> std::string format_string(const std::string& format, Args ... args);

        std::string Filename;
        FILE *hFile;
        bool LogToFile;
        bool LogToStdout;
};

#endif // DLOG_H
