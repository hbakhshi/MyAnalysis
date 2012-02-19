#! /bin/csh

#SAMPLE : ./RunSkim.csh DiLeptonLowPTALL 1 Low

date +%s

setenv STARTUPDIR `pwd`
setenv INPUT $1 # "ElDSRunAReRec_DCap"  "ElDSRunBPRec_DCap" "MuDSRunBPRec_DCap" "MuDSRunAReRec_DCap"
setenv COPYTREEFILENAME "Tree"$3"_"$INPUT".root"
setenv MALLOC_CHECK_ 0
setenv CPUNumber $2
setenv LowPtHighPt $3

cd ../../Scripts
source Env.csh

cd $STARTUPDIR
#python -i Config.py
#SusySelection Skimmer.py
date +%s

FRCalculator FakeEstimator.py

date +%s
