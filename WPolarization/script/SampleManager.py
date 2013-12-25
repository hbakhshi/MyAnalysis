from ROOT import TFile, TTree, THStack, TCanvas, TH1D, gROOT, kBlue, kAzure, kRed, kGreen,kCyan,kGray, kBlack,kOrange, TLegendEntry, TLegend, TGraphErrors, TLatex, TH2D
from Table import *

import copy,os,array
from math import sqrt

gROOT.LoadMacro('tdrstyle.C')
from ROOT import setTDRStyle
setTDRStyle()

FileAddressPattern_Selected = '/home/hbakhshi/Documents/Analysis/Run/WPolarization/Run/WPol_SelectedTTBars_%s.root'
FileAddressPattern_Full = '/home/hbakhshi/Documents/WPolarization/WPol/WPol_%s.root'

#FileAddressPattern_Selected = '/home/hbakhshi/Documents/WPolarization/JESUP/WPol_%s_JESUP.root'
#FileAddressPattern_Full = '/home/hbakhshi/Documents/WPolarization/JESUP/WPol_%s_JESUP.root'

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
#ChannelSpecificFactors = { 'EE':{'Overal':0.994 ,'DYSummer2011':1 } , 'EM':{'Overal':0.995 ,'DYSummer2011':1 } , 'MM':{'Overal':0.997 ,'DYSummer2011':1 } }
XSections = { 'TTBarSummer2011':157.5, 'DYSummer2011':2400.0 , 'DY10To50Summer2011':9611.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 , 'WZSummer2011':0.6 , 'ZZSummer2011':4.65, 'SysWJetsQU':31314.0 , 'SysTTQU':157.5 , 'SysTTQD':157.5 , 'SysZJetsQD':3048.0 , 'SysZJetsQU':3048.0 , 'SysTTM175':157.5 , 'SysWJetsQD':31314.0 , 'SysTTM169':157.5 }


SampleCategoriesOrdered = ['t#bar{t}' , 'diboson' , 'single top' , 'DY+Jets' , 'Data' , 'Systematics']
SampleCategories = { 't#bar{t}':['TTBarSummer2011'] , 'diboson':['WJetsSummer2011','WWSummer2011','WZSummer2011','ZZSummer2011'] , 'single top' :['SingleTopTWSummer2011' , 'SingleTopSummer2011'] , 'DY+Jets':['DYSummer2011' , 'DY10To50Summer2011'] , 'Data':['Data'], 'Systematics':['SysWJetsQU' , 'SysTTQU' , 'SysTTQD', 'SysZJetsQD' , 'SysZJetsQU' , 'SysTTM175', 'SysWJetsQD' , 'SysTTM169' ] }

SampleCategoriesColors = {'t#bar{t}':kRed , 'diboson':kBlue , 'single top':kGreen , 'DY+Jets':kGray , 'Data':kBlack , 'Systematics':kRed }

OLD_Colors = {'TTBarSummer2011':41, 'DYSummer2011':46 ,'WJetsSummer2011':31 , 'WWSummer2011':29 , 'SingleTopSummer2011':4 , 'SingleTopTWSummer2011':7 , 'WZSummer2011':90 , 'ZZSummer2011':66 ,  'Data':1  , 'SysWJetsQU':1 , 'SysTTQU':1 , 'SysTTQD':1, 'SysZJetsQD':1 , 'SysZJetsQU':1 , 'SysTTM175':1, 'SysWJetsQD':1 , 'SysTTM169':1 }

Colors = {'TTBarSummer2011':kBlue-2, 'DYSummer2011':kBlue-10, 'DY10To50Summer2011':kBlue-4 ,'WJetsSummer2011':kAzure-2 , 'WWSummer2011':kAzure - 9 , 'SingleTopSummer2011':kBlue+1 , 'SingleTopTWSummer2011':kAzure+1 , 'WZSummer2011':kAzure , 'ZZSummer2011':kBlue-6 ,  'Data':1  , 'SysWJetsQU':1 , 'SysTTQU':1 , 'SysTTQD':1, 'SysZJetsQD':1 , 'SysZJetsQU':1 , 'SysTTM175':1, 'SysWJetsQD':1 , 'SysTTM169':1 }


IntLumis = {'EE':4529.518 , 'MM':4459.007 , 'EM':4631.724}
stringToRemove = 'Summer2011'

class SampleChannelInfo:
    def SaveAllPlots():
        pass

    def __init__(self, channel, Sample,  FileFull, FileSelected , applyWeight = True , extractAllPlots = True):
        self.Category = ""
        for cat in SampleCategories.keys():
            if SampleCategories[cat].count( Sample ) > 0:
                self.Category = cat
                self.Color = SampleCategoriesColors[cat]
                #print Sample + ' ' + self.Category + '  ' + str(self.Color)
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
        self.PropertiesToDraw = { 'NPrVtx':{} , 'NumberOfJets':{} , 'PFMET':{}, 'Dilepton_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} , 'PositiveLeptonEta':{} , 'PositiveLeptonPt':{} , 'NegativeLeptonEta':{} , 'NegativeLeptonPt':{} , 'FirstBJetPt':{} , 'SecondBJetPt':{} , 'ThirdBJetPt':{} , 'FirstJetEta':{} , 'SecondJetEta':{} , 'ThirdJetEta':{} , 'DPhi':{} }
        
        if applyWeight :
            channel_specific_factor = ChannelSpecificFactors[self.Channel]['Overal']
            if ChannelSpecificFactors[self.Channel].has_key(Sample):
                channel_specific_factor *= ChannelSpecificFactors[self.Channel][Sample]
                print self.Channel + " - " + Sample + " Factor is applied"
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

                #if cut == self.hSelectionFull.GetNbinsX():
                #self.RowW[ colName+"_Err" ] = 
                #self.RowW[ colName+"_Weight" ] = self.Lumi_Weight

                self.RowW[ colName ] = Variable( nPassedW , histo_for_this_cut.GetBinError(cut)*self.Lumi_Weight ) 
                self.Row[ colName ] = nPassed
                last_column_name = colName

                if not cut_folder_name == 'None':
                    self.AllSortedDirectories[ len(self.AllSortedDirectories) ] = cut_folder_name
                    for property_name in self.PropertiesToDraw.keys():
                        hProp_path =  PropertiesHistoPathFormat % {'SF_OF':self.SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name }
                        hProp2 = None
                        hProp2 = file_for_this_cut.Get( hProp_path )
                        if hProp2==None:
                            hProp2 = TH2D()

                        hProp_newName = '%(File)s_%(SF_OF)s_%(Channel)s%(Folder)s_DiLeptonEvent_%(Property)s' % {'SF_OF':self.SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name , 'File':Sample , 'Channel':self.Channel}
                        gROOT.cd()
                        self.PropertiesToDraw[property_name][cut_folder_name] = hProp2.ProjectionY(hProp_newName, self.EventTypeMinVal, self.EventTypeMaxVal, "o" )
                        if property_name == "DPhi":
                            self.PropertiesToDraw[property_name][cut_folder_name].Rebin(9)
                        self.PropertiesToDraw[property_name][cut_folder_name].Scale( self.Lumi_Weight )
                        self.PropertiesToDraw[property_name][cut_folder_name].SetTitle( Sample[ :Sample.find(stringToRemove) ] )
                        self.PropertiesToDraw[property_name][cut_folder_name].SetFillColor(self.Color)
                        self.PropertiesToDraw[property_name][cut_folder_name].SetLineColor(self.Color)
                        self.PropertiesToDraw[property_name][cut_folder_name].SetLineStyle(1)
                        self.PropertiesToDraw[property_name][cut_folder_name].SetFillStyle(1001)

        CosTheta = FileSelected.Get( CosThetaPlotName % {'channel':channel.lower()} )
        CosTheta.Scale(self.Lumi_Weight)

        self.HasFullCosThetaInfo = False
        if self.Sample == 'TTBarSummer2011' or self.Sample.find('SysTT') >= 0:
            print self.Sample
            CosTheta2D = FileSelected.Get( CosTheta2DPlotName % {'channel':channel.lower()} )
            CosTheta2D.Scale( self.Lumi_Weight )
            gROOT.cd()

            rec_projection = CosTheta2D.ProjectionX( "cosTheta2d_Projection_" + channel )
            #rec_projection.Print("base")
            CosTheta.Add( rec_projection )

            self.CosTheta2DOriginal = CosTheta2D.Clone('CosTheta2DGenRec_Original_' + channel )

            self.CosTheta2D = CosTheta2D.RebinX(10 , 'CosTheta2DGenRec_10_' + channel )
            self.CosTheta2D.RebinY(1)
            
            if extractAllPlots:
                CosThetaDir = FileSelected.Get( 'costheta_%(channel)s' % {'channel':channel.lower()} )
                hasCosThetaNeg_vs_Pos_TTBar = CosThetaDir.FindKeyAny('hCosThetaNeg_vs_Pos_TTBar')
                self.HasFullCosThetaInfo = (not (hasCosThetaNeg_vs_Pos_TTBar == None))
                if self.HasFullCosThetaInfo :
                    self.hCosThetaNeg_vs_Pos_TTBar = CosThetaDir.Get('hCosThetaNeg_vs_Pos_TTBar')
                    self.hCosThetaNeg_vs_Pos_TTBar.Scale( self.Lumi_Weight )
                    self.hCosThetaNeg_vs_Pos_TTBar.SetTitle( Sample[ :Sample.find(stringToRemove)] )

                    self.hCosThetaNeg_vs_Pos_Gen   = CosThetaDir.Get('hCosThetaNeg_vs_Pos_Gen'  )
                    self.hCosThetaNeg_vs_Pos_Gen.Scale( self.Lumi_Weight )
                    self.hCosThetaNeg_vs_Pos_Gen.SetTitle( Sample[ :Sample.find(stringToRemove)] )

                    self.hCosThetaVSTopEta_Gen = CosThetaDir.Get('hCosThetaAllLeptonsVsTopEta_Gen')
                    self.hCosThetaVSTopEta_Gen.Scale( self.Lumi_Weight )
                    self.hCosThetaVSTopEta_Gen.SetTitle( Sample[ :Sample.find(stringToRemove)] )

        
        last_column_name_w = '%(colIndex)02d-%(step)s' % {'colIndex':self.hSelectionFull.GetNbinsX()+1  , 'step': 'nTTBars'}
        self.RowW[ last_column_name_w  ] = Variable(0.0 , 0.0)
        for bin_cos_theta in range(1, CosTheta.GetNbinsX()+1):
            self.RowW[ last_column_name_w  ] += Variable(  CosTheta.GetBinContent( bin_cos_theta )/2 , CosTheta.GetBinError( bin_cos_theta )/2    )
        CosTheta.SetTitle(Sample[ :Sample.find(stringToRemove) ])
        
        CosTheta.SetFillStyle(1001)
        CosTheta.SetFillColor(self.Color)
        CosTheta.SetLineColor(self.Color)
        self.hCosTheta = {}
        gROOT.cd()
        self.hCosTheta['05'] = CosTheta.Rebin(20 , "costheta05_"+channel+"_" + Sample)
        self.hCosTheta['10'] = CosTheta.Rebin(10 , "costheta10_"+channel+"_" + Sample)
        self.hCosTheta['20'] = CosTheta.Rebin(5 , "costheta20_"+channel+"_" + Sample)
        self.hCosTheta['25'] = CosTheta.Rebin(4 , "costheta25_"+channel+"_" + Sample)
        self.hCosTheta['50'] = CosTheta.Rebin(2 , "costheta50_"+channel+"_" + Sample)
        self.hCosTheta['100'] = CosTheta.Rebin(1 , "costheta100_"+channel+"_" + Sample)


        if extractAllPlots:
            #print FileSelected.GetName()
            CosThetaVSIso = FileSelected.Get( CosThetaVSIsoPlotName % {'channel':channel.lower()} )
            CosThetaVSIso.Scale(self.Lumi_Weight)

            CosThetaVSIso.SetFillStyle(1001)
            CosThetaVSIso.SetFillColor(self.Color)
            CosThetaVSIso.SetLineColor(self.Color)
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
            CosThetaVSDR.SetFillColor(self.Color)
            CosThetaVSDR.SetLineColor(self.Color)
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
            CosThetaVSPt.SetFillColor(self.Color)
            CosThetaVSPt.SetLineColor(self.Color)
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
        CosTheta.SetFillColor(self.Color)
        CosTheta.SetLineColor(self.Color)
        gROOT.cd()
        self.hCosThetaPreselected = CosTheta.Rebin(4, "preselected_costheta_"+channel+"_" + Sample)

        self.LastColumn = last_column_name
        self.LastColumnW = last_column_name_w

        if extractAllPlots :
            CosThetaDir = FileSelected.Get( 'costheta_%(channel)s' % {'channel':channel.lower()} )
            hasCosThetaNeg_vs_Pos = CosThetaDir.FindKeyAny('hCosThetaNeg_vs_Pos')
            self.HasFullCosThetaInfo = (not (hasCosThetaNeg_vs_Pos == None))
            if self.HasFullCosThetaInfo :
                self.hCosThetaNeg_vs_Pos = CosThetaDir.Get('hCosThetaNeg_vs_Pos')
                self.hCosThetaNeg_vs_Pos.Scale( self.Lumi_Weight )
                self.hCosThetaNeg_vs_Pos.SetTitle( Sample[ :Sample.find(stringToRemove)] )

                self.hCosThetaVSTopEta = CosThetaDir.Get('hCosThetaAllLeptonsVsTopEta')
                self.hCosThetaVSTopEta.Scale( self.Lumi_Weight )
                self.hCosThetaVSTopEta.SetTitle( Sample[ :Sample.find(stringToRemove)] )

            NPrVtsAllNameFormat = '%(File)s_%(Channel)s%(Folder)s_DiLeptonEvent_NPrVtxAlls'
            self.NPrVtxAll = FileSelected.Get('Selection/EventSelectionHistosAfterObjectCreation/AllSelctedEvents/AllSelctedEvents_DiLeptonEvent_EventTypevsNPrVtx').ProjectionY(NPrVtsAllNameFormat % {'File':Sample , 'Channel':channel , 'Folder':'Weighted' } , self.EventTypeMinVal, self.EventTypeMaxVal, "o" )
            self.NPrVtxAll.Scale( self.Lumi_Weight )
            self.NPrVtxAll.SetTitle( Sample[ :Sample.find(stringToRemove) ] )
            self.NPrVtxAll.SetFillColor(self.Color)
            self.NPrVtxAll.SetLineColor(self.Color)
            self.NPrVtxAll.SetLineStyle(1)
            self.NPrVtxAll.SetFillStyle(1001)

            self.NPrVtxAllNoW = FileSelected.Get('Selection/EventSelectionHistosAfterObjectCreation/AllSelctedEventsNoW/AllSelctedEventsNoW_DiLeptonEvent_EventTypevsNPrVtx').ProjectionY(NPrVtsAllNameFormat % {'File':Sample , 'Channel':channel , 'Folder':'NoWeighted' }, self.EventTypeMinVal, self.EventTypeMaxVal, "o" )
            self.NPrVtxAllNoW.Scale( self.Lumi_Weight )
            self.NPrVtxAllNoW.SetTitle( Sample[ :Sample.find(stringToRemove) ] )
            self.NPrVtxAllNoW.SetFillColor(self.Color)
            self.NPrVtxAllNoW.SetLineColor(self.Color)
            self.NPrVtxAllNoW.SetLineStyle(1)
            self.NPrVtxAllNoW.SetFillStyle(1001)


            self.AllTopRecPlots = {'TopToppt':None , 'TopTopEta':2 , 'TopWpt':None , 'TopWEta':None ,'Topneutrinospt':None , 'TopneutrinosEta':None ,'AntitopAntitoppt':None , 'AntitopAntitopEta':2 ,'AntitopWpt':None , 'AntitopWEta':None ,'Antitopneutrinospt':None , 'AntitopneutrinosEta':None ,'TTBarEffectiveMass':20  }
            TopRecPlotsNameFormat = 'neutrino_solver_%(channel)s/MinEffectiveMassResults/MinEffectiveMassResults_TTBarDileptonicEvent_%(prop)s'

            for plot in self.AllTopRecPlots.keys() :
                if self.AllTopRecPlots[plot] == None:
                    self.AllTopRecPlots[plot] = FileSelected.Get( TopRecPlotsNameFormat % {'channel':channel.lower() , 'prop':plot  } ).Clone( Sample + channel + plot )
                else:
                    nGroups = self.AllTopRecPlots[plot]
                    self.AllTopRecPlots[plot] = FileSelected.Get( TopRecPlotsNameFormat % {'channel':channel.lower() , 'prop':plot  } ).Rebin(nGroups , Sample + channel + plot )
                self.AllTopRecPlots[plot].Scale( self.Lumi_Weight )
                self.AllTopRecPlots[plot].SetTitle( Sample[ :Sample.find(stringToRemove)] )
                self.AllTopRecPlots[plot].SetFillColor(self.Color)
                self.AllTopRecPlots[plot].SetLineColor(self.Color)
                self.AllTopRecPlots[plot].SetLineStyle(1)
                self.AllTopRecPlots[plot].SetFillStyle(1001)

class SampleCombinedInfo:
    def __init__(self, Sample, EE , EM, MM , extractAllPlots = True):

        self.Category = EE.Category
        self.Color = EE.Color
        self.Channel = 'Combined'
        self.Sample = Sample
        self.Row  = RowObject()
        self.RowW = RowObject()
        colName = '%(colIndex)02d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
        self.Row[colName] = Sample
        self.RowW[colName] = Sample


        colName = '%(colIndex)02d-%(step)s' % {'colIndex':1  , 'step': 'All'}    
        self.Row[colName] = EM.hSelectionFull.GetBinContent(1)
        self.RowW[colName] = Variable (EM.hSelectionFull.GetBinContent(1) , EM.hSelectionFull.GetBinError(1) )

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


        if self.Sample == 'TTBarSummer2011' or self.Sample.find('SysTT') >= 0:
            self.CosTheta2D = EE.CosTheta2D.Clone( 'CosTheta2DGenRec_10_Combined_' + self.Sample )
            self.CosTheta2D.Add( MM.CosTheta2D )
            self.CosTheta2D.Add( EM.CosTheta2D )

            self.CosTheta2DOriginal = EE.CosTheta2DOriginal.Clone( 'CosTheta2DGenRec_Original_Combined_' + self.Sample )
            self.CosTheta2DOriginal.Add( MM.CosTheta2DOriginal )
            self.CosTheta2DOriginal.Add( EM.CosTheta2DOriginal )

            if EE.HasFullCosThetaInfo:
                self.hCosThetaNeg_vs_Pos_Gen = EE.hCosThetaNeg_vs_Pos_Gen.Clone( 'hCosThetaNeg_vs_Pos_Gen_Combined_' + self.Sample )
                self.hCosThetaNeg_vs_Pos_Gen.Add( MM.hCosThetaNeg_vs_Pos_Gen )
                self.hCosThetaNeg_vs_Pos_Gen.Add( EM.hCosThetaNeg_vs_Pos_Gen )
                
                self.hCosThetaNeg_vs_Pos_TTBar = EE.hCosThetaNeg_vs_Pos_TTBar.Clone( 'hCosThetaNeg_vs_Pos_TTBar_Combined_' + self.Sample )
                self.hCosThetaNeg_vs_Pos_TTBar.Add( MM.hCosThetaNeg_vs_Pos_TTBar )
                self.hCosThetaNeg_vs_Pos_TTBar.Add( EM.hCosThetaNeg_vs_Pos_TTBar )

                self.hCosThetaVSTopEta_Gen = EE.hCosThetaVSTopEta_Gen.Clone( 'hCosThetaVSTopEta_Gen_Combined_' + self.Sample )
                self.hCosThetaVSTopEta_Gen.Add( MM.hCosThetaVSTopEta_Gen )
                self.hCosThetaVSTopEta_Gen.Add( EM.hCosThetaVSTopEta_Gen )
            

        self.hCosTheta = {}

        for cosTheta in EE.hCosTheta.keys():            
            self.hCosTheta[cosTheta] = EE.hCosTheta[cosTheta].Clone('costheta'+ cosTheta +'_combined_' + Sample)
            self.hCosTheta[cosTheta].Add( MM.hCosTheta[cosTheta] )
            self.hCosTheta[cosTheta].Add( EM.hCosTheta[cosTheta] )
        
        
        self.hCosThetaPreselected = EE.hCosThetaPreselected.Clone('prescaled_costheta_combined_' + Sample)
        self.hCosThetaPreselected.Add( MM.hCosThetaPreselected )
        self.hCosThetaPreselected.Add( EM.hCosThetaPreselected )
        
        self.PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Dilepton_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} , 'PositiveLeptonEta':{} , 'PositiveLeptonPt':{} , 'NegativeLeptonEta':{} , 'NegativeLeptonPt':{} , 'FirstBJetPt':{} , 'SecondBJetPt':{} , 'ThirdBJetPt':{} , 'FirstJetEta':{} , 'SecondJetEta':{} , 'ThirdJetEta':{} , 'DPhi':{} }
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


            if EE.HasFullCosThetaInfo:
                self.hCosThetaNeg_vs_Pos = EE.hCosThetaNeg_vs_Pos.Clone( 'hCosThetaNeg_vs_Pos_Combined_' + self.Sample )
                self.hCosThetaNeg_vs_Pos.Add( MM.hCosThetaNeg_vs_Pos )
                self.hCosThetaNeg_vs_Pos.Add( EM.hCosThetaNeg_vs_Pos )

                self.hCosThetaVSTopEta = EE.hCosThetaVSTopEta.Clone( 'hCosThetaVSTopEta_Combined_' + self.Sample )
                self.hCosThetaVSTopEta.Add( MM.hCosThetaVSTopEta )
                self.hCosThetaVSTopEta.Add( EM.hCosThetaVSTopEta )



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
    def __init__(self , extractAllPlots = True):
        self.Name = 'Data'
        self.Category = 'Data'
        self.Color = SampleCategoriesColors[self.Category]
        self.DataFileNames = {'EE':'DoubleEle2011' , 'MM':'DoubleMuon2011' , 'EM':'ElectronMuon2011'}
        self.DataFullFiles = {}
        self.DataSelectedFiles = {}
        self.AllPlots = {}

        for channel in self.DataFileNames.keys():
            self.DataSelectedFiles[channel] = TFile( FileAddressPattern_Selected % self.DataFileNames[channel] , 'READ')
            self.DataFullFiles[channel] = TFile( FileAddressPattern_Full %  self.DataFileNames[channel] , 'READ')
            
            self.AllPlots[channel] = SampleChannelInfo( channel , self.Name , self.DataFullFiles[channel] , self.DataSelectedFiles[channel] , False , extractAllPlots)

        self.EE = self.AllPlots['EE']
        self.MM = self.AllPlots['MM']
        self.EM = self.AllPlots['EM']
        self.Combined = SampleCombinedInfo( self.Name , self.EE , self.EM , self.MM , extractAllPlots)
        self.AllPlots['Combined'] = self.Combined

    def __del__(self):
        for filename in self.DataFullFiles.keys():
            self.DataFullFiles[filename].Close()
            self.DataSelectedFiles[filename].Close()

class SamplesStack:
    def DrawAndSave(self, stack , hist , name , LogY = 0 , external_title = '' , errors_to_draw=[]):
        #if not name == "bbbTopTopEta" :
        #    return 0.0

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
            if not len(errors_to_draw) == 0 :
                maxVal *= 1.4

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
        hist.GetYaxis().SetTitle('')
        hist.Draw("9 E1 SAME")

        leg = TLegend(0.75,0.75,0.95,0.95)
        leg.SetBorderSize( 0 )
        leg.SetTextFont(62)
        leg.SetLineColor(1)
        leg.SetLineStyle(1)
        leg.SetLineWidth(1)
        leg.SetFillColor(0)
        leg.SetFillStyle(0)

        for category in SampleCategoriesOrdered:
            if category == "Systematics" :
                continue

            color = SampleCategoriesColors[category]
            leg_entry_name = "%(cat)s_%(color)d" % {"cat":"category" , "color":color}
            print leg_entry_name
            entry = None
            if category == "Data":
                entry=leg.AddEntry(leg_entry_name,category,"PLE")
            else:
                entry=leg.AddEntry(leg_entry_name,category,"F")
            
            entry.SetLineColor(color)
            entry.SetLineStyle(1)
            entry.SetLineWidth(1)
            entry.SetMarkerColor(color)
            entry.SetMarkerStyle(21)
            entry.SetMarkerSize(1)
            entry.SetFillColor(color)
            entry.SetFillStyle(1001)

        c1.SetPad( 0 , 0.295 , 1, 1)
        c1.cd()
        leg.SetOption("BR NDC")
        leg.ConvertNDCtoPad()
        leg.Draw()

        latex = TLatex( 0.15 , 0.91 , "CMS Preliminary, 4.6 fb^{-1} at #sqrt{s} =7TeV" )
        c1.cd()
        latex.SetNDC(True)
        latex.Draw()
        c1.Modified()
        

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

        xx_for_errors = array.array('d')
        yy_for_errors = array.array('d')
        xxerr_for_errors = array.array('d')
        yyerr_for_errors = array.array('d')

        for h_MC in stack.GetHists():
            #h_MC.Print()
            hSum.Add( h_MC )
        
        for nBin in range(1, hSum.GetNbinsX()+1):
            Errors[nBin] = hSum.GetBinError(nBin)
            if not len(errors_to_draw) == 0:
                xx_for_errors.append( hSum.GetBinCenter(nBin) )
                yy_for_errors.append( hSum.GetBinContent(nBin) )
                yyerr_for_errors.append( errors_to_draw[nBin-1] )
                xxerr_for_errors.append( hSum.GetBinWidth(nBin)/2 )

                Errors[nBin] = sqrt( errors_to_draw[nBin-1]*errors_to_draw[nBin-1] + hSum.GetBinError(nBin)*hSum.GetBinError(nBin) )
        
        if not len(errors_to_draw) == 0:
            err_graph = TGraphErrors( hSum.GetNbinsX() , xx_for_errors , yy_for_errors ,  xxerr_for_errors ,  yyerr_for_errors)
            c.cd(1)
            err_graph.SetFillColor(kBlack)
            err_graph.SetFillStyle(3005)
            err_graph.Draw('2')

            entry=leg.AddEntry("Systematics_Uncertainity","Syst. Unc.","F")
            entry.SetFillColor(kBlack)
            entry.SetFillStyle(3005)

            c.cd(2)

        for nBin in range(1, hSum.GetNbinsX()+1):
            hSum.SetBinError( nBin , Errors[nBin] )

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
        hDivision.Draw("E1")
        hOne.Draw("SAME C")

        total_difference = 0.0
        for nBin in range(1, hist.GetNbinsX()+1):
            data_in_bin = hist.GetBinContent(nBin)
            mc_in_bin = hSum.GetBinContent(nBin)
            total_difference += (data_in_bin - mc_in_bin)

        c.cd()
        c.Modified()
        c.SaveAs(self.Channel + '/' + name + '.gif' )
        c.SaveAs(self.Channel + '/' + name + '.C' )
        c.SaveAs(self.Channel + '/' + name + '.pdf' )
        # c.SaveAs(self.Channel + '/' + name + '.eps' )
        c.Close()
        return total_difference    

    def DrawCosTheta(self , Data , Errors = []):
        dataType = type( Data )
        data = dataType. __getattribute__(Data , self.Channel)

        NNName = '_cos_theta'
        if not len(Errors) == 0:
            NNName += '_errors'

        deltaMC_Data_CosTheta = self.DrawAndSave( self.stack_costheta['10'] ,  data.hCosTheta['10'] , self.Channel + NNName , 0 , 'cos(#theta^{*})' , Errors )
        deltaMC_Data_CosTheta_LogY = self.DrawAndSave( self.stack_costheta['10'] ,  data.hCosTheta['10'] , self.Channel +NNName + '_logy' , 1 , 'cos(#theta^{*})' , Errors )

        ret = '*** Plot of Cos(\\theta) for selected events \n'
        ret = ret + '    - Difference is : 2*' + str(deltaMC_Data_CosTheta/2) + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s%(NNName)s.gif]]]'%{'Channel':self.Channel, 'NNName':NNName}  + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s%(NNName)s_logy.gif]]]'%{'Channel':self.Channel, 'NNName':NNName}  + '\n'
        if len(Errors) == 0:
            deltaMC_Data_CosThetaPreselected = self.DrawAndSave( self.stack_costheta_preselected ,  data.hCosThetaPreselected , self.Channel + '_cos_theta_preselected',1, 'cos(#theta^{*})' )            
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
        for category in reversed(SampleCategoriesOrdered):
            for sampleName in sortedSamples:
                if ArrayOfAllSamples.keys().count(sampleName) == 0:
                    continue
                sample = ArrayOfAllSamples[sampleName]

                if category.count( sample.Category ) == 0:
                    continue
                
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
