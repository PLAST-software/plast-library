
find_package (Doxygen)

if (DOXYGEN_FOUND)

    CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/doxygen/PlastLibrary.doxyfile  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile  @ONLY)
    
    ADD_CUSTOM_TARGET (doc-api ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
endif (DOXYGEN_FOUND)
