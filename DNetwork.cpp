#include "DNetwork.h"

#if __cplusplus > 201402L // C++17
    #include <asio.hpp>

#else
    #include <boost/asio.hpp>
#endif

namespace DTools
{
        namespace DNetwork {
		//! @return Host Name corrente
		std::string GetHostName(void) {
			auto host_name = asio::ip::host_name();
			return(host_name);
		}
	}
}
