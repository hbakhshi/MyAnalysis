#! /bin/csh

setenv STARTUPDIR `pwd`
setenv INPUT $1 # "ElDSRunAReRec_DCap"  "ElDSRunBPRec_DCap" "MuDSRunBPRec_DCap" "MuDSRunAReRec_DCap"
setenv COPYTREEFILENAME "TreeSS_"$INPUT".root"
setenv MALLOC_CHECK_ 0
setenv CPUNumber $2

cd ../../Scripts
source Env.csh

cd $STARTUPDIR
#python -i Config.py
Pmm Config.py
