#!/bin/sh

rm -rf ETHList1
nsls ntp/data/V01-05-01/Mu-Run2010A-Sep17ReReco_v2_RECO/ > ETHList1
#./stageListOfFiles.sh ETHList1 -p $CASTOR_HOME/ntp/data/V01-05-01/Mu-Run2010A-Sep17ReReco_v2_RECO/

rm -rf ETHList2
nsls ntp/data/V01-05-00/EG-Run2010A-Sep17ReReco_v2_RECO/ > ETHList2
#./stageListOfFiles.sh ETHList2 -p $CASTOR_HOME/ntp/data/V01-05-00/EG-Run2010A-Sep17ReReco_v2_RECO/

rm -rf ETHList3
nsls ntp/data/V01-06-00/Electron-Run2010B-PromptReco-v2_RECO/ > ETHList3
./stageListOfFiles.sh ETHList3 -p $CASTOR_HOME/ntp/data/V01-06-00/Electron-Run2010B-PromptReco-v2_RECO/

rm -rf ETHList4
nsls ntp/data/V01-06-00/Mu-Run2010B-PromptReco-v2_RECO/ > ETHList4
#./stageListOfFiles.sh ETHList4 -p $CASTOR_HOME/ntp/data/V01-06-00/Mu-Run2010B-PromptReco-v2_RECO/

