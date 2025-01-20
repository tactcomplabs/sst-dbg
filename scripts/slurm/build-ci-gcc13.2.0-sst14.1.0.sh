#!/bin/bash
#
# scripts/slurm/build-ci-gcc13.2.0-sst14.1.0.sh
#
# Copyright (C) 2017-2025 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# This file is a part of the SST-DBG package.  For license
# information, see the LICENSE file in the top level directory of
# this distribution.
#
#
# Sample SLURM batch script
#
# Usage: sbatch -N1 build.sh
#
# This command requests 1 nodes for execution
#

#-- Stage 1: load the necessary modules
source /etc/qlustar/common/skel/bash/bashrc
module load sst/14.1.0
export CC=gcc
export CXX=g++

touch sstdbg.jenkins.${SLURM_JOB_ID}.out
sst --version >> sstdbg.jenkins.${SLURM_JOB_ID}.out 2>&1

#-- Stage 2: setup the build directories
mkdir -p build
cd build
rm -Rf ./*

#-- Stage 3: initiate the build
cmake -DBUILD_ALL_TESTING=ON ../ >> ../sstdbg.jenkins.${SLURM_JOB_ID}.out 2>&1
make clean >> ../sstdbg.jenkins.${SLURM_JOB_ID}.out 2>&1
make -j >> ../sstdbg.jenkins.${SLURM_JOB_ID}.out 2>&1

#-- Stage 4: test everything
make test >> ../sstdbg.jenkins.${SLURM_JOB_ID}.out 2>&1

# EOF
