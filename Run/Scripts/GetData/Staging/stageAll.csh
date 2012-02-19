rm -rf ETHList
nsls ETH > ETHList
./stageListOfFiles.sh ETHList -p $CASTOR_HOME/ETH/
