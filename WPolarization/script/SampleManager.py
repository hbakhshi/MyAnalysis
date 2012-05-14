from ROOT import TFile, TTree, THStack, TCanvas, TH1D, gROOT
from Table import *

import copy,os
from math import sqrt

FileAddressPattern_Selected = '/home/hbakhshi/Documents/Analysis/Run/WPolarization/WPol_SelectedTTBars_%s.root'
FileAddressPattern_Full = '/home/hbakhshi/Documents/WPolarization/WPol/WPol_%s.root'

PropertiesHistoPathFormat = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s%(Folder)s/%(SF_OF)s%(Folder)s_DiLeptonEvent_EventTypevs%(Property)s'
SelectionPlotName = 'Selection/hEventSelection%(channel)s'
CosThetaPlotName = 'costheta_%(channel)s/hCosThetaAllLepton'

XSections = { 'TTBarSummer2011':157.5, 'DYSummer2011':3048.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 , 'WZSummer2011':0.6 , 'ZZSummer2011':4.65 , 'SysWJetsQU':31314.0 , 'SysTTQU':157.5 , 'SysTTQD':157.5 , 'SysZJetsQD':3048.0 , 'SysZJetsQU':3048.0 , 'SysTTM175':157.5 , 'SysWJetsQD':31314.0 , 'SysTTM169':157.5 }

Colors = {'TTBarSummer2011':41, 'DYSummer2011':46 ,'WJetsSummer2011':31 , 'WWSummer2011':29 , 'SingleTopSummer2011':4 , 'SingleTopTWSummer2011':7 , 'WZSummer2011':90 , 'ZZSummer2011':66 ,  'Data':1  , 'SysWJetsQU':1 , 'SysTTQU':1 , 'SysTTQD':1, 'SysZJetsQD':1 , 'SysZJetsQU':1 , 'SysTTM175':1, 'SysWJetsQD':1 , 'SysTTM169':1 }

IntLumis = {'EE':4529.518 , 'MM':4459.007 , 'EM':4631.724}


class SampleChannelInfo:
    def SaveAllPlots():
        pass

    def __init__(self, channel, Sample,  FileFull, FileSelected , applyWeight = True):
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
        self.PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Electrons_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} }
        
        if applyWeight :
            self.Lumi_Weight = XSections[Sample]*self.integrated_lumi  / self.hSelectionFull.GetBinContent(1)
        else:
            self.Lumi_Weight = 1.0
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
                    self.PropertiesToDraw[property_name][cut_folder_name].SetTitle( Sample )
                    self.PropertiesToDraw[property_name][cut_folder_name].SetFillColor(Colors[Sample])
                    self.PropertiesToDraw[property_name][cut_folder_name].SetLineColor(Colors[Sample])
                    self.PropertiesToDraw[property_name][cut_folder_name].SetLineStyle(1)
                    self.PropertiesToDraw[property_name][cut_folder_name].SetFillStyle(3004)

        CosTheta = FileSelected.Get( CosThetaPlotName % {'channel':channel.lower()} )
        CosTheta.Scale(self.Lumi_Weight)
        last_column_name_w = '%(colIndex)02d-%(step)s' % {'colIndex':self.hSelectionFull.GetNbinsX()+1  , 'step': 'nTTBars'}
        self.RowW[ last_column_name_w  ] = 0
        for bin_cos_theta in range(1, CosTheta.GetNbinsX()+1):
            self.RowW[ last_column_name_w  ] += CosTheta.GetBinContent( bin_cos_theta )/2
        CosTheta.SetTitle(Sample)
        
        CosTheta.SetFillStyle(3004)
        CosTheta.SetFillColor(Colors[Sample])
        CosTheta.SetLineColor(Colors[Sample])
        gROOT.cd()
        self.hCosTheta = CosTheta.Rebin(10, "costheta_"+channel+"_" + Sample)

        CosTheta = FileFull.Get( CosThetaPlotName % {'channel':channel.lower()} )
        CosTheta.Scale(self.Lumi_Weight)
        CosTheta.SetTitle(Sample)
        
        CosTheta.SetFillStyle(3004)
        CosTheta.SetFillColor(Colors[Sample])
        CosTheta.SetLineColor(Colors[Sample])
        gROOT.cd()
        self.hCosThetaPreselected = CosTheta.Rebin(10, "preselected_costheta_"+channel+"_" + Sample)

        self.LastColumn = last_column_name
        self.LastColumnW = last_column_name_w

class SampleCombinedInfo:
    def __init__(self, Sample, EE , EM, MM):
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

        self.hCosTheta = EE.hCosTheta.Clone('costheta_combined_' + Sample)
        self.hCosTheta.Add( MM.hCosTheta )
        self.hCosTheta.Add( EM.hCosTheta )
        
        self.hCosThetaPreselected = EE.hCosThetaPreselected.Clone('prescaled_costheta_combined_' + Sample)
        self.hCosThetaPreselected.Add( MM.hCosThetaPreselected )
        self.hCosThetaPreselected.Add( EM.hCosThetaPreselected )
        
        self.PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Electrons_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} }
        self.AllSortedDirectories = {}

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

class SampleInfo:
    def __init__(self , sampleName):
        self.Name = sampleName
        self.FileSelected = TFile( FileAddressPattern_Selected % sampleName , 'READ')
        self.FileFull = TFile( FileAddressPattern_Full % sampleName , 'READ')

        
        self.EE = SampleChannelInfo( 'EE' , self.Name , self.FileFull , self.FileSelected )
        self.MM = SampleChannelInfo( 'MM' , self.Name , self.FileFull , self.FileSelected )
        self.EM = SampleChannelInfo( 'EM' , self.Name , self.FileFull , self.FileSelected )

        self.Combined = SampleCombinedInfo( self.Name , self.EE , self.EM , self.MM )

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
    def DrawAndSave(self, stack , hist , name):

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
        c.Divide(1,2)

        c1 = c.cd(1)
        if stack.GetMaximum() < hist.GetBinContent(hist.GetMaximumBin()):
            stack.SetMaximum( 1.01*hist.GetBinContent(hist.GetMaximumBin() ) )

        stack.Draw("9 HIST")

        stack.GetHistogram().GetXaxis().SetTitle( hist.GetXaxis().GetTitle() )

        hist.SetLineWidth( 2 )
        hist.Draw("9 E1 SAME")

        c1.BuildLegend(0.8,0.8,1,1)
        c1.SetPad( 0 , 0.2 , 1, 1)

        c2 =  c.cd(2)
        c2.SetPad( c2.GetXlowNDC(), c2.GetXlowNDC() , 1 , 0.2)

        hDivision = TH1D('hDivision' , 'Data/MC' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )
        hSum = TH1D('hSum' , 'SUM_MC' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )

        #hSum.Print("ALL")
        #hist.Print("ALL")

        Errors = {}
        for nBin in range(1, hist.GetNbinsX()+1):
            Errors[nBin] = 0.0

        for h_MC in stack.GetHists():
            hSum.Add( h_MC )
            for nBin in range(1, h_MC.GetNbinsX()+1):
                Errors[nBin] += 0 #(h_MC.GetBinContent(nBin)*h_MC.GetBinContent(nBin))

        for nBin in range(1, hSum.GetNbinsX()+1):
            hSum.SetBinError( nBin , sqrt(Errors[nBin]) )

        hDivision.Divide( hist , hSum )
        hDivision.GetYaxis().SetRangeUser( 0.5 , 1.5)
        hDivision.SetStats(0)
        hDivision.SetLineWidth(2)
        hDivision.SetLineColor(2)
        hDivision.Draw()
        hOne.Draw("SAME E1")

        total_difference = 0.0
        for nBin in range(1, hist.GetNbinsX()+1):
            data_in_bin = hist.GetBinContent(nBin)
            mc_in_bin = hSum.GetBinContent(nBin)
            total_difference += (data_in_bin - mc_in_bin)

        c.cd()
        c.SaveAs(self.Channel + '/' + name + '.gif' )
        c.Close()
        return total_difference    

    def DrawCosTheta(self , Data):
        dataType = type( Data )
        data = dataType. __getattribute__(Data , self.Channel)
        
        deltaMC_Data_CosTheta = self.DrawAndSave( self.stack_costheta ,  data.hCosTheta , self.Channel + '_cos_theta')
        deltaMC_Data_CosThetaPreselected = self.DrawAndSave( self.stack_costheta_preselected ,  data.hCosThetaPreselected , self.Channel + '_cos_theta_preselected')
        ret = '*** Plot of Cos(\\theta) for selected events \n'
        ret = ret + '    - Difference is : 2*' + str(deltaMC_Data_CosTheta/2) + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s_cos_theta.gif]]]'%{'Channel':self.Channel}  + '\n'
        ret = ret +  '*** Plot of Cos(\\theta) for pre-selected events \n'
        ret = ret + '    - Difference is : 2*' + str(deltaMC_Data_CosThetaPreselected/2)  + '\n'
        ret = ret + '    [[[%(Channel)s/%(Channel)s_cos_theta_preselected.gif]]]'%{'Channel':self.Channel} + '\n'
        return ret

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
                TotalDiff = self.DrawAndSave(stack_prop, hPropd, ImgFileName)
                               
                PropertiesLinks[Property]['%(ID)d-%(Name)s'%{'ID':CutID+1 , 'Name':Cut}] = '@<a id="%(id)s" href="%(file)s" class="highslide" onclick="return hs.expand(this)">%(Diff)d@</a>' % {'file':self.Channel + '/' + ImgFileName + '.gif' , 'id':(Cut+'_'+Property).replace('_', 'x') , 'Diff':TotalDiff}
                CutID = CutID+1

            self.t_PropertiesLinks.append( PropertiesLinks[Property] )
        return self.t_PropertiesLinks

        

    def __init__(self , ArrayOfAllSamples):
        self.Channel = ArrayOfAllSamples[0].Channel
        self.AllSortedDirectories = ArrayOfAllSamples[0].AllSortedDirectories

        self.Row  = RowObject()
        self.RowW = RowObject()
        
        self.stack_costheta =  THStack("stackCosTheta_" + self.Channel ,'CosTheta for ' + self.Channel +' Events')
        self.stack_costheta_preselected =  THStack("preselected_stackCosTheta_" + self.Channel ,'CosTheta for preselected ' + self.Channel +' Events')
        self.PropertiesToDraw = {}

        sample_id = 0
        for sample in ArrayOfAllSamples:
            #print sample.Sample
            if sample_id == 0:
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
                sample_id = 1
            else:
                for column in sorted(self.Row.keys())[1:]:
                    self.Row[column] = self.Row[column] + sample.Row[column]
                for column in sorted(self.RowW.keys())[1:]:
                    self.RowW[column] = self.RowW[column] + sample.RowW[column]

                for Property in sample.PropertiesToDraw.keys():
                    for Cut in sample.PropertiesToDraw[Property].keys():
                        self.PropertiesToDraw[Property][Cut].Add( sample.PropertiesToDraw[Property][Cut] )

            self.stack_costheta.Add( sample.hCosTheta )
            self.stack_costheta_preselected.Add( sample.hCosThetaPreselected )
