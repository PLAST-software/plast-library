################################################################################
#  UNIT TESTS GENERATION 
################################################################################
ADD_SUBDIRECTORY(unit)

################################################################################
#  BENCHMARK TESTS GENERATION 
################################################################################
ADD_SUBDIRECTORY(benchmark EXCLUDE_FROM_ALL)

# We add a custom target for delivery binaries
add_custom_target (test 

    COMMAND echo "================================================================"
    COMMAND echo ""
    COMMAND echo " Starting tests..."
    COMMAND echo ""
    COMMAND echo "================================================================"
    COMMAND echo "Running: ${PROJECT_BINARY_DIR}/test/runTest.sh"
    COMMAND ${PROJECT_BINARY_DIR}/test/runTest.sh 
)
