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

file (GLOB_RECURSE  PlastLibraryTestFiles  src/*)

add_definitions     (${plast-flags})
include_directories (${plast-includes} ${PlastLibrary_SOURCE_DIR}/test/unit/src  ${CPPUNIT_INCLUDE_DIR})

add_executable        (PlastLibraryTest    ${PlastLibraryTestFiles})
target_link_libraries (PlastLibraryTest    ${plast-libraries} ${CPPUNIT_LIBRARY})

################################################################################
#  INSTALLATION 
################################################################################
install (TARGETS PlastLibraryTest  DESTINATION bin)
