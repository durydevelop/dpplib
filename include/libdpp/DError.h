#ifndef DErrorH
#define DErrorH

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
