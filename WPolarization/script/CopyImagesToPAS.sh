#! /bin/bash
export PASFIGPATH=/home/hbakhshi/Documents/WPolarization/Note/notes/AN-12-157/trunk/plots_2Sep/
#export COMBINEDFIGPATH=/home/hbakhshi/Documents/WPolarization/Note/notes/TOP-12-015/trunk/plots_23Sep/
export COMBINEDFIGPATH=/home/hbakhshi/Documents/Analysis/Run/WPolarization/Run/Combined/
find $PASFIGPATH -name "*.pdf" -print | while read FILE
do
    export fileName=`basename $FILE .pdf`
    ls $COMBINEDFIGPATH/$fileName.C
    cp $COMBINEDFIGPATH/$fileName.C $PASFIGPATH
    cp $COMBINEDFIGPATH/$fileName.pdf $PASFIGPATH
done
