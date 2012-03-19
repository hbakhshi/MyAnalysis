#! /public/V_CMSSW64/slc5_amd64_gcc434/external/python/2.6.4-cms14/bin/python

from ROOT import TFile, TTree
from Table import *

files = { 'DYSummer2011':3048.0 , 'SingleTopSummer2011':64.5, 'WWSummer2011':4.65} #'WJetsSummer2011':31314.0
#files = { 'DoubleEle2011B':1.0 , 'DoubleEle2011V4':1.0 , 'DoubleEle2011V6': 1.0 , 'DoubleEle2011Aug': 1.0 , 'DoubleEle2011May10': 1.0 }
#files = { 'DoubleMuon2011B':1.0 , 'DoubleMuon2011V4':1.0 , 'DoubleMuon2011V6': 1.0 , 'DoubleMuon2011Aug5': 1.0 , 'DoubleMuon2011May10': 1.0 }
MC=False
WhichChannel = 'mm' # 'mm' 'em'

initial_steps = ['All' , 'Trigger' , 'Cleaning']
initial_step_format = 'Selection/EventSelection/%(STEP)s/%(STEP)s_Muons_NMuons'

steps = ['InvMass12', 'InvMassZ', 'NJets' , 'MET' , 'bTag1' , 'bTag2']# , 'NumberOfBJets' , 'Triggers' ]
SF_OF_Values = ['SameFlavours' , 'OppositeFlavours']
step_format = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s_%(STEP)s/%(SF_OF)s_%(STEP)s_DiLeptonEvent_EventType'

SF_OF_Value = SF_OF_Values[ 0 if (WhichChannel == 'ee' or WhichChannel == 'mm') else 1 ]

integrated_lumi = 5000

t=Table()

for wpol_file in files.keys():
    #print wpol_file
    colIndex = 0
    file = TFile( 'WPol_%s.root' % wpol_file , "READ")
    r = RowObject()
    colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': 'DataSet'}    
    r[colName] = wpol_file
    colIndex = 1

    lumi_weight = 1

    for step in initial_steps:
        #print "\t%s" % step
        fullName = initial_step_format % { 'STEP': step } 
        h = file.Get( fullName )
        colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': step}
        colIndex = colIndex+1

        
        if MC and step == 'All':
            print 'weighting'
            lumi_weight = files[ wpol_file ]*integrated_lumi /  h.GetEntries()

        r[colName] =  h.GetEntries()*lumi_weight

    for step in steps:
        #print '\t%s' %  step
        fullName = step_format % {'STEP': step , 'SF_OF': SF_OF_Value}
        h = file.Get( fullName )
        colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': step}
        colIndex = colIndex+1        
        if h != None:
            if WhichChannel == 'ee':
                r[colName] = h.GetBinContent(1)*lumi_weight
            elif WhichChannel == 'mm' :
                r[colName] = h.GetBinContent(3)*lumi_weight
            else:
                r[colName] = (h.GetBinContent(5)+h.GetBinContent(7) )*lumi_weight

    t.append(r)
    file.Close()

print t
