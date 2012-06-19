from ROOT import TFile, TTree, THStack, TCanvas, TH1D, gROOT, kBlue, kAzure
from Table import *

import copy,os
from math import sqrt

gROOT.LoadMacro('tdrstyle.C')
from ROOT import setTDRStyle
setTDRStyle()

FileAddressPattern_Selected = '/home/hbakhshi/Documents/Analysis/Run/WPolarization/WPol_SelectedTTBars_%s.root'
FileAddressPattern_Full = '/home/hbakhshi/Documents/WPolarization/WPol/WPol_%s.root'

#FileAddressPattern_Selected = '/home/hbakhshi/Documents/WPolarization/BSFDN/WPol_%s_BSFDN.root'
#FileAddressPattern_Full = '/home/hbakhshi/Documents/WPolarization/BSFDN/WPol_%s_BSFDN.root'

#FileAddressPattern_Full = '/home/hbakhshi/Documents/WPolarization/WPol/WPol_%s.root'
#FileAddressPattern_Selected = '/home/hbakhshi/Documents/WPolarization/WPol/WPol_%s.root'

PropertiesHistoPathFormat = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s%(Folder)s/%(SF_OF)s%(Folder)s_DiLeptonEvent_EventTypevs%(Property)s'
SelectionPlotName = 'Selection/hEventSelection%(channel)s'
CosThetaPlotName = 'costheta_%(channel)s/hCosThetaAllLepton'
CosTheta2DPlotName = 'costheta_%(channel)s/hCosThetaAllGenVsREC'
CosThetaVSIsoPlotName = 'costheta_%(channel)s/hCosThetaAllLeptonsVsLeptonIsolation'
CosThetaVSDRName = 'costheta_%(channel)s/hCosThetaAllLeptonsVsLeptonJetDR'
CosThetaVSPtName = 'costheta_%(channel)s/hCosThetaAllLeptonsVsLeptonPt'

ChannelSpecificFactors = { 'EE':{'Overal':0.994 ,'DYSummer2011':1.56 } , 'EM':{'Overal':0.995 ,'DYSummer2011':1.31 } , 'MM':{'Overal':0.997 ,'DYSummer2011':1.09 } }
XSections = { 'TTBarSummer2011':157.5, 'DYSummer2011':2400.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 , 'WZSummer2011':0.6 , 'ZZSummer2011':4.65, 'SysWJetsQU':31314.0 , 'SysTTQU':157.5 , 'SysTTQD':157.5 , 'SysZJetsQD':3048.0 , 'SysZJetsQU':3048.0 , 'SysTTM175':157.5 , 'SysWJetsQD':31314.0 , 'SysTTM169':157.5 }

OLD_Colors = {'TTBarSummer2011':41, 'DYSummer2011':46 ,'WJetsSummer2011':31 , 'WWSummer2011':29 , 'SingleTopSummer2011':4 , 'SingleTopTWSummer2011':7 , 'WZSummer2011':90 , 'ZZSummer2011':66 ,  'Data':1  , 'SysWJetsQU':1 , 'SysTTQU':1 , 'SysTTQD':1, 'SysZJetsQD':1 , 'SysZJetsQU':1 , 'SysTTM175':1, 'SysWJetsQD':1 , 'SysTTM169':1 }

Colors = {'TTBarSummer2011':kBlue-2, 'DYSummer2011':kBlue-10 ,'WJetsSummer2011':kAzure-2 , 'WWSummer2011':kAzure - 9 , 'SingleTopSummer2011':kBlue+1 , 'SingleTopTWSummer2011':kAzure+1 , 'WZSummer2011':kAzure , 'ZZSummer2011':kBlue-6 ,  'Data':1  , 'SysWJetsQU':1 , 'SysTTQU':1 , 'SysTTQD':1, 'SysZJetsQD':1 , 'SysZJetsQU':1 , 'SysTTM175':1, 'SysWJetsQD':1 , 'SysTTM169':1 }


IntLumis = {'EE':4529.518 , 'MM':4459.007 , 'EM':4631.724}
stringToRemove = 'Summer2011'

class SampleChannelInfo:
    def SaveAllPlots():
        pass

    def __init__(self, channel, Sample,  FileFull, FileSelected , applyWeight = True , extractAllPlots = True):
        self.Sample = Sample
        formatting = {'channel':channel}
        self.Channel = channel

        self.hSelectionFull = FileFull.Get( SelectionPlotName % formatting )
        self.hSelectionWFull = FileFull.Get ( ( SelectionPlotName % formatting) + 'W' )
        self.hSelectionFinal = FileSelected.Get( SelectionPlotName % formatting )
        self.hSelectionWFinal = FileSelected.Get ( ( SelectionPlotName % formatting) + 'W' )

        self.Row  = RowObject()
        self.RowW = RowObject()
        colName = '%(colIndex)02d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
        self.Row[colName] = Sample
        self.RowW[colName] = Sample
        last_column_name = colName

        self.integrated_lumi = IntLumis[ channel ]

        if channel=='EM' :
            self.SF_OF_Value = 'OppositeFlavours'
            self.DirectoryNameForStepsFromFull = ['None', 'None', 'None' , '' , '_NJets' , '_MET' , 'None']
            self.DirectoryNameForStepsFromSelected = ['None', 'None', 'None' , 'None' , 'None' , 'None' , '_NumberOfBJets']
            self.EventTypeMinVal = 5
            self.EventTypeMaxVal = 8
        elif channel=='EE' :
            self.SF_OF_Value = 'SameFlavours'
            self.DirectoryNameForStepsFromFull = ['None', 'None', 'None' , '' , '_InvMass12' , '_InvMassZ', '_NJets', '_MET', 'None']
            self.DirectoryNameForStepsFromSelected = ['None', 'None', 'None' , 'None' , 'None' , 'None', 'None', 'None', '_NumberOfBJets']
            self.EventTypeMinVal = 1
            self.EventTypeMaxVal = 2
        elif channel=='MM' :
            self.SF_OF_Value = 'SameFlavours'
            self.DirectoryNameForStepsFromFull = ['None', 'None', 'None' , '' , '_InvMass12' , '_InvMassZ', '_NJets', '_MET', 'None']
            self.DirectoryNameForStepsFromSelected = ['None', 'None', 'None' , 'None' , 'None' , 'None', 'None', 'None', '_NumberOfBJets']
            self.EventTypeMinVal = 3
            self.EventTypeMaxVal = 4
        #Properties
        self.AllSortedDirectories = {}
        self.PropertiesToDraw = {  'NPrVtx':{} , 'NumberOfJets':{} , 'PFMET':{}, 'Dilepton_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} , 'PositiveLeptonEta':{} , 'PositiveLeptonPt':{} , 'NegativeLeptonEta':{} , 'NegativeLeptonPt':{} , 'FirstBJetPt':{} , 'SecondBJetPt':{} , 'ThirdBJetPt':{} , 'FirstJetEta':{} , 'SecondJetEta':{} , 'ThirdJetEta':{} }
        
        if applyWeight :
            channel_specific_factor = ChannelSpecificFactors[self.Channel]['Overal']
            if ChannelSpecificFactors[self.Channel].has_key(Sample):
                channel_specific_factor *= ChannelSpecificFactors[self.Channel][Sample]

            self.Lumi_Weight = channel_specific_factor*XSections[Sample]*self.integrated_lumi  / self.hSelectionFull.GetBinContent(1)
        else:
            self.Lumi_Weight = 1.0


        if extractAllPlots :
            for cut in range(1,self.hSelectionFull.GetNbinsX()+1) :
                cut_name=self.hSelectionFull.GetXaxis().GetBinLabel(cut)
                colName = '%(colIndex)02d-%(step)s' % {'colIndex': cut , 'step': cut_name}
                cut_folder_name = self.DirectoryNameForStepsFromFull[cut-1]
                file_for_this_cut = FileFull
                histoW_for_this_cut = self.hSelectionWFull
                histo_for_this_cut = self.hSelectionFull
                if cut_folder_name == 'None':
                    cut_folder_name = self.DirectoryNameForStepsFromSelected[cut-1]
                    file_for_this_cut = FileSelected
                    histoW_for_this_cut = self.hSelectionWFinal
                    histo_for_this_cut = self.hSelectionFinal
        
                nPassed = histo_for_this_cut.GetBinContent(cut)*self.Lumi_Weight
                nPassedW = histoW_for_this_cut.GetBinContent(cut)*self.Lumi_Weight

                if cut == self.hSelectionFull.GetNbinsX():
                    self.RowW[ colName+"_Err" ] = histo_for_this_cut.GetBinError(cut)
                    self.RowW[ colName+"_Weight" ] = self.Lumi_Weight

                self.RowW[ colName ] = nPassedW
                self.Row[ colName ] = nPassed
                last_column_name = colName

                if not cut_folder_name == 'None':
                    self.AllSortedDirectories[ len(self.AllSortedDirectories) ] = cut_folder_name
                    for property_name in self.PropertiesToDraw.keys():
                        hProp_path =  PropertiesHistoPathFormat % {'SF_OF':self.SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name }
                        hProp2 = file_for_this_cut.Get( hProp_path )
                        hProp_newName = '%(File)s_%(SF_OF)s_%(Channel)s%(Folder)s_DiLeptonEvent_%(Property)s' % {'SF_OF':self.SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name , 'File':Sample , 'Channel':self.Channel}
                        gROOT.cd()
                        self.PropertiesToDraw[property_name][cut_folder_name] = hProp2.ProjectionY(hProp_newName, self.EventTypeMinVal, self.EventTypeMaxVal, "o" )
                        self.PropertiesToDraw[property_name][cut_folder_name].Scale( self.Lumi_Weight )
                        self.PropertiesToDraw[property_name][cut_folder_name].SetTitle( Sample[ :Sample.find(stringToRemove) ] )
                        self.PropertiesToDraw[property_name][cut_folder_name].SetFillColor(Colors[Sample])
                        self.PropertiesToDraw[property_name][cut_folder_name].SetLineColor(Colors[Sample])
                        self.PropertiesToDraw[property_name][cut_folder_name].SetLineStyle(1)
                        self.PropertiesToDraw[property_name][cut_folder_name].SetFillStyle(1001)

        if self.Sample == 'TTBarSummer2011':
            CosTheta2D = FileSelected.Get( CosTheta2DPlotName % {'channel':channel.lower()} )
            CosTheta2D.Scale( self.Lumi_Weight )
            gROOT.cd()
            self.CosTheta2D = CosTheta2D.RebinX(10 , 'CosTheta2DGenRec_10_' + channel )
            self.CosTheta2D.RebinY(10)

        CosTheta = FileSelected.Get( CosThetaPlotName % {'channel':channel.lower()} )
        CosTheta.Scale(self.Lumi_Weight)
        last_column_name_w = '%(colIndex)02d-%(step)s' % {'colIndex':self.hSelectionFull.GetNbinsX()+1  , 'step': 'nTTBars'}
        self.RowW[ last_column_name_w  ] = 0
        for bin_cos_theta in range(1, CosTheta.GetNbinsX()+1):
            self.RowW[ last_column_name_w  ] += CosTheta.GetBinContent( bin_cos_theta )/2
        CosTheta.SetTitle(Sample[ :Sample.find(stringToRemove) ])
        
        CosTheta.SetFillStyle(1001)
        CosTheta.SetFillColor(Colors[Sample])
        CosTheta.SetLineColor(Colors[Sample])
        self.hCosTheta = {}
        gROOT.cd()
        self.hCosTheta['05'] = CosTheta.Rebin(20 , "costheta05_"+channel+"_" + Sample)
        self.hCosTheta['10'] = CosTheta.Rebin(10 , "costheta10_"+channel+"_" + Sample)
        self.hCosTheta['20'] = CosTheta.Rebin(5 , "costheta20_"+channel+"_" + Sample)
        self.hCosTheta['25'] = CosTheta.Rebin(4 , "costheta25_"+channel+"_" + Sample)
        self.hCosTheta['50'] = CosTheta.Rebin(2 , "costheta50_"+channel+"_" + Sample)
        self.hCosTheta['100'] = CosTheta.Rebin(1 , "costheta100_"+channel+"_" + Sample)


        #print FileSelected.GetName()
        CosThetaVSIso = FileSelected.Get( CosThetaVSIsoPlotName % {'channel':channel.lower()} )
        CosThetaVSIso.Scale(self.Lumi_Weight)
        
        CosThetaVSIso.SetFillStyle(1001)
        CosThetaVSIso.SetFillColor(Colors[Sample])
        CosThetaVSIso.SetLineColor(Colors[Sample])
        gROOT.cd()
        self.hCosTheta['IsoAll'] = CosThetaVSIso.ProjectionY(self.Sample + channel + "_pyAll" , 0 , -1 , "o").Rebin(10)
        self.hCosTheta['Iso18'] = CosThetaVSIso.ProjectionY(self.Sample + channel + "_py18" , 0 , 18*2 , "o").Rebin(10)
        self.hCosTheta['Iso16'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py16" , 0 , 16*2 , "o").Rebin(10)
        self.hCosTheta['Iso14'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py14" , 0 , 14*2 , "o").Rebin(10)
        self.hCosTheta['Iso12'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py12" , 0 , 12*2 , "o").Rebin(10)
        self.hCosTheta['Iso10'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py10" , 0 , 10*2 , "o").Rebin(10)
        self.hCosTheta['Iso08'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py08" , 0 , 8*2 , "o").Rebin(10)
        self.hCosTheta['Iso06'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py06" , 0 , 6*2 , "o").Rebin(10)
        self.hCosTheta['Iso04'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py04" , 0 , 4*2 , "o").Rebin(10)
        self.hCosTheta['Iso02'] = CosThetaVSIso.ProjectionY(self.Sample + channel +"_py02" , 0 , 2*2 , "o").Rebin(10)

        CosThetaVSDR = FileSelected.Get( CosThetaVSDRName % {'channel':channel.lower()} )
        CosThetaVSDR.Scale(self.Lumi_Weight)
        
        CosThetaVSDR.SetFillStyle(1001)
        CosThetaVSDR.SetFillColor(Colors[Sample])
        CosThetaVSDR.SetLineColor(Colors[Sample])
        gROOT.cd()
        self.hCosTheta['DR40'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR40" , 8 , -1 , "o").Rebin(10)
        self.hCosTheta['DR50'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR50" , 10 , -1 , "o").Rebin(10)
        self.hCosTheta['DR60'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR60" , 6*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR70'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR70" , 7*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR80'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR80" , 8*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR90'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR90" , 9*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR100'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR100" , 10*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR150'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR150" , 15*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR200'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR200" , 20*2 , -1 , "o").Rebin(10)
        self.hCosTheta['DR250'] = CosThetaVSDR.ProjectionY(self.Sample + channel + "_pyDR250" , 25*2 , -1 , "o").Rebin(10)

        CosThetaVSPt = FileSelected.Get( CosThetaVSPtName % {'channel':channel.lower()} )
        CosThetaVSPt.Scale(self.Lumi_Weight)
        
        CosThetaVSPt.SetFillStyle(1001)
        CosThetaVSPt.SetFillColor(Colors[Sample])
        CosThetaVSPt.SetLineColor(Colors[Sample])
        gROOT.cd()
        self.hCosTheta['Pt20'] = CosThetaVSPt.ProjectionY(self.Sample + channel + "_pyPt20" , 20 , -1 , "o").Rebin(10)
        self.hCosTheta['Pt25'] = CosThetaVSPt.ProjectionY(self.Sample + channel + "_pyPt25" , 25 , -1 , "o").Rebin(10)
        self.hCosTheta['Pt30'] = CosThetaVSPt.ProjectionY(self.Sample + channel + "_pyPt30" , 30 , -1 , "o").Rebin(10)
        self.hCosTheta['Pt35'] = CosThetaVSPt.ProjectionY(self.Sample + channel + "_pyPt35" , 35 , -1 , "o").Rebin(10)
        self.hCosTheta['Pt40'] = CosThetaVSPt.ProjectionY(self.Sample + channel + "_pyPt40" , 40 , -1 , "o").Rebin(10)
        self.hCosTheta['Pt45'] = CosThetaVSPt.ProjectionY(self.Sample + channel + "_pyPt45" , 45 , -1 , "o").Rebin(10)


        CosTheta = FileFull.Get( CosThetaPlotName % {'channel':channel.lower()} )
        CosTheta.Scale(self.Lumi_Weight)
        CosTheta.SetTitle(Sample[ :Sample.find(stringToRemove) ])
        
        CosTheta.SetFillStyle(1001)
        CosTheta.SetFillColor(Colors[Sample])
        CosTheta.SetLineColor(Colors[Sample])
        gROOT.cd()
        self.hCosThetaPreselected = CosTheta.Rebin(4, "preselected_costheta_"+channel+"_" + Sample)

        self.LastColumn = last_column_name
        self.LastColumnW = last_column_name_w

        if extractAllPlots :
            NPrVtsAllNameFormat = '%(File)s_%(Channel)s%(Folder)s_DiLeptonEvent_NPrVtxAlls'
            self.NPrVtxAll = FileSelected.Get('Selection/EventSelectionHistosAfterObjectCreation/AllSelctedEvents/AllSelctedEvents_DiLeptonEvent_EventTypevsNPrVtx').ProjectionY(NPrVtsAllNameFormat % {'File':Sample , 'Channel':channel , 'Folder':'Weighted' } , self.EventTypeMinVal, self.EventTypeMaxVal, "o" )
            self.NPrVtxAll.Scale( self.Lumi_Weight )
            self.NPrVtxAll.SetTitle( Sample[ :Sample.find(stringToRemove) ] )
            self.NPrVtxAll.SetFillColor(Colors[Sample])
            self.NPrVtxAll.SetLineColor(Colors[Sample])
            self.NPrVtxAll.SetLineStyle(1)
            self.NPrVtxAll.SetFillStyle(1001)

            self.NPrVtxAllNoW = FileSelected.Get('Selection/EventSelectionHistosAfterObjectCreation/AllSelctedEventsNoW/AllSelctedEventsNoW_DiLeptonEvent_EventTypevsNPrVtx').ProjectionY(NPrVtsAllNameFormat % {'File':Sample , 'Channel':channel , 'Folder':'NoWeighted' }, self.EventTypeMinVal, self.EventTypeMaxVal, "o" )
            self.NPrVtxAllNoW.Scale( self.Lumi_Weight )
            self.NPrVtxAllNoW.SetTitle( Sample[ :Sample.find(stringToRemove) ] )
            self.NPrVtxAllNoW.SetFillColor(Colors[Sample])
            self.NPrVtxAllNoW.SetLineColor(Colors[Sample])
            self.NPrVtxAllNoW.SetLineStyle(1)
            self.NPrVtxAllNoW.SetFillStyle(1001)


            self.AllTopRecPlots = {'TopToppt':None , 'TopTopEta':None , 'TopWpt':None , 'TopWEta':None ,'Topneutrinospt':None , 'TopneutrinosEta':None ,'AntitopAntitoppt':None , 'AntitopAntitopEta':None ,'AntitopWpt':None , 'AntitopWEta':None ,'Antitopneutrinospt':None , 'AntitopneutrinosEta':None ,'TTBarEffectiveMass':20  }
            TopRecPlotsNameFormat = 'neutrino_solver_%(channel)s/MinEffectiveMassResults/MinEffectiveMassResults_TTBarDileptonicEvent_%(prop)s'

            for plot in self.AllTopRecPlots.keys() :
                if self.AllTopRecPlots[plot] == None:
                    self.AllTopRecPlots[plot] = FileSelected.Get( TopRecPlotsNameFormat % {'channel':channel.lower() , 'prop':plot  } ).Clone( Sample + channel + plot )
                else:
                    nGroups = self.AllTopRecPlots[plot]
                    self.AllTopRecPlots[plot] = FileSelected.Get( TopRecPlotsNameFormat % {'channel':channel.lower() , 'prop':plot  } ).Rebin(nGroups , Sample + channel + plot )
                self.AllTopRecPlots[plot].Scale( self.Lumi_Weight )
                self.AllTopRecPlots[plot].SetTitle( Sample[ :Sample.find(stringToRemove)] )
                self.AllTopRecPlots[plot].SetFillColor(Colors[Sample])
                self.AllTopRecPlots[plot].SetLineColor(Colors[Sample])
                self.AllTopRecPlots[plot].SetLineStyle(1)
                self.AllTopRecPlots[plot].SetFillStyle(1001)

class SampleCombinedInfo:
    def __init__(self, Sample, EE , EM, MM , extractAllPlots = True):
        self.Channel = 'Combined'
        self.Sample = Sample
        self.Row  = RowObject()
        self.RowW = RowObject()
        colName = '%(colIndex)02d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
        self.Row[colName] = Sample
        self.RowW[colName] = Sample


        colName = '%(colIndex)02d-%(step)s' % {'colIndex':1  , 'step': 'All'}    
        self.Row[colName] = EM.hSelectionFull.GetBinContent(1)
        self.RowW[colName] = EM.hSelectionFull.GetBinContent(1)

        self.LastColumnW = colName
        self.LastColumn = colName

        for columnIndexInEM in range( 4 , len(EM.Row)):
            cut = sorted(EM.Row.keys())[columnIndexInEM]
            NameForOF = sorted(EE.Row.keys())[columnIndexInEM+2]
            self.Row[cut] = EM.Row[cut] + EE.Row[NameForOF] + MM.Row[NameForOF]
            self.LastColumn = cut


        for columnIndexInEM in range( 4 , len(EM.RowW)):
            cut = sorted(EM.RowW.keys())[columnIndexInEM]
            NameForOF = sorted(EE.RowW.keys())[columnIndexInEM+2]
            self.RowW[cut] = EM.RowW[cut] + EE.RowW[NameForOF] + MM.RowW[NameForOF]
            self.LastColumnW = cut


        if self.Sample == 'TTBarSummer2011':
            self.CosTheta2D = EE.CosTheta2D.Clone( 'CosTheta2DGenRec_10_Combined' )
            self.CosTheta2D.Add( MM.CosTheta2D )
            self.CosTheta2D.Add( EM.CosTheta2D )

        self.hCosTheta = {}

        for cosTheta in EE.hCosTheta.keys():            
            self.hCosTheta[cosTheta] = EE.hCosTheta[cosTheta].Clone('costheta'+ cosTheta +'_combined_' + Sample)
            self.hCosTheta[cosTheta].Add( MM.hCosTheta[cosTheta] )
            self.hCosTheta[cosTheta].Add( EM.hCosTheta[cosTheta] )
        
        
        self.hCosThetaPreselected = EE.hCosThetaPreselected.Clone('prescaled_costheta_combined_' + Sample)
        self.hCosThetaPreselected.Add( MM.hCosThetaPreselected )
        self.hCosThetaPreselected.Add( EM.hCosThetaPreselected )
        
        self.PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Dilepton_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} , 'PositiveLeptonEta':{} , 'PositiveLeptonPt':{} , 'NegativeLeptonEta':{} , 'NegativeLeptonPt':{} , 'FirstBJetPt':{} , 'SecondBJetPt':{} , 'ThirdBJetPt':{} , 'FirstJetEta':{} , 'SecondJetEta':{} , 'ThirdJetEta':{} }
        self.AllSortedDirectories = {}

        if extractAllPlots :
            for propertY in self.PropertiesToDraw.keys():
                EMProp = EM.PropertiesToDraw[propertY]
                EEProp = EE.PropertiesToDraw[propertY]
                MMProp = MM.PropertiesToDraw[propertY]

                for cutID in sorted( EM.AllSortedDirectories.keys() ):
                    cut = EM.AllSortedDirectories[cutID]
                    self.AllSortedDirectories[ cutID ] = cut
                    hPropName = '%(File)s_Combined_%(Folder)s_%(Property)s' % {'Folder':cut , 'Property':propertY , 'File':Sample }
                    self.PropertiesToDraw[propertY][cut] = EMProp[cut].Clone( hPropName )
                    cut_name_forSF = cut
                    if cut == '':
                        cut_name_forSF = '_InvMassZ'
                    self.PropertiesToDraw[propertY][cut].Add( EEProp[ cut_name_forSF ] )
                    self.PropertiesToDraw[propertY][cut].Add( MMProp[ cut_name_forSF ] )

            NPrVtsAllNameFormat = '%(File)s_%(Channel)s%(Folder)s_DiLeptonEvent_NPrVtxAlls'
            self.NPrVtxAllNoW = EE.NPrVtxAllNoW.Clone( NPrVtsAllNameFormat % {'File':Sample , 'Channel':self.Channel , 'Folder':'NoWeighted'} ) 
            self.NPrVtxAllNoW.Add( MM.NPrVtxAllNoW )
            self.NPrVtxAllNoW.Add( EM.NPrVtxAllNoW )

            self.NPrVtxAll = EE.NPrVtxAll.Clone( NPrVtsAllNameFormat % {'File':Sample , 'Channel':self.Channel , 'Folder':'Weighted'} ) 
            self.NPrVtxAll.Add( MM.NPrVtxAll )
            self.NPrVtxAll.Add( EM.NPrVtxAll )

            self.AllTopRecPlots = {'TopToppt':None , 'TopTopEta':None , 'TopWpt':None , 'TopWEta':None ,'Topneutrinospt':None , 'TopneutrinosEta':None ,'AntitopAntitoppt':None , 'AntitopAntitopEta':None ,'AntitopWpt':None , 'AntitopWEta':None ,'Antitopneutrinospt':None , 'AntitopneutrinosEta':None ,'TTBarEffectiveMass':None  }

            for plot in self.AllTopRecPlots.keys() :
                self.AllTopRecPlots[plot] = EE.AllTopRecPlots[plot].Clone( Sample + self.Channel + plot )
                self.AllTopRecPlots[plot].Add( MM.AllTopRecPlots[plot] )
                self.AllTopRecPlots[plot].Add( EM.AllTopRecPlots[plot] )



class SampleInfo:
    def __init__(self , sampleName , extractAllPlots = True):
        self.Name = sampleName
        self.FileSelected = TFile( FileAddressPattern_Selected % sampleName , 'READ')
        self.FileFull = TFile( FileAddressPattern_Full % sampleName , 'READ')

        
        self.EE = SampleChannelInfo( 'EE' , self.Name , self.FileFull , self.FileSelected ,True,  extractAllPlots )
        self.MM = SampleChannelInfo( 'MM' , self.Name , self.FileFull , self.FileSelected ,True,  extractAllPlots )
        self.EM = SampleChannelInfo( 'EM' , self.Name , self.FileFull , self.FileSelected ,True,  extractAllPlots )

        self.Combined = SampleCombinedInfo( self.Name , self.EE , self.EM , self.MM , extractAllPlots)

    def __del__(self):
        self.FileSelected.Close()
        self.FileFull.Close()

    def DrawProperties(self , Cut , Property):
        self.Combined.PropertiesToDraw[Property][Cut].Draw()

        self.EM.PropertiesToDraw[Property][Cut].Draw('SAMES')
        
        if Cut == '' :
            Cut = '_InvMassZ'

        self.EE.PropertiesToDraw[Property][Cut].Draw('SAMES')
        self.MM.PropertiesToDraw[Property][Cut].Draw('SAMES')

class DataInfo(SampleInfo):
    def __init__(self):
        self.Name = 'Data'
        self.DataFileNames = {'EE':'DoubleEle2011' , 'MM':'DoubleMuon2011' , 'EM':'ElectronMuon2011'}
        self.DataFullFiles = {}
        self.DataSelectedFiles = {}
        self.AllPlots = {}

        for channel in self.DataFileNames.keys():
            self.DataSelectedFiles[channel] = TFile( FileAddressPattern_Selected % self.DataFileNames[channel] , 'READ')
            self.DataFullFiles[channel] = TFile( FileAddressPattern_Full %  self.DataFileNames[channel] , 'READ')
            
            self.AllPlots[channel] = SampleChannelInfo( channel , self.Name , self.DataFullFiles[channel] , self.DataSelectedFiles[channel] , False)

        self.EE = self.AllPlots['EE']
        self.MM = self.AllPlots['MM']
        self.EM = self.AllPlots['EM']
        self.Combined = SampleCombinedInfo( self.Name , self.EE , self.EM , self.MM)
        self.AllPlots['Combined'] = self.Combined

    def __del__(self):
        for filename in self.DataFullFiles.keys():
            self.DataFullFiles[filename].Close()
            self.DataSelectedFiles[filename].Close()

class SamplesStack:
    def DrawAndSave(self, stack , hist , name , LogY = 0 , external_title = ''):

        if not os.access( self.Channel , os.F_OK) :
            os.mkdir( self.Channel )

        hOne = TH1D('hOne' , 'One' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )  
        hOne.SetStats(0)
        hOne.SetLineColor(4)
        hOne.SetLineWidth(2)
        hOne.SetLineStyle(3)

        for nBin in range(1, hist.GetNbinsX()+1):
            err = sqrt(hist.GetBinContent(nBin))
            hist.SetBinError(nBin, err)

            hOne.SetBinContent( nBin , 1)
            hOne.SetBinError( nBin , 0)

        c = TCanvas(name, '', 800, 800)
        c.Divide(1,2, 0.0 , 0.0)

        c1 = c.cd(1)

        c1.SetLeftMargin(0.09)
        c1.SetRightMargin(0.02333333)
        c1.SetTopMargin(0.04)

        c1.SetLogy(LogY)
        maxVal = stack.GetMaximum()
        if hist.GetBinContent(hist.GetMaximumBin()) > maxVal:
            stack.SetMaximum( 1.01*hist.GetBinContent(hist.GetMaximumBin()) )
            maxVal = hist.GetBinContent(hist.GetMaximumBin())

        stack.Draw("9 HIST")

        xaxisTitle = hist.GetXaxis().GetTitle()
        if not external_title == '':
            xaxisTitle = external_title

        stack.GetHistogram().GetXaxis().SetTitle( '' )
        stack.GetHistogram().GetXaxis().SetLabelSize(0.0)

        #if LogY == 1:
            #stack.GetHistogram().GetYaxis().SetRangeUser(1 , maxVal)
        #else:
        stack.GetHistogram().GetYaxis().SetRangeUser(0.0 , maxVal)
        
        hist.SetLineWidth( 2 )
        hist.GetXaxis().SetTitle('')
        hist.GetXaxis().SetLabelSize(0.0)
        hist.Draw("9 E1 SAME")

        c1.BuildLegend(0.7,0.7,1.0,1.0)
        c1.SetPad( 0 , 0.295 , 1, 1)

        c2 =  c.cd(2)
        c2.SetPad( c2.GetXlowNDC(), c2.GetXlowNDC() , 1 , 0.295)

        c2.Range(-2.636104,-0.3032786,225.4441,1.568279)

        c2.SetLeftMargin(0.09)
        c2.SetRightMargin(0.02333333)
        c2.SetTopMargin(0.04)
        c2.SetBottomMargin(0.4)

        hDivision = TH1D('hDivision' , 'Data/MC' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )
        hSum = TH1D('hSum' , 'SUM_MC' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )

        #hSum.Print("ALL")
        #hist.Print("ALL")

        Errors = {}
        for nBin in range(1, hist.GetNbinsX()+1):
            Errors[nBin] = 0.0

        for h_MC in stack.GetHists():
            hSum.Add( h_MC )
        
        for nBin in range(1, hSum.GetNbinsX()+1):
            Errors[nBin] = hSum.GetBinError(nBin)

        for nBin in range(1, hSum.GetNbinsX()+1):
            hSum.SetBinError( nBin , sqrt(Errors[nBin]) )

        hDivision.Divide( hist , hSum )
        hDivision.GetYaxis().SetRangeUser( 0.5 , 1.5)
        hDivision.SetStats(0)
        hDivision.SetLineWidth(2)
        hDivision.SetLineColor(2)
        hDivision.GetYaxis().SetLabelSize(0.08)
        hDivision.GetXaxis().SetLabelSize(0.1)
        hDivision.GetXaxis().SetTitleSize(0.2)
        hDivision.GetXaxis().SetTitle(xaxisTitle)
        #hDivision.GetXaxis().SetLabelSize(0.0)
        hDivision.GetYaxis().SetNdivisions(3 , 6 , 0 , True)
        hDivision.Draw()
        hOne.Draw("SAME E1")

        total_difference = 0.0
        for nBin in range(1, hist.GetNbinsX()+1):
            data_in_bin = hist.GetBinContent(nBin)
            mc_in_bin = hSum.GetBinContent(nBin)
            total_difference += (data_in_bin - mc_in_bin)

        c.cd()
        c.SaveAs(self.Channel + '/' + name + '.gif' )
        c.SaveAs(self.Channel + '/' + name + '.C' )
        c.SaveAs(self.Channel + '/' + name + '.pdf' )
        c.SaveAs(self.Channel + '/' + name + '.eps' )
        c.Close()
        return total_difference    

    def DrawCosTheta(self , Data):
        dataType = type( Data )
        data = dataType. __getattribute__(Data , self.Channel)
        
        deltaMC_Data_CosTheta = self.DrawAndSave( self.stack_costheta['10'] ,  data.hCosTheta['10'] , self.Channel + '_cos_theta' , 0 , 'cos(#theta^{*})' )
        deltaMC_Data_CosTheta_LogY = self.DrawAndSave( self.stack_costheta['10'] ,  data.hCosTheta['10'] , self.Channel + '_cos_theta_logy' , 1 , 'cos(#theta^{*})' )
        deltaMC_Data_CosThetaPreselected = self.DrawAndSave( self.stack_costheta_preselected ,  data.hCosThetaPreselected , self.Channel + '_cos_theta_preselected',1, 'cos(#theta^{*})' )
        ret = '*** Plot of Cos(\\theta) for selected events \n'
        ret = ret + '    - Difference is : 2*' + str(deltaMC_Data_CosTheta/2) + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s_cos_theta.gif]]]'%{'Channel':self.Channel}  + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s_cos_theta_logy.gif]]]'%{'Channel':self.Channel}  + '\n'
        ret = ret +  '*** Plot of Cos(\\theta) for pre-selected events \n'
        ret = ret + '    - Difference is : 2*' + str(deltaMC_Data_CosThetaPreselected/2)  + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s_cos_theta_preselected.gif]]]'%{'Channel':self.Channel} + '\n'
        return ret


    def DrawTopRecPlots(self, Data):
        dataType = type( Data )
        data = dataType. __getattribute__(Data , self.Channel)
        self.t_TopRecPropertiesLinks = Table(False)
        TheRow = RowObject()
        PropertiesLinks = {}
        for Property in self.AllTopRecStacks.keys():
            stack_prop = self.AllTopRecStacks[Property]
            hPropd = data.AllTopRecPlots[Property]
                
            ImgFileName = 'bbb'+ Property
            ImgFileName = ImgFileName.replace("_", 'z')
            TotalDiff = self.DrawAndSave(stack_prop, hPropd, ImgFileName ,0)
            
            TheRow[Property] = '@<a id="%(id)s" href="%(file)s" class="highslide" onclick="return hs.expand(this)">%(PropName)s@</a>' % {'file':self.Channel + '/' + ImgFileName + '.gif' , 'id':(Property).replace('_', 'x') , 'PropName':Property}


        stack_prop = self.stack_NPrVtxAll
        hPropd = data.NPrVtxAll
        
        ImgFileName = 'bbbNPrVtxAll'
        TotalDiff = self.DrawAndSave(stack_prop, hPropd, ImgFileName , 0)
        TheRow['NPrVtxAll'] = '@<a id="%(PropName)s" href="%(file)s" class="highslide" onclick="return hs.expand(this)">%(PropName)s@</a>' % {'file':self.Channel + '/' + ImgFileName + '.gif' ,  'PropName':Property}        

        stack_prop = self.stack_NPrVtxAllNoW
        hPropd = data.NPrVtxAllNoW      
        ImgFileName = 'bbbNPrVtxAllNoW'
        TotalDiff = self.DrawAndSave(stack_prop, hPropd, ImgFileName , 0)
        TheRow['NPrVtxAllNoW'] = '@<a id="%(PropName)s" href="%(file)s" class="highslide" onclick="return hs.expand(this)">%(PropName)s@</a>' % {'file':self.Channel + '/' + ImgFileName + '.gif' ,  'PropName':Property}        

        self.t_TopRecPropertiesLinks.append( TheRow )
        return self.t_TopRecPropertiesLinks
        

    def DrawAllProperties(self , Data):
        dataType = type( Data )
        data = dataType. __getattribute__(Data , self.Channel)
        self.t_PropertiesLinks = Table(False)
        PropertiesLinks = {}
        for Property in self.PropertiesToDraw.keys():
            PropertiesLinks[Property] = RowObject()
            
            PropertiesLinks[Property]['0-Property'] = Property
            
            for CutID in sorted(self.AllSortedDirectories.keys()):
                Cut = self.AllSortedDirectories[CutID]
                stack_prop = self.PropertiesToDraw[Property][Cut]
                hPropd = data.PropertiesToDraw[Property][Cut]
                if Cut == '':
                    stack_prop.SetTitle( '%(Channel)s : after pair choose' % {'Channel':self.Channel } )
                else:
                    stack_prop.SetTitle( '%(Channel)s : after cut on  %(Cut)s' % {'Channel':self.Channel, 'Cut':Cut  } )
                    
                ImgFileName = 'aaa'+ str(CutID)+ "_" +Cut+  "_"  + Property
                ImgFileName = ImgFileName.replace("_", 'z')
                TotalDiff = self.DrawAndSave(stack_prop, hPropd, ImgFileName , 0)
                               
                PropertiesLinks[Property]['%(ID)d-%(Name)s'%{'ID':CutID+1 , 'Name':Cut}] = '@<a id="%(id)s" href="%(file)s" class="highslide" onclick="return hs.expand(this)">%(Diff)d@</a>' % {'file':self.Channel + '/' + ImgFileName + '.gif' , 'id':(Cut+'_'+Property).replace('_', 'x') , 'Diff':TotalDiff}
                CutID = CutID+1

            self.t_PropertiesLinks.append( PropertiesLinks[Property] )
        return self.t_PropertiesLinks

        

    def __init__(self , ArrayOfAllSamples, sortedSamples , StackCosTheta = True , StackControlPlots = True , StackOtherPlots = True):
        self.Row  = RowObject()
        self.RowW = RowObject()
        
        sample_id = 0
        for sampleName in sortedSamples:
            if ArrayOfAllSamples.keys().count(sampleName) == 0:
                continue
            sample = ArrayOfAllSamples[sampleName]
            #print sample.Sample
            if sample_id == 0:
                self.Channel = sample.Channel
                self.AllSortedDirectories = sample.AllSortedDirectories
                
                if StackCosTheta:
                    self.stack_costheta = {}

                    for cosTheta in sample.hCosTheta.keys():
                        self.stack_costheta[cosTheta] =  THStack("stackCosTheta" +cosTheta+ "_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')
                    # self.stack_costheta['10'] =  THStack("stackCosTheta10_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')
                    # self.stack_costheta['20'] =  THStack("stackCosTheta20_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')
                    # self.stack_costheta['25'] =  THStack("stackCosTheta25_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')
                    # self.stack_costheta['50'] =  THStack("stackCosTheta50_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')
                    # self.stack_costheta['100'] =  THStack("stackCosTheta100_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')

                    self.stack_costheta_preselected =  THStack("preselected_stackCosTheta_" + self.Channel ,'CosTheta for preselected ' + self.Channel +' Events')

                if StackControlPlots:
                    self.PropertiesToDraw = {}

                    for column in sample.Row:
                        self.Row[column] = sample.Row[column]
                    self.Row[ sorted(self.Row.keys())[0] ] = ''
                    for column in sample.RowW:
                        self.RowW[column] = sample.RowW[column]
                    self.RowW[ sorted(self.RowW.keys())[0] ] = ''

                    for Property in sample.PropertiesToDraw.keys():
                        allStacks = {}
                        for Cut in sample.PropertiesToDraw[Property].keys():
                            stack_newName = 'stack_%(Channel)s_%(Cut)s_%(Property)s' % {'Cut':Cut , 'Property':Property , 'Channel':self.Channel }
                            allStacks[Cut]= THStack( stack_newName , stack_newName )
                            allStacks[Cut].Add( sample.PropertiesToDraw[Property][Cut] )

                        self.PropertiesToDraw[Property] = allStacks

                if StackOtherPlots:

                    self.AllTopRecStacks = {}
                    for prop in sample.AllTopRecPlots.keys():
                        self.AllTopRecStacks[prop] = THStack("stack_" + prop + "_" + self.Channel , sample.AllTopRecPlots[prop].GetTitle() )
                        self.AllTopRecStacks[prop].Add( sample.AllTopRecPlots[prop] )

                    self.stack_NPrVtxAll = THStack("stack_NPrVtxAll_" + self.Channel , '# Primary vertices')
                    self.stack_NPrVtxAllNoW = THStack("stack_NPrVtxAllNoW_" + self.Channel , '# Primary vertices , no pu')
                sample_id = 1
            else:
                if StackControlPlots:
                    for column in sorted(self.Row.keys())[1:]:
                        self.Row[column] = self.Row[column] + sample.Row[column]
                    for column in sorted(self.RowW.keys())[1:]:
                        self.RowW[column] = self.RowW[column] + sample.RowW[column]

                    for Property in sample.PropertiesToDraw.keys():
                        for Cut in sample.PropertiesToDraw[Property].keys():
                            self.PropertiesToDraw[Property][Cut].Add( sample.PropertiesToDraw[Property][Cut] )

                if StackOtherPlots:
                    for prop in sample.AllTopRecPlots.keys():
                        self.AllTopRecStacks[prop].Add( sample.AllTopRecPlots[prop] )

            if StackCosTheta:
                for cosTheta in sample.hCosTheta.keys():
                    self.stack_costheta[cosTheta].Add( sample.hCosTheta[cosTheta] )
                # self.stack_costheta['10'].Add( sample.hCosTheta['10'] )
                # self.stack_costheta['20'].Add( sample.hCosTheta['20'] )
                # self.stack_costheta['25'].Add( sample.hCosTheta['25'] )
                # self.stack_costheta['50'].Add( sample.hCosTheta['50'] )
                # self.stack_costheta['100'].Add( sample.hCosTheta['100'] )

                self.stack_costheta_preselected.Add( sample.hCosThetaPreselected )
                
            if StackOtherPlots:
                self.stack_NPrVtxAll.Add( sample.NPrVtxAll )
                self.stack_NPrVtxAllNoW.Add( sample.NPrVtxAllNoW )
