#!/bin/sh

set -x
set -e

cd $WIND_BASE/target/src 
make -k CPU=PENTIUM4 TOOL=gnu DUAL_BUILD=false COMMON_DIR=false

