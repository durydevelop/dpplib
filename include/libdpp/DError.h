#ifndef DErrorH
#define DErrorH

#include <string>

namespace DTools
{
namespace DError
{

    enum DErrorCodeID { ERROR_NOT_SET = -1, SUCCESS, ERROR_WARNING, ERROR_GENERIC, ERROR_CUSTOM };

    class DErrorCode {

        public:
            DErrorCodeID ErrorCodeID;
            int ErrorCodeCUSTOM;

            DErrorCode() {
                ErrorCodeID=ERROR_NOT_SET;
                ErrorCodeCUSTOM=ERROR_NOT_SET;
            }

            void Set(DErrorCodeID ErrorCode, const std::string& ErrorMsg = std::string()) {
                ErrorCodeID=ErrorCode;
                switch (ErrorCodeID) {
                    case SUCCESS:
                        ErrorDesc="Success";
                        break;
                    case ERROR_WARNING:
                        ErrorDesc=ErrorMsg;
                        break;
                    case ERROR_GENERIC:
                        ErrorDesc="Error";
                        break;
                    case ERROR_NOT_SET:
                        ErrorDesc=std::string();
                        break;
                    case ERROR_CUSTOM:
                        ErrorDesc=ErrorMsg;
                        break;
                    default:
                        break;
                }
            }

            void SetCustom(std::string CustomErrorMsg, int CustomErrorCode) {
                Set(ERROR_CUSTOM,CustomErrorMsg);
                ErrorCodeCUSTOM=CustomErrorCode;
            }

            void SetWarning(std::string WarningMsg) {
                Set(ERROR_WARNING,WarningMsg);
            }

            std::string Message(void) {
                return(ErrorDesc);
            }

            bool IsSet(void) {
                return(ErrorCodeID != ERROR_NOT_SET);
            }

            bool IsWarning(void) {
                return(ErrorCodeID == ERROR_WARNING);
            }

        private:
            std::string ErrorDesc;
    };

}
}

#endif
