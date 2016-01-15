#!/bin/bash

#*****************************************************************************************
# Plast management script. Only for use from cmake script.
#
# Run Unit test for the PlastLibrary. 
# 
# Author: Patrick Durand, Inria
# Created: January 2016
#*****************************************************************************************

l_script_dir=$( cd -P -- "$(dirname -- "$(command -v -- "$0")")" && pwd -P )
data_dir="$l_script_dir/db"

if [ ! -e $data_dir ]; then
  mkdir $data_dir
  cd $data_dir
  echo "Retrieving test data from Inria Forge..."

  download=0

  if ! [ -x "$(command -v wget)" ]; then
    curl --silent -O http://plast.gforge.inria.fr/files/plast-test-data.tar.gz 
    download=1
  else
    wget --quiet http://plast.gforge.inria.fr/files/plast-test-data.tar.gz
    download=1
  fi

  if [ ! $download ]; then
    echo "  /!\ WGET nor CURL is installed: unable to continue."
    exit 1
  fi

  echo "Extracting test data..."
  gunzip plast-test-data.tar.gz
  tar -xf plast-test-data.tar
  rm plast-test-data.tar
  cd ..
fi

echo "Running unit tests..."
$l_script_dir/../bin/PlastLibraryTest $data_dir
