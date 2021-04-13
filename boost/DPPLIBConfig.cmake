set(DPPLIB_VERSION 0.2.1.2)


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

set_and_check(DPPLIB_INCLUDE_DIR   "/usr/local/include" )
set_and_check(DPPLIB_LIBRARY_DIR   "/usr/local/lib" )
set_and_check(DPPLIB_DATA_DIR   "/usr/local/" )

check_required_components(DPPLIB)

#message("@@ DPPLIB found in ${DPPLIB_DATA_DIR}")
#message("@@ DPPLIB includes: ${DPPLIB_INCLUDE_DIR}")
#message("@@ DPPLIB libs: ${DPPLIB_LIBRARY_DIR}")

#set_and_check(DPPLIB_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include")
#set_and_check(DPPLIB_SYSCONFIG_DIR "${PACKAGE_PREFIX_DIR}/etc/DPPLIB")
