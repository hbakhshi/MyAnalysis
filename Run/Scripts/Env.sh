#!/bin/sh

export MACHINENAME=`uname -n`
export PYTHONPATH=$PWD/../Configs:$PYTHONPATH
export LD_LIBRARY_PATH=$PWD/../../external/${MACHINENAME}/PYTHONLIB/:$PWD/../../external/${MACHINENAME}/BOOSTLIB:$PWD/../../bin/$MACHINENAME/:$LD_LIBRARY_PATH
export PATH=$PWD/../../bin/${MACHINENAME}/:$PATH

export ROOTBINDIR=`$PWD/../../external/$MACHINENAME/root-config --bindir`
source $ROOTBINDIR/thisroot.sh

#echo $PYTHONPATH
#echo $LD_LIBRARY_PATH