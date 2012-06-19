#! /usr/bin/python -i

from ROOT import  TH2D,  TRandom, TH1F, TFile, TFitResultPtr , TFitResult, gROOT
import os, math

class ValidationPlots:
    def __init__(self , file0):
        gROOT.cd()
        self.PullF0 = file0.Get("hPullF0").Rebin(10 , "PullF0")
        self.PullFNeg = file0.Get("hPullFNeg").Rebin( 10 , "PullFNeg")
        self.PullFPos = file0.Get("hPullFPos").Rebin( 10 , "PullFPos")

        self.ResF0 = file0.Get("hResF0").Clone("ResF0")
        self.ResFNeg = file0.Get("hResFNeg").Clone("ResFNeg")
        self.ResFPos = file0.Get("hResFPos").Clone("ResFPos")

        self.NEntries = self.PullFPos.GetEntries()
        self.AllResutls = {}

    def Add(self, file1):
        self.PullF0.Add( file1.Get("hPullF0").Rebin(10) )
        self.PullFNeg.Add( file1.Get("hPullFNeg").Rebin(10) )
        self.PullFPos.Add( file1.Get("hPullFPos").Rebin(10) )

        self.ResF0.Add( file1.Get("hResF0") )
        self.ResFNeg.Add( file1.Get("hResFNeg") )
        self.ResFPos.Add( file1.Get("hResFPos") )

        self.NEntries = self.PullFPos.GetEntries()


    def Fit(self):
        res = {}

        totalDiff = 0.0

        res['NEntries'] = self.NEntries

        fitF0_ = self.PullF0.Fit('gaus', 'SQ')
        fitF0 = fitF0_.Get()
        allArgs = fitF0.GetParams()
        res['sigmaPullF0'] =  allArgs[2]
        totalDiff = totalDiff + (1.0-res['sigmaPullF0'])


        fitFL_ = self.PullFNeg.Fit('gaus', 'SQ')
        fitFL = fitFL_.Get()
        allArgs = fitFL.GetParams()
        res['sigmaPullFL'] = allArgs[2]
        totalDiff = totalDiff + (1.0-res['sigmaPullFL'])

        fitFR_ = self.PullFPos.Fit('gaus' , 'SQ')
        fitFR = fitFR_.Get()
        allArgs = fitFR.GetParams()
        res['sigmaPullFR'] = allArgs[2]
        totalDiff = totalDiff + (1.0-res['sigmaPullFR'])


        res['totalDiff'] = totalDiff

        self.AllResutls[self.NEntries] = math.fabs(totalDiff )

        fitFRRes_ = self.ResFPos.Fit('gaus' , 'SQ')
        fitFRRes = fitFRRes_.Get()
        allArgs = fitFRRes.GetParams()
        res['sigmaResFR'] = allArgs[2]

        fitFLRes_ = self.ResFNeg.Fit('gaus' , 'SQ')
        fitFLRes = fitFLRes_.Get()
        allArgs = fitFLRes.GetParams()
        res['sigmaResFL'] = allArgs[2]

        fitF0Res_ = self.ResF0.Fit('gaus' , 'SQ')
        fitF0Res = fitF0Res_.Get()
        allArgs = fitF0Res.GetParams()
        res['sigmaResF0'] = allArgs[2]

        return res



AllFiles = os.listdir('.')

ValPlots = None
for f in AllFiles:
    if f.find('.root') == -1:
        continue
    root_file = TFile(f , 'READ')
    
    if ValPlots == None:
        ValPlots = ValidationPlots( root_file )
    else:
        ValPlots.Add( root_file )

    if ValPlots.NEntries % 30 == 0 :
        print ValPlots.Fit()

    root_file.Close()

print min(ValPlots.AllResutls,key = lambda a: ValPlots.AllResutls.get(a))
