project(PlastLibrary)

cmake_minimum_required (VERSION 2.6)

find_package (JNI REQUIRED)

#set (LIBRARY_OUTPUT_PATH    lib/${CMAKE_BUILD_TYPE})
#set (EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

################################################################################
# The version number.
################################################################################
set (plast_VERSION_MAJOR 2)
set (plast_VERSION_MINOR 2)
set (plast_VERSION_PATCH 0b)
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
    set (LIBRARY_COMPILE_DEFINITIONS  "${LIBRARY_COMPILE_DEFINITIONS} -O3 -funroll-loops  -fomit-frame-pointer  -msse3  -DOBSFUCATION  -DJNI_OBSFUCATION")
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

# We define the libraries used for linking binary based on the library
set (plast-libraries  PlastLibrary-static  dl  pthread)

# NOTE... we have to duplicate the variables for the other scopes (in particular for sub directories)
set (plast-flags     ${plast-flags}     PARENT_SCOPE)
set (plast-includes  ${plast-includes}  PARENT_SCOPE)
set (plast-libraries ${plast-libraries} PARENT_SCOPE)

################################################################################
#  LIBRARY GENERATION 
################################################################################
ADD_SUBDIRECTORY(src)

################################################################################
#  UNIT TESTS GENERATION 
################################################################################
ADD_SUBDIRECTORY(test)

################################################################################
#  DOCUMENTATION GENERATION 
################################################################################
ADD_SUBDIRECTORY (doc EXCLUDE_FROM_ALL)

################################################################################
#  EXAMPLES GENERATION 
################################################################################
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)

################################################################################
#  TOOLS GENERATION 
################################################################################
ADD_SUBDIRECTORY(tools)

################################################################################
#  INSTALLER 
################################################################################
SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY  "plast project")
SET (CPACK_PACKAGE_VENDOR               "INRIA/Korilog")
SET (CPACK_PACKAGE_VERSION_MAJOR        "${plast_VERSION_MAJOR}")
SET (CPACK_PACKAGE_VERSION_MINOR        "${plast_VERSION_MINOR}")
SET (CPACK_PACKAGE_VERSION_PATCH        "${plast_VERSION_PATCH}")
SET (CPACK_PACKAGE_VERSION              "${plast-version}")  

include (CPack)
