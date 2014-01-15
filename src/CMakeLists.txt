################################################################################
#  HEADERS COPY
################################################################################
file (COPY ${PROJECT_SOURCE_DIR}/src/
    DESTINATION ${PROJECT_BINARY_DIR}/include
    FILES_MATCHING PATTERN "*.hpp"  PATTERN "*.h"  PATTERN "*.pri"
)

install (DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.pri")

################################################################################
#  CONFIGURATION FILE
################################################################################
configure_file (
    ${PROJECT_SOURCE_DIR}/src/misc/api/version.hpp.in 
    ${PROJECT_BINARY_DIR}/include/misc/api/version.hpp
)

install (FILES ${PROJECT_BINARY_DIR}/include/misc/api/version.hpp DESTINATION include/os/api/)

################################################################################
#  LIBRARY GENERATION 
################################################################################

# We add the compilation options for the library
add_definitions (${plast-flags})

file (GLOB_RECURSE  LibraryFiles  ./*)

include_directories (${plast-includes}  ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})

add_library    (PlastLibrary-static  STATIC  ${LibraryFiles} )
#add_library    (PlastLibrary-dynamic SHARED  ${LibraryFiles} )

set_target_properties (PlastLibrary-static   PROPERTIES OUTPUT_NAME  PlastLibrary  clean_direct_output 1)
#set_target_properties (PlastLibrary-dynamic  PROPERTIES OUTPUT_NAME  PlastLibrary  clean_direct_output 1)


################################################################################
#  INSTALLATION 
################################################################################

# We install the libraries
install (TARGETS PlastLibrary-static DESTINATION lib)
