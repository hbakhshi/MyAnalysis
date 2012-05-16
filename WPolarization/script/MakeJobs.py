#! python

import os

for rangeid in range(0,4):

    SubmitFile = open( 'Submit%d.sh' % (rangeid) , 'w')

    for JobID in range(rangeid*25 , (rangeid+1)*25):
        print >> SubmitFile, '%(File)s %(Channel)s %(AllEventsHistogramDirectory)s %(FirstPEX)d %(PEXs)d %(FirstPull)d %(LPull)d %(CPU)d' % {'File':'../../bin/Goraz/FitValidation' , 'Channel':'Combined', 'AllEventsHistogramDirectory':'~/Documents/WPolarization/WPol/' , 'FirstPEX':JobID*3 , 'PEXs':3 , 'FirstPull':JobID*10 , 'LPull':10 , 'CPU':rangeid}

    SubmitFile.close()
