#!/bin/csh

set CURDIR=`pwd`
cd BOOSTLIB
set BSTDIR=`pwd`
cd -
source $BSTDIR/../etc/profile.d/init.csh

cd $CURDIR

cd $CURDIR/PYTHONLIB
set PTONLIB=`pwd`
cd -
source $PTONLIB/../etc/profile.d/init.csh

#setenv LD_LIBRARY_PATH `pwd`/PCRE:$LD_LIBRARY_PATH
