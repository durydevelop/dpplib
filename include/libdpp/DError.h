#ifndef DErrorH
#define DErrorH

#include <string>

namespace DTools
{
namespace DError
{

    enum DErrorCodeID { SUCCESS, ERROR_WARNING, ERROR_GENERIC, ERROR_CUSTOM, ERROR_NOT_SET };

    class DErrorCode {

        public:
            DErrorCode() {
                ErrorCodeID=ERROR_NOT_SET;
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

            void SetCustom(std::string ErrorMsg) {
                Set(ERROR_CUSTOM,ErrorMsg);
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

        private:
            DErrorCodeID ErrorCodeID;
            std::string ErrorDesc;
    };

}
}

#endif
