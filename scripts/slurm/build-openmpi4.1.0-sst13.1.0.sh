#!/bin/bash
#
# scripts/slurm/build-openmpi4.1.0-sst13.1.0.sh
#
# Copyright (C) 2017-2024 Tactical Computing Laboratories, LLC
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
source /etc/profile.d/modules.sh
module load cmake/3.23.0 sst/13.1.0 openmpi/gcc/64/4.1.0 llvm/12.0.0
export CC=mpicc
export CXX=mpicxx
export OMPI_CC=clang
export OMPI_CXX=clang++

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
