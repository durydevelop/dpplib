//---------------------------------------------------------------------------
#ifndef DCommonH
#define DCommonH
//---------------------------------------------------------------------------

// Filesystem library include
#if __cplusplus > 201402L // C++17
	#include <filesystem>
#else
	#include <boost/filesystem.hpp>
#endif

namespace DTools
{
    // Filesystem library namespace
	#if __cplusplus > 201402L // C++17
		namespace fs = std::filesystem;
		namespace err=std;
	#else
		namespace fs=boost::filesystem;
		namespace err=boost::system;
	#endif


}
#endif
