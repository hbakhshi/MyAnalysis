#!/bin/sh
set -o nounset

PROGNAME=$(basename $0)

function usage
{
  cat <<EOF
Query the Castor stager area to find out if a file is available there. If it is not
request staging. This script should be run before trying to use any Castor file.
If a list of files is specified using a catalog file, you will get at the end 
a list of files that are available on the stage area (file: ./tmp.cff)

Usage: $PROGNAME files.list <options>

       An example files.list
       ====================
       /store/Production/2007/9/11/PreCSA07-HLT-QCD_Pt_120_170-A1/0004/04AA5F93-0A67-DC11-B1E1-003048562A96.root
       /store/Production/2007/9/11/PreCSA07-HLT-QCD_Pt_120_170-A1/0004/10226A96-0A67-DC11-B918-003048562762.root
       /store/Production/2007/9/11/PreCSA07-HLT-QCD_Pt_120_170-A1/0004/2C735494-0A67-DC11-9FED-003048562968.root
       /store/Production/2007/9/11/PreCSA07-HLT-QCD_Pt_120_170-A1/0004/363F44B0-0A67-DC11-B05F-000423D98868.root
       /store/Production/2007/9/11/PreCSA07-HLT-QCD_Pt_120_170-A1/0004/409A1895-0A67-DC11-A424-00304856288A.root
       /store/Production/2007/9/11/PreCSA07-HLT-QCD_Pt_120_170-A1/0004/58C76396-0A67-DC11-9ACC-0030485627B8.root

Options are:
  -p|--prefix     use a prefix (D=/castor/cern.ch/cms that is usually the case if you get the list from DBS)
  -c|--cff        use a filename that will contain the staged files as a cff fragment (D=tmp.cff)
  -v|--verbose    Turn on debug statements (D=false)
  -h|--help       This message

   example: $PROGNAME files.list --prefix /castor/cern.ch --verbose

v0.7 27/09/2007 - Subir
EOF

  exit 1
}

[ $# -gt 0 ] || { usage; exit 1; }
infile=$1
shift

DOMAIN="/castor/cern.ch/user/hbakhshi/"
CFF_FILE="tmp.cff"
let "verbose = 0"
while [ $# -gt 0 ]; do
  case $1 in
    -p | --prefix )         shift
                            DOMAIN=$1
                            ;;
    -c | --cff )            shift
                            CFF_FILE=$1
                            ;;
    -v | --verbose )        let "verbose = 1"
                            ;;
    -h | --help )           usage
                            ;;
     * )                    usage
                            ;;
  esac
  shift
done

[ -e $CFF_FILE ] && rm $CFF_FILE

fileNIS=
for file in $(cat $infile)
do
  echo $file | grep $DOMAIN > /dev/null
  [ $? -eq 0 ] || file="$DOMAIN""$file"
  nsls -l $file | grep '^m' > /dev/null
  if [ $? -eq 0 ]; then
    stager_status=$(stager_qry -M $file | grep $file)
    echo $stager_status | grep -i 'error' > /dev/null
    if [ $? -eq 0 ]; then
      echo -- $stager_status
      fileNIS="$fileNIS -M $file" 
    else
      status=$(echo $stager_status | awk '{print $NF}')
      echo $status | grep -e 'STAGED' -e 'CANBEMIGR' > /dev/null
      if [ $? -eq 0 ]; then
        echo -- $file, status=$status 
        echo $'\t'\'rfio:$file\' >> $CFF_FILE
      else
        echo -- $file, status=$status
      fi
    fi
    continue
  else
    echo -- $file not found on tape [no ^m]! check path, may be on disk already  
    echo $'\t'\'rfio:$file\' >> $CFF_FILE
  fi
done

if [ "$fileNIS" != "" ]
then
  echo -- Requesting stage-in for the following files
  echo -- stager_get $fileNIS
  stager_get $fileNIS
fi

echo
cat $CFF_FILE
echo Number of files already staged: $(wc -l tmp.cff | awk '{print $1}')
exit 0
