#!/usr/local/bin/csh

# 1- getting the certification from https://ca.cern.ch/ca/ 
# 2- converting the certificate in a format appropriate for use in the Grid by      
#  * Open Mozilla browser (Firefox)
#  * In the main menu click Edit->Preferences->Advanced
#  * Click View Certificates
#  * From the list of certificates select the certificate you want to extract and click "Backup"
#  * Enter a file name to backup and click "Save"
#  * choose where you want the certificate to be saved and choose the file name making sure it ends with '.p12', such as 'myCert.p12'
#  * Choose a certificate backup password and click "Ok"
#  * Copy the above PKCS#12 myCert.p12 file to the computer where you will run grid-proxy-init.
#  * Extract your certificate (which contains the public key) and the private key:
#    o Certificate:
#      openssl pkcs12 -in myCert.p12 -clcerts -nokeys -out $HOME/.globus/usercert.pem
#    o To get the encrypted private key:
#      openssl pkcs12 -in myCert.p12 -nocerts -out $HOME/.globus/userkey.pem
#  * You must set the mode on your userkey.pem file to read/write only by the owner, otherwise grid-proxy-init will not use it
#      chmod go-rw $HOME/.globus/userkey.pem
#  * Delete the myCert.p12 file created above to avoid security issues.

source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.csh
eval `scramv1 runtime -csh`
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.csh
#source /afs/cern.ch/cms/ccs/wm/scripts/Crab/CRAB_2_4_4/crab.csh
#voms-proxy-init -voms cms

# crab -create all -submit all
# crab -status 
# crab -getoutput all
# crab -renewCredential 
# crab -copyData [range]
# crab -resubmit [range]
# crab -cfg [file] -> Default is crab.cfg.
