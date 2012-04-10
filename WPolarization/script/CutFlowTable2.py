#! /usr/bin/python

from ROOT import TFile, TTree, THStack, TCanvas, TH1D, gROOT
from Table import *

from math import sqrt
import os

org_file = open("TablesDataMC.org" , 'w')

RunOnSelectedEvents = True

#TO ENABLE JS IN THE HTML OUTPUT 
print >>org_file,'#+INFOJS_OPT: view:content toc:nil' #http://orgmode.org/manual/JavaScript-support.html#JavaScript-support
print >>org_file,'#+TITLE: WPolarization, data/mc comparison'
#needed lines for hs.expand command works
print >>org_file,'#+STYLE: <script type="text/javascript" src="highslide/highslide.js"></script>\n\
#+STYLE: <link rel="stylesheet" type="text/css" href="highslide/highslide.css" /> \n\
#+STYLE: <script type="text/javascript">\n\
#+STYLE:    hs.graphicsDir = "./";\n\
#+STYLE:    hs.outlineType = null;\n\
#+STYLE:    hs.wrapperClassName = "colored-border";\n\
#+STYLE: </script>\n'
# download highslide from http://highslide.com/ 

WhichChannel = ''
def DrawAndSave( stack , hist , name):
    
    if not os.access( WhichChannel , os.F_OK) :
        os.mkdir( WhichChannel )

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

    stack.Draw()

    stack.GetHistogram().GetXaxis().SetTitle( hist.GetXaxis().GetTitle() )

    hist.Draw("E1 SAME")

    c1.BuildLegend(0.8,0.8,1,1)
    c1.SetPad( 0 , 0.2 , 1, 1)

    c2 =  c.cd(2)
    c2.SetPad( c2.GetXlowNDC(), c2.GetXlowNDC() , 1 , 0.2)

    hDivision = TH1D('hDivision' , 'Data/MC' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )
    hSum = TH1D('hSum' , 'SUM_MC' , hist.GetNbinsX() , hist.GetBinLowEdge(1) , hist.GetBinLowEdge(hist.GetNbinsX())+hist.GetBinWidth(hist.GetNbinsX()) )

    #hSum.Print("ALL")
    #hist.Print("ALL")

    for h_MC in stack.GetHists():
        hSum.Add( h_MC )

    hDivision.Divide( hist , hSum )
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
    c.SaveAs(WhichChannel + '/' + name + '.gif' )
    c.Close()
    return total_difference    


for WhichChannel_ in ['EM' , 'MM' , 'EE']:

    WhichChannel = WhichChannel_

    #FILES and XSections
    sorted_samples = ['TTBarSummer2011', 'DYSummer2011' ,'SingleTopTWSummer2011', 'WJetsSummer2011' , 'WWSummer2011' , 'SingleTopSummer2011' , 'WZSummer2011', 'ZZSummer2011' ]
    sorted_samples.reverse()
    files_xsec = { 'TTBarSummer2011':157.5, 'DYSummer2011':3048.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 , 'WZSummer2011':0.6 , 'ZZSummer2011':4.65 }

    files_ee_weights = { 'TTBarSummer2011':0.192709, 'DYSummer2011':0.475306 ,'WJetsSummer2011':1.896160 , 'WWSummer2011':0.017588 , 'SingleTopSummer2011':0.077388 , 'SingleTopTWSummer2011':0.090660 , 'ZZSummer2011':0.005090 ,'WZSummer2011':0.002272  }
    files_mm_weights = { 'TTBarSummer2011':0.189709, 'DYSummer2011':0.467907 ,'WJetsSummer2011':1.866643 , 'WWSummer2011':0.017314 , 'SingleTopSummer2011':0.076184 , 'SingleTopTWSummer2011':0.089249 , 'ZZSummer2011':0.005011 ,'WZSummer2011':0.002237 }
    files_em_weights = { 'TTBarSummer2011':0.197058, 'DYSummer2011':0.486031 ,'WJetsSummer2011':1.938946 , 'WWSummer2011':0.017985 , 'SingleTopSummer2011':0.079134 , 'SingleTopTWSummer2011':0.092706 , 'ZZSummer2011':0.005205 ,'WZSummer2011':0.002323  }

    data_file = ''
    DirectoryNameForSteps = []
    SF_OF_Value = ''
    integrated_lumi = 0
    EventTypeMinVal = 0
    EventTypeMaxVal = 0

    if WhichChannel=='EM' :
        data_file = 'ElectronMuon2011'
        integrated_lumi = 4631.724
        SF_OF_Value = 'OppositeFlavours'
        DirectoryNameForSteps = ['None', 'None', 'None' , '' , '_NJets' , '_MET' , '_NumberOfBJets']
        EventTypeMinVal = 5
        EventTypeMaxVal = 8
        if RunOnSelectedEvents:
            files_xsec = files_em_weights
            DirectoryNameForSteps = ['None', 'None', 'None' , 'None' , 'None' , 'None' , '_NumberOfBJets']
    elif WhichChannel=='EE' :
        data_file ='DoubleEle2011'
        integrated_lumi = 4529.518
        SF_OF_Value = 'SameFlavours'
        DirectoryNameForSteps = ['None', 'None', 'None' , '' , '_InvMass12' , '_InvMassZ', '_NJets', '_MET', '_NumberOfBJets']
        EventTypeMinVal = 1
        EventTypeMaxVal = 2
        if RunOnSelectedEvents:
            files_xsec = files_ee_weights
            DirectoryNameForSteps = ['None', 'None', 'None' , 'None' , 'None' , 'None', 'None', 'None', '_NumberOfBJets']
    elif WhichChannel=='MM' :
        data_file = 'DoubleMuon2011'
        integrated_lumi = 4459.007
        SF_OF_Value = 'SameFlavours'
        DirectoryNameForSteps = ['None', 'None', 'None' , '' , '_InvMass12' , '_InvMassZ', '_NJets', '_MET', '_NumberOfBJets']
        EventTypeMinVal = 3
        EventTypeMaxVal = 4
        if RunOnSelectedEvents:
            files_xsec = files_mm_weights
            DirectoryNameForSteps = ['None', 'None', 'None' , 'None' , 'None' , 'None', 'None', 'None', '_NumberOfBJets']
        #Properties
    PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Electrons_InvariantMass':{}, 'JetsHT':{} }

    t_PropertiesLinks = Table(False)
    PropertiesLinks = {}
    for Property in PropertiesToDraw.keys():
        PropertiesLinks[Property] = RowObject()
        t_PropertiesLinks.append( PropertiesLinks[Property] )
        
    PropertiesHistoPathFormat = 'Selection/EventSelectionHistosAfterObjectCreation/%(SF_OF)s%(Folder)s/%(SF_OF)s%(Folder)s_DiLeptonEvent_EventTypevs%(Property)s'

    cosThetaPlotName = 'costheta_%(channel)s/hCosThetaAllLepton' % {'channel':WhichChannel.lower()}
    stack_costheta = THStack("stackCosTheta",'CosTheta for ' + WhichChannel +' Events')
        
    selectionPlotName = 'Selection/hEventSelection%(channel)s' % {'channel':WhichChannel}

        #STYLE INFO
    colors = {'TTBarSummer2011':41, 'DYSummer2011':46 ,'WJetsSummer2011':31 , 'WWSummer2011':29 , 'SingleTopSummer2011':4 , 'SingleTopTWSummer2011':7 , 'WZSummer2011':90 , 'ZZSummer2011':66 }


    t_mc = Table()
    t_mc_w = Table()
        
    print >>org_file,'* ' + WhichChannel
    print >>org_file,"** Wights"
    last_column_name = ''
    last_column_name_w = ''

    for wpol_file in sorted_samples:
        file = 0
        if RunOnSelectedEvents:
            file = TFile( '/home/hbakhshi/Documents/Analysis/Run/WPolarization/WPol_SelectedTTBars_%s.root' % wpol_file , "READ")
        else:
            file = TFile( 'WPol_%s.root' % wpol_file , "READ")

        r = RowObject()
        r_w = RowObject()
            
        colName = '%(colIndex)02d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
        r[colName] = wpol_file
        r_w[colName] = wpol_file
        last_column_name = colName
        
    
        hSelectionW = file.Get( selectionPlotName + 'W' )

        hSelection = file.Get( selectionPlotName )

        lumi_weight = 0
        if RunOnSelectedEvents:
            lumi_weight = files_xsec[ wpol_file ]
        else:
            nAll = hSelection.GetBinContent(1)
            lumi_weight = files_xsec[ wpol_file ]*integrated_lumi /  nAll

        print >>org_file, '  |%(file)s | %(weight)f' % {'file': wpol_file , 'weight' : lumi_weight}
        
        for cut in range(1,hSelection.GetNbinsX()+1) :
            cut_name=hSelection.GetXaxis().GetBinLabel(cut)
            colName = '%(colIndex)02d-%(step)s' % {'colIndex': cut , 'step': cut_name}
            nPassed = hSelection.GetBinContent(cut)*lumi_weight
            
            cut_folder_name = DirectoryNameForSteps[cut-1]        
            r[colName] = nPassed
            
            nPassedW = hSelectionW.GetBinContent(cut)*lumi_weight
            
            r_w[colName] = nPassedW
            #print wpol_file + ' ' + colName + '  ' + str( r_w[colName] )
            last_column_name = colName
            
            if not cut_folder_name == 'None':
                for property_name in PropertiesToDraw.keys():
                    hProp_path =  PropertiesHistoPathFormat % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name }
                    hProp2 = file.Get( hProp_path )
                    hProp_newName = '%(File)s_%(SF_OF)s%(Folder)s_DiLeptonEvent_%(Property)s' % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name , 'File':wpol_file}
                    gROOT.cd()
                    hProp = hProp2.ProjectionY(hProp_newName, EventTypeMinVal, EventTypeMaxVal, "o" )
                    hProp.Scale( lumi_weight )
                    hProp.SetTitle( wpol_file )
                    hProp.SetFillStyle(3004)
                    hProp.SetFillColor(colors[wpol_file])
                    hProp.SetLineColor(colors[wpol_file])
                    hTemp = hProp.Clone()
                    if PropertiesToDraw[property_name].keys().count(cut_folder_name)==0:
                        PropertiesToDraw[property_name][cut_folder_name] = THStack(WhichChannel + '_' + cut_folder_name + '_' + property_name, WhichChannel + '_' + cut_folder_name + '_' + property_name)
                    PropertiesToDraw[property_name][cut_folder_name].Add( hTemp )

        hCosTheta = (file.Get(cosThetaPlotName))
        hCosTheta.Scale(lumi_weight)
        last_column_name_w = '%(colIndex)02d-%(step)s' % {'colIndex':hSelection.GetNbinsX()+1  , 'step': 'nTTBars'}
        r_w[ last_column_name_w  ] = 0
        for bin_cos_theta in range(1, hCosTheta.GetNbinsX()+1):
            r_w[ last_column_name_w  ] += hCosTheta.GetBinContent( bin_cos_theta )/2
        hCosTheta.SetTitle(wpol_file)
        
        hCosTheta.SetFillStyle(3004)
        hCosTheta.SetFillColor(colors[wpol_file])
        hCosTheta.SetLineColor(colors[wpol_file])
        gROOT.cd()
        hTemp = hCosTheta.Rebin(10, "costheta_"+wpol_file)
        stack_costheta.Add(hTemp)

        t_mc_w.append(r_w)
        t_mc.append(r)
        file.Close()


    print >>org_file,"** CutFlow table"
    print >>org_file,t_mc
    sumRow = t_mc.GetSumRow()
    last_sum = sumRow[last_column_name]
    nCols = len(t_mc.rows[0])
    
    print >>org_file,"    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

    print >>org_file,"** CutFlow table, Weighted"
    print >>org_file,t_mc_w
    sumRow_w = t_mc_w.GetSumRow()
    #print sumRow_w
    last_sum = sumRow_w[last_column_name_w]
    nCols = len(t_mc_w.rows[0])
        
    tbl_mc_weighted_formula_str = "    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

    t_data = Table(False, False)

    file = TFile( 'WPol_%s.root' % data_file , "READ")
    r = RowObject()

    colName = '%(colIndex)02d-%(step)s' % {'colIndex':0  , 'step': 'DataSet'}    
    r[colName] = data_file
    for property_name in PropertiesToDraw.keys():
        PropertiesLinks[property_name][colName] = property_name
                
    hSelection = file.Get( selectionPlotName )
    for cut in range(1,hSelection.GetNbinsX()+1) :
        cut_name=hSelection.GetXaxis().GetBinLabel(cut)
        colName = '%(colIndex)02d-%(step)s' % {'colIndex': cut , 'step': cut_name}
        nPassed = hSelection.GetBinContent(cut)
        r[colName] = nPassed
        cut_folder_name = DirectoryNameForSteps[cut-1]
        for property_name in PropertiesToDraw.keys():
            if not cut_folder_name == 'None':
                hProp_path =  PropertiesHistoPathFormat % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name } 
                hProp2d = file.Get( hProp_path )
                hProp_newName = '%(File)s_%(SF_OF)s%(Folder)s_DiLeptonEvent_%(Property)s' % {'SF_OF':SF_OF_Value , 'Folder':cut_folder_name , 'Property':property_name , 'File':wpol_file}
                gROOT.cd()
                hPropd = hProp2d.ProjectionY(hProp_newName, EventTypeMinVal, EventTypeMaxVal, "o" )
                hPropd.SetTitle(data_file)
                hPropd.SetFillStyle(0)
                hPropd.SetLineColor(1)
                hPropd.SetStats(0)
                
                stack_prop = PropertiesToDraw[property_name][cut_folder_name]
                if cut_folder_name == '':
                    stack_prop.SetTitle( '%(Channel)s : after pair choose' % {'Channel':WhichChannel } )
                else:
                    stack_prop.SetTitle( '%(Channel)s : after cut on  %(Cut)s' % {'Channel':WhichChannel, 'Cut':cut_name  } )
                    
                ImgFileName =  str(cut)+ "_"+ cut_folder_name + "_" + property_name 
                ImgFileName = ImgFileName.replace("_", 'z')
                TotalDiff = DrawAndSave(stack_prop, hPropd, ImgFileName)
                               
                PropertiesLinks[property_name][colName] = '@<a id="%(id)s" href="%(file)s" class="highslide" onclick="return hs.expand(this)">%(Diff)d@</a>' % {'file':WhichChannel + '/' + ImgFileName + '.gif' , 'id':(cut_folder_name+'_'+property_name).replace('_', 'x') , 'Diff':TotalDiff}
            else:
                PropertiesLinks[property_name][colName] = ''

    hCosTheta = (file.Get(cosThetaPlotName))
    
    r[ '%(colIndex)02d-%(step)s' % {'colIndex':hSelection.GetNbinsX()+1  , 'step':'nTTBars'}  ] = 0
    for bin_cos_theta in range(1, hCosTheta.GetNbinsX()+1):
        r[ '%(colIndex)02d-%(step)s' % {'colIndex':hSelection.GetNbinsX()+1  , 'step':'nTTBars'}  ] += hCosTheta.GetBinContent( bin_cos_theta )/2
        
    hCosTheta.SetTitle(data_file)
    hCosTheta.SetFillStyle(0)
    hCosTheta.SetLineColor(1)
    
    hCosTheta.Rebin(10)
    
    DeltaMC_Data_CosTheta = DrawAndSave( stack_costheta , hCosTheta , WhichChannel +  '_cos_theta')
        
    t_data.append(r)
    file.Close()

    print >>org_file,t_data

    t_data_mc_diff = Table(False,False)
    r_data_mc_diff = RowObject()
        
    for column in r.keys():
        if column.find( 'DataSet' ) > -1:
            r_data_mc_diff[column] = 'Data-MC'
        else:
            r_data_mc_diff[column] = r[column] - sumRow_w[column]
        
    t_data_mc_diff.append(r_data_mc_diff)
    
    print >>org_file,t_data_mc_diff

    t_weight_effect = Table(False,False)
    r_weight_effect = RowObject()
        
    for column in sumRow.keys():
        if column.find( 'DataSet' ) > -1:
            r_weight_effect[column] = 'Weight-Effect'
        else:
            r_weight_effect[column] = sumRow_w[column]/sumRow[column]
        
    t_weight_effect.append(r_weight_effect)
    
    print >>org_file,t_weight_effect
    print >> org_file, tbl_mc_weighted_formula_str

    print >>org_file, '** Plot of Cos(\theta)'
    print >> org_file, '   - Difference is : 2*' + str(DeltaMC_Data_CosTheta/2)
    print >> org_file, '   [[[%(Channel)s/%(Channel)s_cos_theta.gif]]]'%{'Channel':WhichChannel}

    print >>org_file,'** Control Plots'
    print >>org_file,t_PropertiesLinks


org_file.close()
