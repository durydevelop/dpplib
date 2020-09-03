#ifndef DFilesystem_H
#define DFilesystem_H

// Filesystem library include
#if __cplusplus > 201402L // C++17
    #if __GNUC__ >= 8
        #include <filesystem>
    #else
        #include <experimental/filesystem>
    #endif
#else
    #include <boost/filesystem.hpp>
#endif

namespace DTools
{
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
}
#endif
