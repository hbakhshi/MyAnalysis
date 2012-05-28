#! /usr/bin/python
# -*- coding: utf-8 -*-
from ROOT import TFile, TTree, THStack, TCanvas, TH1D, TH1F, gROOT, TGraph, TGraphErrors 
from Table import *
from SampleManager import *
from Fit import *

from math import sqrt
import os

org_file = open("TablesDataMC.org" , 'w')

#TO ENABLE JS IN THE HTML OUTPUT 
print >>org_file,'#+INFOJS_OPT: view:content toc:nil' #http://orgmode.org/manual/JavaScript-support.html#JavaScript-support
print >>org_file,'#+TITLE: WPolarization, data/mc comparison'
#needed lines for hs.expand command works
print >>org_file,'#+STYLE: <script type="text/javascript" src="../highslide/highslide.js"></script>\n\
#+STYLE: <link rel="stylesheet" type="text/css" href="../highslide/highslide.css" /> \n\
#+STYLE: <script type="text/javascript">\n\
#+STYLE:    hs.graphicsDir = "./";\n\
#+STYLE:    hs.outlineType = null;\n\
#+STYLE:    hs.wrapperClassName = "colored-border";\n\
#+STYLE: </script>\n'
# download highslide from http://highslide.com/ 

WhichChannel = ''

sorted_samples = ['TTBarSummer2011', 'DYSummer2011' ,'SingleTopTWSummer2011', 'WJetsSummer2011' , 'WWSummer2011' , 'SingleTopSummer2011' , 'WZSummer2011', 'ZZSummer2011' ]
sorted_samples.reverse()
systematic_samples = [] #'SysWJetsQU' , 'SysTTQU' , 'SysTTQD' , 'SysZJetsQD' , 'SysZJetsQU' , 'SysTTM175' , 'SysWJetsQD' , 'SysTTM169']
SamplesInfo = {}
SysSamplesInfo = {}

stack_costheta_ee = THStack("stackCosTheta",'CosTheta for ee Events')
stack_costheta_mm = THStack("stackCosTheta",'CosTheta for mm Events')
stack_costheta_em = THStack("stackCosTheta",'CosTheta for em Events')
stack_costheta_comb = THStack("stackCosTheta",'CosTheta for all Events')

PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Electrons_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} }


for sample in sorted_samples:
    SamplesInfo[sample] = SampleInfo(sample)

for sample in systematic_samples:
    SysSamplesInfo[sample] = SampleInfo(sample)

Data = DataInfo()

SampleInfoType = type( SamplesInfo[SamplesInfo.keys()[0] ])


gFNeg = TGraphErrors()
gF0 = TGraphErrors()
gFPos = TGraphErrors()
hAxis = TH1D('hAxis' , 'Axis' , 4 , 0.5 ,4.5 )
hAxis.SetStats(0)
hAxis2 = hAxis.Clone('hAxis_0')
hAxis3 = hAxis.Clone('hAxis_p')

for  WhichChannel_ in ['EM' , 'MM' , 'EE', 'Combined']:
     WhichChannel = WhichChannel_
     
     t_mc = Table()
     t_mc_w = Table()

     #AllSamples = [ SampleInfoType. __getattribute__( SamplesInfo[sample] , WhichChannel) for sample in SamplesInfo ]
     AllSamples = dict( ( sample , SampleInfoType. __getattribute__( SamplesInfo[sample] , WhichChannel) ) for sample in SamplesInfo  )
     print AllSamples
     print >>org_file,'* ' + WhichChannel
     print >>org_file,"** Wights"

     last_column_name = ''
     last_column_name_w = ''

     for sampleName in sorted_samples:
         sample = AllSamples[sampleName]
         if not WhichChannel == 'Combined':
             print >>org_file, '  |%(file)s | %(weight)f' % {'file': sample.Sample , 'weight' :sample.Lumi_Weight}
         t_mc.append( sample.Row )
         t_mc_w.append( sample.RowW )
         last_column_name_w = sample.LastColumnW
         last_column_name = sample.LastColumn
     
     stack = SamplesStack( AllSamples , sorted_samples )     
     print >>org_file,"** CutFlow table"
     print >>org_file,t_mc
     sumRow = stack.Row
     last_sum = sumRow[ last_column_name ]
     nCols = len(t_mc.rows[0])
         
     print >>org_file,"    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

     print >>org_file,"** CutFlow table, Weighted"
     print >>org_file,t_mc_w
     sumRow_w = stack.RowW
     #print sumRow_w
     last_sum = sumRow_w[last_column_name_w]
     nCols = len(t_mc_w.rows[0])
         
     tbl_mc_weighted_formula_str = "    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

     tbl_data_tmp = Table(False, False)
     tbl_data_tmp.append( Data.AllPlots[WhichChannel_].RowW )
     
     r_data_mc_diff = RowObject()        
     for column in Data.AllPlots[WhichChannel_].RowW.keys():
         if column.find( 'DataSet' ) > -1:
             r_data_mc_diff[column] = 'Data-MC'
         else:
             r_data_mc_diff[column] = Data.AllPlots[WhichChannel_].RowW[column] - sumRow_w[column]
     tbl_data_tmp.append( r_data_mc_diff )

     r_weight_effect = RowObject()      
     for column in sumRow.keys():
         if column.find( 'DataSet' ) > -1:
             r_weight_effect[column] = 'Weight-Effect'
         else:
             r_weight_effect[column] = sumRow_w[column]/sumRow[column]

     
     r_weight_effect[Data.AllPlots[WhichChannel_].LastColumnW] = 0.0
     tbl_data_tmp.append(r_weight_effect)

     print >> org_file, tbl_data_tmp
     print >> org_file, tbl_mc_weighted_formula_str

     print >>org_file, '** Plot of Cos(\\theta)'
     print >> org_file, stack.DrawCosTheta( Data )

     print >>org_file,'** Control Plots'
     print >>org_file, stack.DrawAllProperties(Data)

     #start fitting 
     print >>org_file, '** Fit Results'
     tableRowFormat =  '    |%(FitName)s|%(nBins)s|%(FL)f ± %(FLErr)f | %(F0)f ± %(F0Err)f | %(FR)f ± %(FRErr)f | %(FRecGen)f ± %(FRecGenErr)f |'
     print >>org_file, '    |Method|nBins|F_{L} | F_{0} | F_{R} | F_{Rec/Gen} |'
     AllSamplesButTTBar =  dict( (sampleName , AllSamples[sampleName] ) for sampleName in AllSamples if sampleName.find('TTBar') < 0 )

     FL = 0
     FLErr = 0
     F0 = 0
     F0Err = 0
     FRecGenErr = 0
     FRecGen = 0
     FR = 0
     FRErr = 0
     for nBins in ['05' , '10' , '20' , '25' , '50' , '100']:
         stackAllButTTBar =  SamplesStack( AllSamplesButTTBar , sorted_samples ).stack_costheta[nBins]
         ttBarH =  SampleInfoType.__getattribute__( SamplesInfo['TTBarSummer2011'] , WhichChannel_).hCosTheta[nBins]
         dataH = Data.AllPlots[WhichChannel_].hCosTheta[nBins]
         llFunction = LLFunction.GetLLFunction( WhichChannel_ , stackAllButTTBar , dataH , ttBarH , PoissonDistribution , 0.3 , 0.7)[0]
         fitVals = GetMinimum( llFunction )
         FL = fitVals[0]
         FLErr = fitVals[3]
         F0 = fitVals[1]
         F0Err = fitVals[4]
         FRecGen = fitVals[2]
         FRecGenErr = fitVals[5]
         FR = 1.0 - FL - F0
         FRErr = sqrt( FLErr*FLErr + F0Err*F0Err )
         print >>org_file, tableRowFormat % {'nBins':nBins ,'FitName':'3D Fitting' , 'FL':FL , 'FLErr':FLErr , 'F0':F0 , 'F0Err':F0Err , 'FR':FR , 'FRErr':FRErr , 'FRecGen':FRecGen , 'FRecGenErr':FRecGenErr }

     pointID = gFNeg.GetN()
     
     hAxis.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)
     hAxis2.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)
     hAxis3.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)

     gFNeg.SetPoint( pointID , FL , pointID + 1 )
     gFNeg.SetPointError( pointID ,FLErr , 0.0 )
     
     gF0.SetPoint( pointID , F0 , pointID + 1 )
     gF0.SetPointError( pointID , F0Err , 0.0 )

     gFPos.SetPoint( pointID , FR , pointID + 1 )
     gFPos.SetPointError( pointID ,  FRErr , 0.0 )
     

print >>org_file, '* Fit Comparison'
if not os.access( 'FitResults' , os.F_OK) :
    os.mkdir( 'FitResults' )


allHorizontalHistos = {}
def DrawAndCompStandardVal( haxis  , graph , std , stdErr , name , title):  
    c = TCanvas(name)
    c.cd()

    xMin = std - 5*stdErr
    xMax = std + 5*stdErr 
    hAxisNeg = TH1F('hAxisxL'  , "" , 100 , xMin , xMax)
    allHorizontalHistos[ hAxisNeg.GetName() ] = hAxisNeg

    haxis.SetTitle(title)
    haxis.SetMinimum(xMin)
    haxis.SetMaximum(xMax)
    haxis.Draw('9HBAR')
    
    graph.SetLineWidth(3)
    graph.SetHistogram(hAxisNeg)
    graph.Draw('P*')


    gstd = TGraphErrors()
    gstd.SetName(name + 'gstd')
    gstd.SetFillColor(2);
    gstd.SetFillStyle(3003);
    gstd.SetPoint( 0 , std - stdErr , 0.5)
    gstd.SetPoint( 1 , std + stdErr , 0.5)
    gstd.SetPoint( 2 , std + stdErr , 4.5)
    gstd.SetPoint( 3 , std - stdErr , 4.5)
    gstd.SetPoint( 4 , std - stdErr , 0.5)

    gstd.SetHistogram(hAxisNeg)

    gstd.Draw('F')

    c.SaveAs('FitResults/'+name+'.gif')
    return 'FitResults/'+name+'.gif'

print >>org_file, '** F_{L}'
#flStd = 0.3
#flStdErr = 0.02

print >>org_file, '   [[['+ DrawAndCompStandardVal(hAxis , gFNeg , 0.29 , 0.02  , 'FL' , 'F_{L}')  +']]]'

print >>org_file, '** F_{0}'
print >>org_file, '   [[['+ DrawAndCompStandardVal(hAxis2 , gF0 , 0.71 , 0.02 , 'F0' , 'F_{0}')  +']]]'

print >>org_file, '** F_{R}'
print >>org_file, '   [[[' +DrawAndCompStandardVal(hAxis3 , gFPos , 0.0 , 0.02 , 'FR' , 'F_{R}')+ ']]]'

fOut = TFile('AllCombined.root' , 'RECREATE')
for sample in SamplesInfo:
    SamplesInfo[sample].Combined.hCosTheta.Write()
for sample in SysSamplesInfo:
    SysSamplesInfo[sample].Combined.hCosTheta.Write()
Data.Combined.hCosTheta.Write()
fOut.Close()
