#!/bin/csh
#
set CURRENDIR=`pwd`
set MACHINENAME=`uname -n`

if (`expr match $MACHINENAME ".*lxplus.*"`) set MACHINENAME=lxplus
if (`expr match $MACHINENAME "lxb.*"`) set MACHINENAME=lxplus

set ROOTBINDIR=`$PWD/../../external/${MACHINENAME}/root-config --bindir`
setenv ROOTSYS "`(cd ${ROOTBINDIR}/..;pwd)`"

set path = ($ROOTSYS/bin $path)

if ($?LD_LIBRARY_PATH) then
   setenv LD_LIBRARY_PATH $ROOTSYS/lib:$LD_LIBRARY_PATH
else
   setenv LD_LIBRARY_PATH $ROOTSYS/lib
endif

if ($?PYTHONPATH) then
   setenv PYTHONPATH $ROOTSYS/lib:$PYTHONPATH
else
   setenv PYTHONPATH $ROOTSYS/lib
endif

if ($?MANPATH) then
   setenv MANPATH `dirname $ROOTSYS/man/man1`:$MANPATH
else
   setenv MANPATH `dirname $ROOTSYS/man/man1`
endif

rehash
