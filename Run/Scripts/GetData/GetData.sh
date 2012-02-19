export STARTUPDIR=`pwd`

cd /afs/cern.ch/user/h/hbakhshi/scratch0/CMSSW_3_8_2/src
export SCRAM_ARCH=slc5_ia32_gcc434
eval `scramv1 runtime -sh`

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh
eval `scramv1 runtime -sh`
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh

voms-proxy-init -voms cms -pwstdin < PASS

lcg-cp -v 'srm://t3se01.psi.ch:8443/srm/managerv2?SFN=/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-03-01/EG-Run2010A-PromptReco-v4-V01-03-01.root' file:///$STARTUPDIR/EG-Run2010A-PromptReco-v4-V01-03-01.root

rfcp $STARTUPDIR/EG-Run2010A-PromptReco-v4-V01-03-01.root /castor/cern.ch/user/h/hbakhshi/ETH/
