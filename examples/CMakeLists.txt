################################################################################
#  EXAMPLES 
################################################################################

add_definitions     (${plast-flags})
include_directories (${plast-includes})

file (GLOB_RECURSE  PROGRAMS_FILES  *.cpp)

FOREACH (program ${PROGRAMS_FILES})

	# we get the base name of the current example
    get_filename_component(basename ${program} NAME_WE)

    # we add an executable for this example
  	add_executable (${basename} ${program})
  
    # we use the plast library for the link
  	target_link_libraries (${basename} ${plast-libraries})
  
    list (APPEND PROGRAMS_NAMES ${basename})
      
ENDFOREACH (program)

# We add a custom target for making all examples
add_custom_target (examples ALL DEPENDS ${PROGRAMS_NAMES})

