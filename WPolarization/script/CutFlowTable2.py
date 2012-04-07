#! /usr/bin/python

from ROOT import TFile, TTree, THStack, TCanvas, TH1D , gROOT
from Table import *

WhichChannel = 'MM' # 'MM' 'EM'

#FILES and XSections
sorted_samples = ['TTBarSummer2011', 'DYSummer2011' ,'SingleTopTWSummer2011', 'WJetsSummer2011' , 'WWSummer2011' , 'SingleTopSummer2011'  ]
sorted_samples.reverse()
files_xsec = { 'TTBarSummer2011':157.5, 'DYSummer2011':3048.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 }

data_file = ''
DirectoryNameForSteps = []
SF_OF_Value = ''
integrated_lumi = 0

if WhichChannel=='EM' :
    data_file = 'ElectronMuon2011'
    integrated_lumi = 4631.724
    SF_OF_Value = 'OppositeFlavours'
    DirectoryNameForSteps = ['None', 'None', 'None' , '' , '_NJets' , '_MET' , '_NumberOfBJets']
elif WhichChannel=='EE' :
    data_file ='DoubleEle2011'
    integrated_lumi = 4529.518
    SF_OF_Value = 'SameFlavours'
    DirectoryNameForSteps = ['None', 'None', 'None' , '' , '_InvMass12' , '_InvMassZ', '_NJets', '_MET', '_NumberOfBJets']
elif WhichChannel=='MM' :
    data_file = 'DoubleMuon2011'
    integrated_lumi = 4459.007
    SF_OF_Value = 'SameFlavours'
    DirectoryNameForSteps = ['None', 'None', 'None' , '' , '_InvMass12' , '_InvMassZ', '_NJets', '_MET', '_NumberOfBJets']

#Properties
PropertiesToDraw = {} #{'NumberOfJets':{}, 'PFMET':{}, 'Electrons_InvariantMass':{}, 'JetsHT':{} }
PropertiesHistoPathFormat = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s%(Folder)s/%(SF_OF)s%(Folder)s_DiLeptonEvent_%(Property)s'

cosThetaPlotName = 'costheta_%(channel)s/hCosThetaAllLepton' % {'channel':WhichChannel.lower()}
stack_costheta = THStack("stackCosTheta",'CosTheta for ' + WhichChannel +' Events')

selectionPlotName = 'Selection/hEventSelection%(channel)s' % {'channel':WhichChannel}

#STYLE INFO
colors = {'TTBarSummer2011':41, 'DYSummer2011':46 ,'WJetsSummer2011':31 , 'WWSummer2011':29 , 'SingleTopSummer2011':4 , 'SingleTopTWSummer2011':7 }


t_mc = Table()
t_mc_w = Table()

print WhichChannel
print "- Wights"
last_column_name = ''

for wpol_file in sorted_samples:
    file = TFile( 'WPol_%s.root' % wpol_file , "READ")
    r = RowObject()
    r_w = RowObject()

    colName = '%(colIndex)d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
    r[colName] = wpol_file
    r_w[colName] = wpol_file
    last_column_name = colName

    
    hSelectionW = file.Get( selectionPlotName + 'W' )

    hSelection = file.Get( selectionPlotName )
    nAll = hSelection.GetBinContent(1)
    lumi_weight = files_xsec[ wpol_file ]*integrated_lumi /  nAll
    print  '  |%(file)s | %(weight)f' % {'file': wpol_file , 'weight' : lumi_weight}

    for cut in range(1,hSelection.GetNbinsX()+1) :
        cut_name=hSelection.GetXaxis().GetBinLabel(cut)
        colName = '%(colIndex)d-%(step)s' % {'colIndex': cut , 'step': cut_name}
        nPassed = hSelection.GetBinContent(cut)*lumi_weight

        cut_folder_name = DirectoryNameForSteps[cut-1]        
        r[colName] = nPassed

        nPassedW = hSelectionW.GetBinContent(cut)*lumi_weight

        if cut == hSelection.GetNbinsX() : #btag sf is not applied correctly in the hSelection
            hEventType_path =  PropertiesHistoPathFormat % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':'EventType' }
            hEventType = file.Get(hEventType_path)
            if WhichChannel == 'EE':
                nPassedW = hEventType.GetBinContent(1)*lumi_weight
            elif WhichChannel == 'MM' :
                nPassedW = hEventType.GetBinContent(3)*lumi_weight
            else:
                nPassedW = (hEventType.GetBinContent(5)+hEventType.GetBinContent(7) )*lumi_weight

        r_w[colName] = nPassedW

        last_column_name = colName

        if not cut_folder_name == 'None':
            for property_name in PropertiesToDraw.keys():
                hProp_path =  PropertiesHistoPathFormat % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name } 
                #print hProp_path
                hProp = file.Get( hProp_path )
                hProp.Scale(lumi_weight)
                hProp.SetTitle(wpol_file)
                hProp.SetFillStyle(0)
                hProp.SetLineColor(colors[wpol_file])
                gROOT.cd()
                hTemp = hProp.Clone()
                if PropertiesToDraw[property_name].keys().count(cut_folder_name)==0:
                    PropertiesToDraw[property_name][cut_folder_name] = THStack(WhichChannel + '_' + cut_folder_name + '_' + property_name, WhichChannel + '_' + cut_folder_name + '_' + property_name)
                PropertiesToDraw[property_name][cut_folder_name].Add( hTemp )

    hCosTheta = (file.Get(cosThetaPlotName))
    hCosTheta.Scale(lumi_weight)
    hCosTheta.SetTitle(wpol_file)
    hCosTheta.SetFillStyle(0)
    hCosTheta.SetLineColor(colors[wpol_file])
    gROOT.cd()
    hTemp = hCosTheta.Rebin(10, "costheta_"+wpol_file)
    stack_costheta.Add(hTemp)

    t_mc_w.append(r_w)
    t_mc.append(r)
    file.Close()


print "- CutFlow table"
print t_mc
sumRow = t_mc.GetSumRow()
last_sum = sumRow[last_column_name]
nCols = len(t_mc.rows[0])

print "    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

print "- CutFlow table, Weighted"
print t_mc_w
sumRow = t_mc_w.GetSumRow()
last_sum = sumRow[last_column_name]
nCols = len(t_mc.rows[0])

print "    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

t_data = Table()

file = TFile( 'WPol_%s.root' % data_file , "READ")
r = RowObject()

colName = '%(colIndex)d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
r[colName] = data_file

hSelection = file.Get( selectionPlotName )
for cut in range(1,hSelection.GetNbinsX()+1) :
    cut_name=hSelection.GetXaxis().GetBinLabel(cut)
    colName = '%(colIndex)d-%(step)s' % {'colIndex': cut , 'step': cut_name}
    nPassed = hSelection.GetBinContent(cut)
    r[colName] = nPassed
    cut_folder_name = DirectoryNameForSteps[cut-1]
    if not cut_folder_name == 'None':
        for property_name in PropertiesToDraw.keys():
            hProp_path =  PropertiesHistoPathFormat % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name } 
            hProp = file.Get( hProp_path )
            hProp.SetTitle(data_file)
            hProp.SetFillStyle(0)
            hProp.SetLineColor(1)
            
            c = TCanvas(WhichChannel + "_" + cut_folder_name + "_" + property_name , cut_folder_name + property_name, 800, 800)
            c.cd()
            PropertiesToDraw[property_name][cut_folder_name].Draw()
            hProp.Draw("SAME")

            c.BuildLegend(0.8,0.8,1,1)
            #c.SaveAs( c.GetName() + ".gif" )
            c.Close()

hCosTheta = (file.Get(cosThetaPlotName))
hCosTheta.SetTitle(data_file)
hCosTheta.SetFillStyle(0)
hCosTheta.SetLineColor(1)

hCosTheta.Rebin(10)

from math import sqrt
for nBin in range(1, 11):
    err = sqrt(hCosTheta.GetBinContent(nBin))
    hCosTheta.SetBinError(nBin, err)


c_costheta = TCanvas(WhichChannel +  '_cos_theta', 'cos_theta', 800, 800)
c_costheta.cd()

if stack_costheta.GetMaximum() > hCosTheta.GetBinContent(hCosTheta.GetMaximumBin()):
    stack_costheta.Draw()
    hCosTheta.Draw("E1 SAME")
else:
    stack_costheta.Draw()
    hCosTheta.Draw("E1 SAME")

c_costheta.BuildLegend(0.8,0.8,1,1)

c_costheta.SaveAs( c_costheta.GetName() + '.gif')
c_costheta.Close()

t_data.append(r)
file.Close()

print '- Data'
print t_data

