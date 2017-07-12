#!/bin/bash

#########################################################################################
#
# Companion file to be used with Dockerfile.alpine-compiler
#
# This script is embedded within the container: it is responsible for compiling
# PLAST tool.
#
#########################################################################################
#
# A shell script to compile PLAST using Alpine system: c/c++ libs and compiler.
#
# Use: build.sh "name1;name2;name3;version"
#      (see below for a description of this single string-based argument)
#
# It works as follows:
#  a. get a PLAST Source bundle from Github/PLAST-software/plast-library
#  b. compile the source within an Alpine Docker Container
#  c. make the binary bundle for 'Alpine'
#  d. run PLAST tool test
#  e. archive the Alpine Binary bundle
#
# Declaration of a PLAST description is done using a single string-based argument
# within double quotes. This argument is formated as:
#
#  "name1;name2;name3;version"
#
#  where:
#   [0]:name1:       PLAST github project name 
#   [1]:name2:       PLAST name of the binary tarball
#   [2]:name3:       PLAST name of the source tarball
#   [3]:version:     PLAST version to retrieve and compile (without prefix 'v')
#
#   Caution: - ';' is the field separator
#            - do not add space chars in between fields and ';'
#            - do not forget to enclose string within " (otherwise cmd-line won't work)
#
#
# Author: Patrick G. Durand, Inria, July 2017
#
#########################################################################################

# Here is ready-to-use description to compile PLAST from its official release:
#
# "plast-library;plastbinary;plast_source;2.3.2"
#
# Simple use:
#   
#   docker run --rm -i -t -v $PWD:/tmp plast_alpine_compiler "plast-library;plastbinary;plast_source;2.3.2"
#
#   where '$PWD' is the place where Alpine Binary bundle is created on YOUR
#   local directory.
#

# ##  DECLARATIONS  #####################################################################

# 'Make' command can use these many cores
CORES=4
# This is the working directory INSIDE the container. You can map it 
# outside the container using docker -v argument
WK_DIR=/tmp
# get cmd-line argument in an appropriate variable: the GATB-Tool to process
PLAST_TOOL_DESCRIPTION=$1
# Sample string format (only for help message)
WKDIR_HELP="/path/to/wkdir"
CMD_HELP="docker run --rm -it -v $WKDIR_HELP:/tmp plast_compiler"
DESC_HELP="plast-library;plastbinary;plastbinary;2.3.2"
RES_HELP="docker execution results will be located in $WKDIR_HELP: an Alpine binary bundle."
# In case of error, this script will return a dedicated code !=0
NB_ARGS_ERROR=1
ARGS_ERROR=2
CMAKE_ERROR=3
MAKE_ERROR=4
TEST_ERROR_CODE=5
CD_SRC_DIR_ERROR=6
CURL_SRC_ERROR=7
LINUX_BIN_ERROR=9
CP1_BIN_ERROR=10
CP2_BIN_ERROR=11
CP3_BIN_ERROR=12
PLAST_EXEC_ERROR=13
PLAST2_EXEC_ERROR=13

# Figure out whether or not this script get PLAST official archives
# (source and binary) from Github. Set to zero if using Docker container
# on INRIA-CI platform: access to the web is not allowed.
DO_CURL=1
# set a dedicated time format
TIMEFORMAT='      Time - real:%3lR | user:%3lU | sys:%3lS'

# ##  MAIN  #############################################################################

# Check argument
if [ "$#" -ne 1 ]; then
    echo "ERROR: missing PLAST description string."
    echo "  sample: $CMD_HELP \"$DESC_HELP\""
    echo "  $RES_HELP"
    exit $NB_ARGS_ERROR
fi

nb_strings=$(grep -o ";" <<< "$1" | wc -l)
if [ "$nb_strings" -ne 3 ]; then
    echo "ERROR: bad format for description string"
    echo "  sample: $CMD_HELP \"$DESC_HELP\""
    echo "  $RES_HELP"
    exit $ARGS_ERROR
fi

# == STEP 1: get source code if requested. This step is optional
# since using this script within Docker containers on Inria CI
# platform prevents any remote connection to the Internet.
# Be sure we are in the appropriate directory
cd ${WK_DIR}
# get plast-tool fields: names and release nb.
arr=(${PLAST_TOOL_DESCRIPTION//;/ })
FNAME=${arr[0]}_v${arr[3]}
echo "##  Making: ${FNAME}"
echo ""
# Prepare the curl command
if [ "$DO_CURL" -eq "1" ]; then
  TOOLTGZ=${arr[2]}_v${arr[3]}.tar.gz
  GIT_URL=https://github.com/PLAST-software/${arr[0]}/releases/download
  TOOLURL=${GIT_URL}/v${arr[3]}/${TOOLTGZ}
  echo "getting source from: ${TOOLURL} ..."
  time curl -ksL ${TOOLURL} | tar xz
  if [ ! $? -eq 0 ]; then
      echo "    FAILED"
      exit $CURL_SRC_ERROR
  fi
  echo "      OK"
fi


# == STEP 2: compile source code
# Prepare the cmake/make call
SOURCE_BASE_NAME=${arr[2]}_v${arr[3]}
cd ${SOURCE_BASE_NAME}
if [ ! $? -eq 0 ]; then
    echo "    cd ${SOURCE_BASE_NAME}: FAILED"
    exit $CD_SRC_DIR_ERROR
fi
mkdir -p build
cd build
echo "running CMake in: $PWD ..."
time cmake .. > ${WK_DIR}/${FNAME}-CMake.log 2>&1 
if [ ! $? -eq 0 ]; then
    echo "    CMake: FAILED."
    echo "     See: ${WK_DIR}/${FNAME}-CMake.log"
    exit $CMAKE_ERROR
fi 
echo "      OK"

echo "running make ..."
time make -j${CORES} > ${WK_DIR}/${FNAME}-make.log 2>&1 
if [ ! $? -eq 0 ]; then
    echo "    make: FAILED"
    echo "     See: ${WK_DIR}/${FNAME}-make.log"
    exit $MAKE_ERROR
fi
echo "      OK"

# == STEP 3: prepare Alpine binary bundle
# update that archive: replace bin programs with Alpine ones
cd ${WK_DIR}
echo "preparing Alpine binary bundle ..."
BIN_BASE_NAME=${arr[1]}_v${arr[3]}-Alpine
# remove any other old build directory
[ -e ${BIN_BASE_NAME} ] && rm -rf ${BIN_BASE_NAME}
mkdir -p ${BIN_BASE_NAME}
if [ ! -d ${BIN_BASE_NAME} ]; then
    echo "    FAILED: unable to locate: ${BIN_BASE_NAME}"
    exit $LINUX_BIN_ERROR
fi
cd ${BIN_BASE_NAME}
cp ${WK_DIR}/${arr[2]}_v${arr[3]}/build/bin/plast .
if [ ! $? -eq 0 ]; then
    echo "    FAILED: unable to copy ${arr[0]} Alpine binaries"
    exit $CP1_BIN_ERROR
fi
mkdir db
cp ${WK_DIR}/${arr[2]}_v${arr[3]}/db/query.fa ./db
if [ ! $? -eq 0 ]; then
    echo "    FAILED: unable to copy db sample files"
    exit $CP2_BIN_ERROR
fi
cp ${WK_DIR}/${arr[2]}_v${arr[3]}/db/tursiops.fa ./db
if [ ! $? -eq 0 ]; then
    echo "    FAILED: unable to copy db sample files"
    exit $CP3_BIN_ERROR
fi
echo "      OK"

# == STEP 4: test PLAST
echo "testing PLAST binary bundle ..."
export PLAST_RES=plast.res
./plast -p plastp -i ./db/query.fa -d ./db/tursiops.fa -o $PLAST_RES -bargraph 
if [ ! $? -eq 0 ]; then
    echo "    FAILED: unable to run PLAST"
    exit $PLAST_EXEC_ERROR
fi

if [ ! -e "$PLAST_RES" ]; then
    echo "    FAILED: unable to locate PLAST result file: $PLAST_RES"
    exit $PLAST2_EXEC_ERROR
fi
rm -f $PLAST_RES ./db/tursiops.fa
echo "      OK"

# == STEP 5: package Alpine binary bundle
TAR_BAL=${BIN_BASE_NAME}.tar.gz
echo "archiving Alpine bundle in: ${TAR_BAL} ..."
cd ${WK_DIR}
# remove any 'old' tarball
[ -e ${TAR_BAL} ] && rm -f ${TAR_BAL}
# package the Alpine bundle 
tar -cf ${BIN_BASE_NAME}.tar ${BIN_BASE_NAME}
gzip ${BIN_BASE_NAME}.tar
# made some additional cleanup
rm -rf ${SOURCE_BASE_NAME} ${BIN_BASE_NAME}
echo "      OK"

exit 0
