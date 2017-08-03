#!/bin/bash
# Shell script for provisioning an Ubuntu 16.04 LTS slave on Inria cloudstack to compile GATB-CORE
# (for use with Jenkins, ci.inria.fr)

set -xv
set -e


yum -y update yum
yum clean all

# Install necessary packages
# --------------------------

yum -y --disablerepo=\* --enablerepo=base,update update cmake

yum -y --disablerepo=\* --enablerepo=base,update install flex
yum -y --disablerepo=\* --enablerepo=base,update install cppunit.x86_64
yum -y --disablerepo=\* --enablerepo=base,update install cppunit-devel.x86_64
yum -y --disablerepo=\* --enablerepo=base,update install graphviz


# Install Doxygen
# ---------------
# Note:  CentOS 6.3 comes with doxygen 1.6 (not compatible with Plast, needing 1.8).

cd /tmp
git clone https://github.com/doxygen/doxygen.git
cd doxygen
git checkout 0072ba56aa99420   # validated on Plast, as of 2017/08/03

# http://www.stack.nl/~dimitri/doxygen/manual/install.html
mkdir build
cd build
cmake -G "Unix Makefiles" ..

make

make install

# Display java version 
# --------------------
# as of 2017/08/03 : OpenJDK Runtime Environment (IcedTea6 1.11.4) (rhel-1.49.1.11.4.el6_3-x86_64)

java -version