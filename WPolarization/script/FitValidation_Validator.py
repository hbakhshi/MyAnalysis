#! /usr/bin/python -i

from ROOT import  TH2D,  TRandom, TH1F, TFile, TFitResultPtr , TFitResult, gROOT
import os

class ValidationPlots:
    def __init__(self , file0):
        gROOT.cd()
        self.PullF0 = file0.Get("hPullF0").Clone("PullF0")
        self.PullFNeg = file0.Get("hPullFNeg").Clone("PullFNeg")
        self.PullFPos = file0.Get("hPullFPos").Clone("PullFPos")

        self.NEntries = self.PullFPos.GetEntries()

    def Add(self, file1):
        self.PullF0.Add( file1.Get("hPullF0") )
        self.PullFNeg.Add( file1.Get("hPullFNeg") )
        self.PullFPos.Add( file1.Get("hPullFPos") )

        self.NEntries = self.PullFPos.GetEntries()


    def Fit(self):
        res = {}               

        res['NEntries'] = self.NEntries

        fitF0_ = self.PullF0.Fit('gaus', 'SQ')
        fitF0 = fitF0_.Get()
        allArgs = fitF0.GetParams()
        res['sigmaPullF0'] =  allArgs[2]

        fitFL_ = self.PullFNeg.Fit('gaus', 'SQ')
        fitFL = fitFL_.Get()
        allArgs = fitFL.GetParams()
        res['sigmaPullFL'] = allArgs[2]

        fitFR_ = self.PullFPos.Fit('gaus' , 'SQ')
        fitFR = fitFR_.Get()
        allArgs = fitFR.GetParams()
        res['sigmaPullFR'] = allArgs[2]

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
