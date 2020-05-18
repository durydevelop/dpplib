//---------------------------------------------------------------------------
#ifndef DNetworkH
#define DNetworkH
//---------------------------------------------------------------------------
#include <string>

// Filesystem library include
#if __cplusplus > 201402L // C++17
	#include <filesystem>
	#include <asio.hpp>
#else
	#include <boost/filesystem.hpp>
	#include <boost/asio.hpp>
#endif

namespace DTools
{
	namespace DNet {
		// Filesystem library namespace
		#if __cplusplus > 201402L // C++17
			//#include <filesystem>
			namespace fs = std::filesystem;
			namespace err = std;
		#else
			//#include <boost/filesystem.hpp>
			namespace fs=boost::filesystem;
			namespace asio=boost::asio;
			namespace err = boost::system;
		#endif
		std::string GetHostName(void);
	}
}

#endif