project(PlastLibrary)

cmake_minimum_required (VERSION 2.6)

find_package (JNI REQUIRED)

#set (LIBRARY_OUTPUT_PATH    lib/${CMAKE_BUILD_TYPE})
#set (EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

################################################################################
# GENERAL DEFINITIONS
################################################################################
add_definitions (-O3 -funroll-loops  -fomit-frame-pointer  -msse3  -DOBSFUCATION  -DJNI_OBSFUCATION)

################################################################################
# OPERATING SYSTEM SPECIFIC
################################################################################
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-D__DARWIN__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-D__LINUX__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-D__WINDOWS__)
    # IMPORTANT ! we have to add this flag for getting correct methods names in the library    
    # otherwise the Java JVM won't find the native JNI methods.
    set_target_properties (PlastLibrary PROPERTIES LINK_FLAGS -Wl,--kill-at)
    set_target_properties (PlastLibrary PROPERTIES PREFIX "")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

################################################################################
#  LIBRARY GENERATION 
################################################################################

set (LIBRARY_OUTPUT_PATH    lib/${CMAKE_BUILD_TYPE})

file (GLOB_RECURSE  LibraryFiles  src/*)

include_directories (src  ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})

add_library    (PlastLibrary-static  STATIC  ${LibraryFiles} )
add_library    (PlastLibrary-dynamic SHARED  ${LibraryFiles} )

set_target_properties (PlastLibrary-static   PROPERTIES OUTPUT_NAME  PlastLibrary  clean_direct_output 1)
set_target_properties (PlastLibrary-dynamic  PROPERTIES OUTPUT_NAME  PlastLibrary  clean_direct_output 1)

################################################################################
#  UNIT TESTS GENERATION 
################################################################################
ADD_SUBDIRECTORY(test)

################################################################################
#  DOCUMENTATION GENERATION 
################################################################################
ADD_SUBDIRECTORY(doc)

################################################################################
#  EXAMPLES GENERATION 
################################################################################
ADD_SUBDIRECTORY(examples)

################################################################################
#  TOOLS GENERATION 
################################################################################
ADD_SUBDIRECTORY(tools)
