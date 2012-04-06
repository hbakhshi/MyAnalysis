#! /usr/bin/python -i
#! /public/V_CMSSW64/slc5_amd64_gcc434/external/python/2.6.4-cms14/bin/python

from ROOT import TFile, TTree, THStack, TCanvas, TH1D , gROOT
from Table import *

files = {}
files_xsec = { 'TTBarSummer2011':157.5, 'DYSummer2011':3048.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':7.9 }

files_ee_weights = { 'TTBarSummer2011':0.192709, 'DYSummer2011':0.475306 ,'WJetsSummer2011':1.896160 , 'WWSummer2011':0.017588 , 'SingleTopSummer2011':0.077388 , 'SingleTopTWSummer2011':0.045330 }
files_mm_weights = { 'TTBarSummer2011':0.189709, 'DYSummer2011':0.467907 ,'WJetsSummer2011':1.866643 , 'WWSummer2011':0.017314 , 'SingleTopSummer2011':0.076184 , 'SingleTopTWSummer2011':0.044625 }
files_em_weights = { 'TTBarSummer2011':0.197058, 'DYSummer2011':0.486031 ,'WJetsSummer2011':1.938946 , 'WWSummer2011':0.017985 , 'SingleTopSummer2011':0.079134 , 'SingleTopTWSummer2011':0.046353 }

files_data = {}

colors = [41, 46, 31 , 29 , 4, 7 ]

CalcWeights=False
MC=True
WhichChannel = 'em' # 'mm' 'em'

initial_steps = [] #'All' , 'Trigger' , 'Cleaning']
initial_step_format = 'Selection/EventSelection/%(STEP)s/%(STEP)s_Muons_NMuons'

steps = [ 'MET' , 'NumberOfBJets' ] # 'InvMass12', 'InvMassZ', 'NJets' , 'MET' , 'NumberOfBJets'

step_format_0 = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s/%(SF_OF)s_DiLeptonEvent_EventType'
step_format = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s_%(STEP)s/%(SF_OF)s_%(STEP)s_DiLeptonEvent_EventType'

SF_OF_Values = ['SameFlavours' , 'OppositeFlavours']
SF_OF_Value = SF_OF_Values[ 0 if (WhichChannel == 'ee' or WhichChannel == 'mm') else 1 ]

integrated_lumi = 0

last_column_name=''

if WhichChannel=='em' :
    files_data = { 'ElectronMuon2011':1 }
    if not CalcWeights:
        files = files_em_weights
    integrated_lumi = 4631.724
elif WhichChannel=='ee' :
    files_data = { 'DoubleEle2011':1 }
    if not CalcWeights:
        files = files_ee_weights
    integrated_lumi = 4529.518
elif WhichChannel=='mm' :
    files_data = { 'DoubleMuon2011':1 }
    if not CalcWeights:
        files = files_mm_weights
    integrated_lumi = 4459.007

if not MC :
    files = files_data

if CalcWeights:
	files = files_xsec

t=Table()

cosThetaPlot_Name = 'costheta_%(channel)s/hCosThetaAllLepton' % {'channel':WhichChannel}
hs = THStack("hs",WhichChannel)
histos = {}
canvases = {}

print WhichChannel
print "- Wights"

for wpol_file in files.keys():
    #print wpol_file
    colIndex = 0
    file = TFile( 'WPol_SelectedTTBars_%s.root' % wpol_file , "READ")
    r = RowObject()
    colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': 'DataSet'}    
    r[colName] = wpol_file
    colIndex = 1

    lumi_weight = 1
    if MC and not CalcWeights:
        lumi_weight = files[ wpol_file ]

    for step in initial_steps:
        #print "\t%s" % step
        fullName = initial_step_format % { 'STEP': step } 
        h = file.Get( fullName )
        colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': step}
        last_column_name=colName
        colIndex = colIndex+1

        
        if MC and step == 'All' and CalcWeights:
            lumi_weight = files[ wpol_file ]*integrated_lumi /  h.GetEntries()
            print '  |%(file)s | %(weight)f' % {'file': wpol_file , 'weight' : lumi_weight}

        r[colName] =  h.GetEntries()*lumi_weight

    fullName = step_format_0 % {'SF_OF': SF_OF_Value}
    h = file.Get( fullName )
    colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': 'DiLeptonSelection'}
    last_column_name=colName
    colIndex = colIndex+1        
    if h != None:
        if WhichChannel == 'ee':
            r[colName] = h.GetBinContent(1)*lumi_weight
        elif WhichChannel == 'mm' :
            r[colName] = h.GetBinContent(3)*lumi_weight
        else:
            r[colName] = (h.GetBinContent(5)+h.GetBinContent(7) )*lumi_weight

    for step in steps:
        #print '\t%s' %  step
        fullName = step_format % {'STEP': step , 'SF_OF': SF_OF_Value}
        h = file.Get( fullName )
        colName = '%(colIndex)d-%(step)s' % {'colIndex': colIndex , 'step': step}
        last_column_name=colName
        colIndex = colIndex+1        
        if h != None:
            if WhichChannel == 'ee':
                r[colName] = h.GetBinContent(1)*lumi_weight
            elif WhichChannel == 'mm' :
                r[colName] = h.GetBinContent(3)*lumi_weight
            else:
                r[colName] = (h.GetBinContent(5)+h.GetBinContent(7) )*lumi_weight

    t.append(r)
    
    hCosTheta = (file.Get(cosThetaPlot_Name))
    hCosTheta.Scale(lumi_weight)
    hCosTheta.SetTitle(wpol_file)
    hCosTheta.SetFillStyle(0)
    hCosTheta.SetLineColor(colors[len(histos)])
    gROOT.cd()
    histos[wpol_file] = hCosTheta.Clone()

    file.Close()

print "- CutFlow table"
print t
sumRow = t.GetSumRow()
last_sum = sumRow[last_column_name]
nCols = len(t.rows[0])

print "    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }


for file in histos.keys():
    hs.Add(histos[file])

c1 = TCanvas ("c1","stacked hists",10,10,700,900)
hs.Draw()

if MC:
    file = TFile('WPol_SelectedTTBars_%s.root' % files_data.keys()[0])
    hcostheta_data = file.Get(cosThetaPlot_Name)
    #hcostheta_data.SetFillColor()
    hcostheta_data.SetFillStyle(0)
    hcostheta_data.SetLineColor(1)
    #hcostheta_data.SetLineStyle()
    gROOT.cd()
    histos['data'] = hcostheta_data.Clone()
    file.Close()

histos['data'].Draw("SAME")

c1.BuildLegend()
