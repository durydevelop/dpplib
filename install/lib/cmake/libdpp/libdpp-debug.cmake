#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libdpp" for configuration "Debug"
set_property(TARGET libdpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libdpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liblibdpp.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libdpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libdpp "${_IMPORT_PREFIX}/lib/liblibdpp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
