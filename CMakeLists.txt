project(PlastLibrary)

cmake_minimum_required (VERSION 2.6)

find_package (JNI REQUIRED)

################################################################################
# The version number.
################################################################################
#
# DO NOT modify these numbers ; only used by Inria team to create official STABLE release
#
set (plast_VERSION_MAJOR 2)
set (plast_VERSION_MINOR 3)
set (plast_VERSION_PATCH "master-unstable")
set (plast-version ${plast_VERSION_MAJOR}.${plast_VERSION_MINOR}.${plast_VERSION_PATCH})
set (plast-date "xxxx-xx-xx")

################################################################################
# Current Date
################################################################################
INCLUDE(FindPerl)

# We execute a command that retrieves the current date.
IF (PERL_FOUND)
    EXECUTE_PROCESS (
        COMMAND "${PERL_EXECUTABLE}" "-le" "@T=localtime; printf (\"%04d-%02d-%02d %02d:%02d:%02d\",$T[5]+1900,$T[4]+1,$T[3],$T[2],$T[1],$T[0])"
        OUTPUT_VARIABLE plast-date
    )
ENDIF (PERL_FOUND)

################################################################################
# Compilation options
################################################################################

if (debug)
    set (LIBRARY_COMPILE_DEFINITIONS  "${LIBRARY_COMPILE_DEFINITIONS} -g -p")
    message("-- COMPILATION IN DEBUG MODE")
else()
    set (LIBRARY_COMPILE_DEFINITIONS  "${LIBRARY_COMPILE_DEFINITIONS} -O3 -funroll-loops  -fomit-frame-pointer  -msse3 ")
#  -DOBSFUCATION  -DJNI_OBSFUCATION
endif()

message("-- Options: ${LIBRARY_COMPILE_DEFINITIONS}")

################################################################################
# OPERATING SYSTEM SPECIFIC
################################################################################
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (LIBRARY_COMPILE_DEFINITIONS "${LIBRARY_COMPILE_DEFINITIONS} -D__DARWIN__")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (LIBRARY_COMPILE_DEFINITIONS "${LIBRARY_COMPILE_DEFINITIONS} -D__LINUX__")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (LIBRARY_COMPILE_DEFINITIONS "${LIBRARY_COMPILE_DEFINITIONS} -D__WINDOWS__")
    # IMPORTANT ! we have to add this flag for getting correct methods names in the library
    # otherwise the Java JVM won't find the native JNI methods.
    set_target_properties (PlastLibrary PROPERTIES LINK_FLAGS -Wl,--kill-at)
    set_target_properties (PlastLibrary PROPERTIES PREFIX "")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

################################################################################
#  DIRECTORIES MANAGEMENT
################################################################################

set (LIBRARY_OUTPUT_PATH       ${PROJECT_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set (EXECUTABLE_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

################################################################################
# Library variables
################################################################################

# We define the compilation flags used for compiling binary based on the library
set (plast-flags ${LIBRARY_COMPILE_DEFINITIONS})

# We define the include directories used for linking binary based on the library
set (plast-includes ${PROJECT_BINARY_DIR}/include)

# we set the log4cpp libame
set ( LOG4CPP_LIBRARY_NAME "log4cpp" )
# We define the libraries used for linking binary based on the library
set (plast-libraries  PlastLibrary-static  dl  pthread ${LOG4CPP_LIBRARY_NAME})

# NOTE... we have to duplicate the variables for the other scopes (in particular for sub directories)
set (plast-flags     ${plast-flags}     PARENT_SCOPE)
set (plast-includes  ${plast-includes}  PARENT_SCOPE)
set (plast-libraries ${plast-libraries} PARENT_SCOPE)

################################################################################
#  log4cpp LIBRARY
################################################################################
# we compile the library
ADD_SUBDIRECTORY(thirdparty/log4cpp)

# we add library inlude files to main include directory
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/log4cpp/src/
    DESTINATION ${PROJECT_BINARY_DIR}/include/log4cpp
    FILES_MATCHING PATTERN "*.hpp"  PATTERN "*.h" PATTERN "*.hh"
)
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/log4cpp/include/log4cpp
    DESTINATION ${PROJECT_BINARY_DIR}/include/
    FILES_MATCHING PATTERN "*.hpp"  PATTERN "*.h" PATTERN "*.hh"
)

################################################################################
#  LIBRARY GENERATION
################################################################################
ADD_SUBDIRECTORY(src)

################################################################################
#  UNIT TESTS GENERATION
################################################################################
# tests are now done on Inria Jenkins Platform and required additional tests
# data sets only available on that platform.
if (tests)
  ADD_SUBDIRECTORY(test)
endif()

################################################################################
#  TOOLS GENERATION
################################################################################
ADD_SUBDIRECTORY(tools)

################################################################################
#  DOCUMENTATION GENERATION
################################################################################
# call 'make doc-api' to compile doc
ADD_SUBDIRECTORY (doc EXCLUDE_FROM_ALL)

################################################################################
#  EXAMPLES GENERATION
################################################################################
# call 'make examples' to compile examples using PLAST c++ API
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)
