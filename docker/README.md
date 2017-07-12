# Introduction

This directory contains Docker material aims at creating a PLAST Docker container.

# CentOS container

Genscale Team uses a CentOS 6 Linux system to compile and to test the PLAST software.

As a consequence, we first designed a Dockerfile relying on that OS. 

You simply create the Docker image as follows:

    docker build -f Dockerfile -t plast_machine .

Image size is 230 Mb.

Then to run PLAST jobs using that container, refer to the header of Dockerfile.

# Alpine Linux container

In order to provide a more compact version of the PLAST Docker container, we designed another Dockerfile to use the compact Alpine Linux system.

For that purpose, we actually made two Dockerfiles:

* Dockerfile.alpine-compiler: use to compile PLAST source code into Alpine-based binary
* Dockerfile.alpine: use to package the previous binary into a ready-to-use PLAST job machine

So, you create the PLAST machine as follows:

    docker build -f Dockerfile.alpine-compiler -t plast_alpine_compiler .
    docker run --rm -i -t -v $PWD:/tmp plast_alpine_compiler \
            "plast-library;plastbinary;plast_source;2.3.2"
    docker build -f Dockerfile.alpine -t plast_alpine_machine .

You'll end up with a very, very compact PLAST machine sizing only 9.6 Mb!

To use that Docker container, please refer to the header of Dockerfile.alpine

 
