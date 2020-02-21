#include "DNetwork.hpp"
namespace DTools
{
	namespace DNet {
		//! @return Host Name corrente
		std::string GetHostName(void) {
			auto host_name = boost::asio::ip::host_name();
			return(host_name);
		}
	}
}