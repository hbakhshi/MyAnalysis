#! /bin/csh

date +%s

setenv STARTUPDIR `pwd`
setenv INPUT $1
setenv COPYTREEFILENAME "Tree"$3"_"$INPUT".root"
setenv MALLOC_CHECK_ 0
setenv CPUNumber $2

cd ../../Scripts
source Env.csh

cd $STARTUPDIR
#python -i Config.py
FRCalculator FakeEstimator.py


date +%s
