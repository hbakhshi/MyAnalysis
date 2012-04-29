#! python

import os

SubmitFile = open( 'SubmitAll.sh' , 'w')

print >> SubmitFile, 'chmod a+x Job_*.sh'

for JobID in range(0 , 100):
    FileName = 'Job_%(JobID)02d.sh' % {'JobID':JobID}
    print >> SubmitFile, 'bsub -q 8nh %(File)s &' % {'File':FileName}
    JobFile = open( FileName , 'w')
    print >> JobFile, '. /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh'
    print >> JobFile, 'export PATH="/afs/cern.ch/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/bin:$PATH"'
    print >> JobFile, 'export LD_LIBRARY_PATH="/afs/cern.ch/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/lib:$LD_LIBRARY_PATH"'
    print >> JobFile, '. /afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.01/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh'
    print >> JobFile, 'cd /afs/cern.ch/user/h/hbakhshi/Desktop/WPolarization'
    print >> JobFile, '%(File)s %(Channel)s %(AllEventsHistogramDirectory)s %(FirstPEX)d %(PEXs)d %(FirstPull)d %(LPull)d' % {'File':'./FitValidation.py' , 'Channel':'EM', 'AllEventsHistogramDirectory':'./WPol/' , 'FirstPEX':JobID*3 , 'PEXs':3 , 'FirstPull':JobID*10 , 'LPull':10}
    JobFile.close()

SubmitFile.close()
