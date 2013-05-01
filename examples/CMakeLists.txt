################################################################################
#  EXAMPLES 
################################################################################

set (LIBRARY_OUTPUT_PATH    lib/${CMAKE_BUILD_TYPE})

list (APPEND PROGRAMS example1 example2 example3 example4 example5)
list (APPEND PROGRAMS example6 example7 example8 example9 example10)
list (APPEND PROGRAMS example11)

FOREACH (program ${PROGRAMS})
  add_executable(${program} ${program}.cpp)
  target_link_libraries(${program} PlastLibrary-static  dl pthread)
ENDFOREACH (program)
