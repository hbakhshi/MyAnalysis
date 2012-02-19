#! /bin/bash

export DESTINATION=$CASTOR_HOME/ETH/EG-Run2010A-PromptReco-v4_RECO 

echo "" > CastorFileList
nsls $DESTINATION >> CastorFileList

echo "" > ServerFileList

for i in $(cat FileList ) ; do 
    
    echo ""
    echo `basename $i` >> ServerFileList_

done

sort ServerFileList_ > ServerFileList
rm ServerFileList_

diff ServerFileList CastorFileList
