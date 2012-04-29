#!/bin/csh

set CURRENDIR=`pwd`
set MACHINENAME=`uname -n`

if (`expr match $MACHINENAME ".*lxplus.*"`) set MACHINENAME=lxplus
if (`expr match $MACHINENAME "lxb.*"`) set MACHINENAME=lxplus

echo $MACHINENAME

source PrepareRoot.csh

echo after root

cd $PWD

cd $PWD/../../external/${MACHINENAME}/

echo before env2

source Env.csh

echo after env2

cd $CURRENDIR


echo before python

setenv PYTHONPATH $PWD/../Configs:$PYTHONPATH

echo after python_path 

setenv LD_LIBRARY_PATH $PWD/../../bin/${MACHINENAME}/:$LD_LIBRARY_PATH

echo after ld path

setenv PATH $PWD/../../bin/${MACHINENAME}/:$PATH

echo after path

rehash

#echo $PYTHONPATH
#echo $LD_LIBRARY_PATH
