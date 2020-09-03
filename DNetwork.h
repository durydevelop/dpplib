//---------------------------------------------------------------------------
#ifndef DNetworkH
#define DNetworkH
//---------------------------------------------------------------------------
#include <string>

// Filesystem library include
#if __cplusplus > 201402L // C++17
    #if __GNUC__ >= 8
        #include <filesystem>
    #else
        #include <experimental/filesystem>
    #endif
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
            #if __GNUC__ >= 8
                namespace fs = std::filesystem;
            #else
                namespace fs = std::experimental::filesystem;
            #endif
            namespace err = std;
        #else
            namespace fs=boost::filesystem;
            namespace err = boost::system;
        #endif
		std::string GetHostName(void);
	}
}

#endif
