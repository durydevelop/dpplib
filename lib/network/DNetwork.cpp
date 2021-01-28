#include "libdpp/network/DNetwork.h"

#if __cplusplus > 201402L // C++17
    #include <boost/asio.hpp>
    namespace asio=boost::asio;

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
/*
		std::string EnumarateIP() {
			asio::io_service io_service;
			asio::tcp::resolver resolver(io_service);
			asio::tcp::resolver::query query(boost::asio::ip::host_name(),"");
			asiotcp::resolver::iterator it=resolver.resolve(query);

			while(it!=tcp::resolver::iterator())
			{
				boost::asio::ip::address addr=(it++)->endpoint().address();
				if(addr.is_v6())
				{
					std::cout<<"ipv6 address: ";
				}
				else
					std::cout<<"ipv4 address: ";

				std::cout<<addr.to_string()<<std::endl;

			}
		}
*/
	}
}
