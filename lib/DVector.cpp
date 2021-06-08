#include "libdpp/DVector.h"

namespace DTools
{
namespace DVector
{
	//! Join all string vector values with a specific string
	/**
	* @param Vector		->  Vector to read
	* @param JoinStr    ->  String to join with
	* @return joined string like "value1 + joinStr + value2 + JoinStr +...."
	**/
	std::string JoinToStr(std::vector<std::string> &Vector, std::string JoinStr) {
			std::stringstream JoinedValues;
			for (std::string& Value: Vector) {
				JoinedValues << Value << JoinStr;
			}
		//Strip off the trailing comma
		return(JoinedValues.str().substr(0,JoinedValues.str().size()-1));
	}
}
}
