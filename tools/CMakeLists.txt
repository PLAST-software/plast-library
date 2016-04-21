################################################################################
#  TOOLS
################################################################################

add_definitions     (${plast-flags})
include_directories (${plast-includes})

list (APPEND PROGRAMS PlastCmd makeplastdb)

FOREACH (program ${PROGRAMS})
  add_executable(${program} ${program}.cpp)
  target_link_libraries(${program} ${plast-libraries} ${LOG4CPP_LIBRARY_NAME})
ENDFOREACH (program)

################################################################################
#  INSTALLATION
################################################################################
install (TARGETS PlastCmd makeplastdb DESTINATION bin)
