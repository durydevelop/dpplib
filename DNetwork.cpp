#include "DNetwork.h"

namespace DTools
{
	namespace DNet {
		//! @return Host Name corrente
		std::string GetHostName(void) {
			auto host_name = asio::ip::host_name();
			return(host_name);
		}
	}
}