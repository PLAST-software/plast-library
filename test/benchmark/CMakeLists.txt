################################################################################
#  BENCHMARKS 
################################################################################

add_definitions     (${plast-flags})
include_directories (${plast-includes})

list (APPEND PROGRAMS bench1 AlgoAnalysis1)

FOREACH (program ${PROGRAMS})
  add_executable(${program} ${program}.cpp)
  target_link_libraries(${program} ${plast-libraries})
ENDFOREACH (program)
