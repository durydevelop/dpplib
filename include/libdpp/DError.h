#ifndef DErrorH
#define DErrorH

#include <string>

namespace DTools
{
namespace DError
{

enum DErrorCodeID { SUCCESS, CUSTOM_ERROR, NOT_SET };

    class DErrorCode {

        public:
        DErrorCode() {
            ErrorCodeID=NOT_SET;
        }

        DErrorCodeID ErrorCodeID;
        std::string ErrorDesc;
    };

}
}

#endif
