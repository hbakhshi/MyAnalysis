#! python

import os

for rangeid in range(0,8):

    SubmitFile = open( 'Submit%d.sh' % (rangeid) , 'w')

    start = rangeid*12
    end = (rangeid+1)*12
    if rangeid == 7:
        end = 100

    for JobID in range(start , end):
        print >> SubmitFile, '%(File)s %(Channel)s %(AllEventsHistogramDirectory)s %(FirstPEX)d %(PEXs)d %(FirstPull)d %(LPull)d %(CPU)d' % {'File':'../../../bin/cms-computing/FitValidation' , 'Channel':'Combined', 'AllEventsHistogramDirectory':'/home/hbakhshi/WPolarization/WPol/' , 'FirstPEX':JobID*3 , 'PEXs':3 , 'FirstPull':JobID*10 , 'LPull':10 , 'CPU':rangeid}

    SubmitFile.close()
