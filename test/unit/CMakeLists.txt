################################################################################
#  CPPUNIT 
################################################################################
FIND_PATH (CPPUNIT_INCLUDE_DIR cppunit/extensions/HelperMacros.h
  /local/users/edrezen/local/include
  /usr/local/include
)

FIND_LIBRARY (CPPUNIT_LIBRARY cppunit
    ${CPPUNIT_INCLUDE_DIR}/../lib
)

IF (CPPUNIT_INCLUDE_DIR)
    IF (CPPUNIT_LIBRARY)
        SET (CPPUNIT_FOUND "YES")
        SET (CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY})
    ENDIF (CPPUNIT_LIBRARY)
ENDIF (CPPUNIT_INCLUDE_DIR)

################################################################################
#  COMPONENT 
################################################################################

set (LIBRARY_OUTPUT_PATH    lib/${CMAKE_BUILD_TYPE})

file (GLOB_RECURSE  PlastLibraryTestFiles  src/*)

include_directories (${PlastLibrary_SOURCE_DIR}/src  ${PlastLibrary_SOURCE_DIR}/test/unit/src  ${CPPUNIT_INCLUDE_DIR})

add_executable        (PlastLibraryTest    ${PlastLibraryTestFiles})
target_link_libraries (PlastLibraryTest     PlastLibrary-static  ${CPPUNIT_LIBRARY}  dl pthread)

