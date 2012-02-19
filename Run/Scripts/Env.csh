#!/bin/csh

set CURRENDIR=`pwd`
set MACHINENAME=`uname -n`

if (`expr match $MACHINENAME ".*lxplus.*"`) set MACHINENAME=lxplus
if (`expr match $MACHINENAME "lxb.*"`) set MACHINENAME=lxplus

echo $MACHINENAME

source PrepareRoot.csh

cd $PWD

cd $PWD/../../external/${MACHINENAME}/
source Env.csh
cd $CURRENDIR

setenv PYTHONPATH $PWD/../Configs:$PYTHONPATH
setenv LD_LIBRARY_PATH $PWD/../../bin/${MACHINENAME}/:$LD_LIBRARY_PATH
setenv PATH $PWD/../../bin/${MACHINENAME}/:$PATH

rehash

#echo $PYTHONPATH
#echo $LD_LIBRARY_PATH
