
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(${CMAKE_CURRENT_LIST_DIR}/libdpp.cmake)

set(libdpp_INCLUDE_DIR   "E:/Programming/Lib/cpp/libdpp/install/include" )
set(libdpp_LIBRARY_DIR   "E:/Programming/Lib/cpp/libdpp/install/lib" )
set(libdpp_DATA_DIR   "E:/Programming/Lib/cpp/libdpp/install/" )
#...logic to determine installedPrefix from the own location...
set(libdpp_CONFIG_DIR  "${installedPrefix}/" )

check_required_components(libdpp)

message(">> libdpp found in ${libdpp_INCLUDE_DIR}")
