#########################################################################################
#
#  Docker file for PLAST project.
#
#  It prepares a Docker container to run PLAST jobs.
#
#########################################################################################
#
# == Docker build command
#
#    docker build -f Dockerfile -t plast_machine .
#
# == Docker test command
#
#    docker run --rm -i -t plast_machine -h
#
#    -> you should see the PLAST help message.
#
# == Running a PLAST job
#
#    docker run --rm -i -t plast_machine <args>
#
#    where <args> are the PLAST command-line arguments.
#    See https://plast.inria.fr/user-guide/plast-command-line-arguments/
#
# == Sample PLAST job with provided data
#
#    docker run --rm -i -t -v $PWD:/tmp plast_machine -p plastp -i /opt/plastbinary/db/query.fa -d /opt/plastbinary/db/tursiops.fa -o /tmp/test.out -a 4
#
#    -> you should have a 'test.out' file when PLAST job is done.
#
#    This command-line line explained:
#
#    docker run                            [1]
#       --rm                               [2]
#       -i -t                              [3]
#       -v $PWD:/tmp                       [4]
#       plast_machine                      [5] 
#       -p plastp                          [6]
#       -i /opt/plastbinary/db/query.fa    [7] 
#       -d /opt/plastbinary/db/tursiops.fa [8]
#       -o /tmp/test.out                   [9]
#       -a 4                               [10]
#
#       [1]-[5]: Docker arguments
#       [6]-[10]: PLAST arguments
#
#       [1]: start Docker container
#       [2]: destroy container when Docker finishes
#            (it does NOT delete the 'plast_machine' image)
#       [3]: start an interactive job 
#            (for instance, you'll see messages on stdout, if any)
#       [4]: mount a volume. This is required to get the results from PLAST.
#            Here, we say that current local directory will be viewed as '/tmp'
#            from the inside of the container. 
#       [5]: tell Docker which image to start: the 'plast_machine' of course.
#       [6]: run a plastp job, i.e. protein query vs. protein reference bank
#       [7]: the query; this file is provided inside the container.
#       [8]: the reference bank; this file is provided inside the container.
#       [9]: the result file. 
#            Created within the '/tmp' directory inside the container, this file
#            will be available in the current local directory, thanks to argument [4].
#       [10]: request to use up to 4 threads
#            (adapt the value to your computer!)
#
#########################################################################################

# ###
#    PLAST official release is compiled using INRIA's Continuous 
#    Integration platform on a CentOS 6 system.
#
FROM centos:6.6

# who to blame?
MAINTAINER Patrick Durand patrick.durand@inria.fr

# ###
#    We always use the latest official PLAST release.
#
ENV PLAST_VERSION=2.3.2
    
# ###
#     Package installation and configuration
#
RUN yum install -y curl tar

# ###
#     PLAST installation: get the release from Github mirror.
#
RUN cd /opt \
    && export PLAST_TGZ=plastbinary_linux_v${PLAST_VERSION}.tar.gz \
    && export GIT_URL=https://github.com/PLAST-software/plast-library/releases/download \
    && export PLAST_URL=${GIT_URL}/v${PLAST_VERSION}/${PLAST_TGZ} \
    && curl -ksL ${PLAST_URL} | tar xz \
    && rm -f ${PLAST_TGZ} \
    && mv plastbinary_linux_v${PLAST_VERSION} plastbinary

# ###
#
# Start plast
#
ENTRYPOINT ["/opt/plastbinary/build/bin/plast"]
