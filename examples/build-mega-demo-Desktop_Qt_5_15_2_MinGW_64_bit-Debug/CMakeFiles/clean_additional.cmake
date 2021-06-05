# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MegaDEMO_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MegaDEMO_autogen.dir\\ParseCache.txt"
  "MegaDEMO_autogen"
  "dep\\CMakeFiles\\DPPLIB_autogen.dir\\AutogenUsed.txt"
  "dep\\CMakeFiles\\DPPLIB_autogen.dir\\ParseCache.txt"
  "dep\\DPPLIB_autogen"
  )
endif()
