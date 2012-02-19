#! /bin/bash

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh
export SCRAM_ARCH=slc5_ia32_gcc434
eval `scramv1 runtime -sh`
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh
voms-proxy-init -voms cms


for QCDDir in $(cat QCDDirsList ) ; do 

    echo $QCDDir

###BEGIN TO BE CHANGED
    export NTupleVersion="V01-11-02"
    export DSName=$QCDDir
    export ListFileName=$QCDDir
    export mc_data=mc
###END TO BE CHANGED

    export SOURCEADD="srm://t3se01.psi.ch:8443/srm/managerv2?SFN="
    export DESTINATION="$CASTOR_HOME/ntp/$mc_data/$NTupleVersion"

    rfdir $DESTINATION &> /dev/null
    if  [ $? != 0 ]; then
	rfmkdir $DESTINATION
    fi
    export DESTINATION="$DESTINATION/$DSName/"

    export DESTINATIONPrefix="srm://srm-cms.cern.ch:8443"

    export lfn_pfn="pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/$mc_data"
    export lfn_pfn="$lfn_pfn/$NTupleVersion/$DSName/"
    echo "$SOURCEADD/$lfn_pfn"

    rfdir $DESTINATION &> /dev/null
#it returns 2 if the directory doesn't exist and 0 if it already exists
    if  [ $? == 0 ]; then
	echo $DESTINATION
	echo -n "The dirctory exists in your castor. Do you want to delete it first (you can [s]kip this step and then only new files will be copied)  ? [Y/n/s]"
	read -n 1 AreYouSure
	echo ""
	if [ $AreYouSure == "Y" ]; then
	    nsrm -rf $DESTINATION
	elif [ $AreYouSure == "s" ]; then
	    echo "Just new files will be copied."
	else
	    echo "You need to delete it manually"
	    exit 1
	fi
    else
	export AreYouSure="Y"
    fi

    if [ -f $ListFileName ]; then
	echo -n "The $ListFileName exists. Do you want to [d]elete it [u]se it? [d/u]"
	read -n 1 DelOrUse
	if [ $DelOrUse == "d" ]; then
	    rm $ListFileName
	    lcg-ls "$SOURCEADD/$lfn_pfn" > $ListFileName
	fi
    else
	lcg-ls "$SOURCEADD/$lfn_pfn" > $ListFileName
    fi

    if [ $AreYouSure == "Y" ]; then
	nsmkdir $DESTINATION
    fi
    nschmod 775 $DESTINATION

    for i in $(cat $ListFileName ) ; do 
	
	echo ""
	echo `basename $i`
	if [ $AreYouSure == "s" ]; then
	    rfdir "$DESTINATION/`basename $i`" &> /dev/null
	    if  [ $? == 0 ]; then
		echo "The file exists in castor"
		continue
	    fi
	fi
	lcg-cp -v "$SOURCEADD$i" "$DESTINATIONPrefix$DESTINATION/`basename $i`"
    done

done
