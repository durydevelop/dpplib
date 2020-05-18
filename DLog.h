/**
* @file DLog.h
*
* @author $Author
*
* @version $Version
**/

#ifndef DLogH
#define DLogH

#include <string>
#include <sstream>

namespace DTools {
    namespace DLog {
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

                std::string Filename;
                FILE *hFile;
                bool LogToFile;
                bool LogToStdout;
        };
    }
}
#endif
