#!/bin/csh
# 
set dt=`date +%Y%m%d%H%M%S`
mkdir tmp/Analysis$dt -p
cd tmp/Analysis$dt
tar -xjf ../../Analysis.bz2.tar

#set VO_CMS_SW_DIR=/afs/cern.ch/cms/

set MACHINENAME=`uname -n`
if (`expr match $MACHINENAME ".*lxplus.*"`) set MACHINENAME=lxplus
if (`expr match $MACHINENAME "lxb.*"`) set MACHINENAME=lxplus

set SCRAM_ARCH=slc5_ia32_gcc434
set CMSSWDIR=$VO_CMS_SW_DIR/$SCRAM_ARCH/

mkdir external/$MACHINENAME -p
cd external/$MACHINENAME
ln -s $CMSSWDIR/external/boost/1.42.0/lib/ BOOSTLIB
ln -s $CMSSWDIR/external/boost/1.42.0/include/ BOOSTINCLUDE
ln -s $CMSSWDIR/external/python/2.6.4-cms6/lib/ PYTHONLIB
ln -s $CMSSWDIR/external/python/2.6.4-cms6/include/python2.6/ PYTHONINCLUDE
ln -s $CMSSWDIR/external/pcre/4.4/lib/ PCRE
ln -s $CMSSWDIR/lcg/root/5.22.00d-cms19/bin/root-config
ln -s ../../CMSSW/ CMSSWREALEASEBASE
ln -s ../../CMSSW/CLHEPINCLUDE CLHEPINCLUDE

cp ../../Run/Scripts/MachineEnv_Template.csh ./Env.csh

cd -
mkdir bin/$MACHINENAME -p
mkdir obj/$MACHINENAME -p

cd $CMSSWDIR/external/gcc/4.3.4/etc/profile.d
source init.csh

cd -
cd Run/Scripts/
source Env.csh

cd ../../
make all -j4
