#!/bin/csh -f

setenv PATH "/home/hbakhshi/bin:/home/hbakhshi/scripts:/usr/sue/bin:/bin:/usr/local/bin:/usr/local/bin/X11:/usr/bin:/usr/bin/X11"
setenv LD_LIBRARY_PATH ""
cd $3
setenv STARTUPDIR `pwd`
setenv INPUT $1
setenv COPYTREEFILENAME "TreeTTbar_"$INPUT".root"
setenv MALLOC_CHECK_ 0
setenv CPUNumber $2

cd ../Scripts
source Env.csh

cd $STARTUPDIR
#python -i Config.py
rm -f Log$INPUT
rm -f Err$INPUT
(WPolarization Config.py > Log$INPUT) >& Err$INPUT

