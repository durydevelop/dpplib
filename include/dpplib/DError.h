#ifndef DErrorH
#define DErrorH

#ifdef _WIN32
    #include<windows.h>
    #include <lmerr.h>
#endif

#include <string>

namespace DTools
{
namespace DError
{
    class DErrorCode {
        public:
            enum DErrorCodeID { ERR_NOT_SET = -1, ERR_SUCCESS, ERR_WARNING, ERR_ERROR };
            DErrorCodeID ErrorCodeID;
            int ErrorCodeCUSTOM;
            int max_lenght=255;

            DErrorCode() {
                ErrorCodeID=ERR_NOT_SET;
                ErrorCodeCUSTOM=ERR_NOT_SET;
            }

            void SetError(std::string ErrorMsg, int CustomErrorCode = ERR_NOT_SET) {
                Set(ERR_ERROR,ErrorMsg,CustomErrorCode);
            }

            void SetWarning(std::string WarningMsg, int CustomErrorCode = ERR_NOT_SET) {
                Set(ERR_WARNING,WarningMsg,CustomErrorCode);
            }

            void Clear(void) {
                ErrorCodeID=ERR_NOT_SET;
                ErrorCodeCUSTOM=ERR_NOT_SET;
                ErrorStatus.clear();
                ErrorMessage.clear();
            }

            std::string Message(void) {
                return(ErrorMessage);
            }

            std::string StatusMessage(void) {
                return(ErrorStatus+" "+ErrorMessage);
            }

            bool IsSet(void) {
                return(ErrorCodeID != ERR_NOT_SET);
            }

            bool IsWarning(void) {
                return(ErrorCodeID == ERR_WARNING);
            }

            bool IsError(void) {
                return(ErrorCodeID == ERR_ERROR);
            }

#ifdef _WIN32
            /**
         * @brief Retrive last Windows system error text.
         * @return a string with decoded text.
         */
            std::string GetLastErrorText(void) {
                HMODULE hModule = NULL; // default to system source
                LPSTR MessageBuffer;
                std::string StrMessage;

                DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                      FORMAT_MESSAGE_IGNORE_INSERTS |
                                      FORMAT_MESSAGE_FROM_SYSTEM ;

                //
                // If dwLastError is in the network range,
                // load the message source.
                //
                DWORD dwLastError=GetLastError();
                if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR) {
                    hModule = LoadLibraryEx(
                        TEXT("netmsg.dll"),
                        NULL,
                        LOAD_LIBRARY_AS_DATAFILE
                        );

                    if(hModule != NULL)
                        dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
                }

                //
                // Call FormatMessage() to allow for message
                //  text to be acquired from the system
                //  or from the supplied module handle.
                //

                int mLen=FormatMessageA(
                    dwFormatFlags,
                    hModule, // module to get message from (NULL == system)
                    dwLastError,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
                    (LPSTR) &MessageBuffer,
                    0,
                    NULL
                    );
                if(mLen)
                {
                    StrMessage=MessageBuffer;
                    //
                    // Free the buffer allocated by the system.
                    //
                    LocalFree(MessageBuffer);
                }

                //
                // If we loaded a message source, unload it.
                //
                if(hModule != NULL)
                    FreeLibrary(hModule);

                return StrMessage;
            }
#endif

        private:
            void Set(DErrorCodeID ErrorCode, const std::string& ErrorMsg, int CustomErrorCode) {
                ErrorCodeID=ErrorCode;
                switch (ErrorCodeID) {
                    case ERR_SUCCESS:
                        ErrorStatus="SUCCESS";
                        break;
                    case ERR_WARNING:
                        ErrorStatus="WARNING";
                        break;
                    case ERR_ERROR:
                        ErrorStatus="ERROR";
                        break;
                    case ERR_NOT_SET:
                        ErrorStatus=std::string();
                        break;
                    default:
                        break;
                }
                ErrorMessage=ErrorMsg;
                ErrorCodeCUSTOM=CustomErrorCode;
            }

            std::string ErrorStatus;
            std::string ErrorMessage;
    };
}
}

#endif
