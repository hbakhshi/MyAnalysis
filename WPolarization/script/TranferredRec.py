#! /usr/bin/python

from ROOT import TH1D, TH2D, TStyle, gStyle, kWhite, kYellow
from SampleManager import *
from Fit import * #weight and dgamma functinos

def Average( l ):
    return ( sum(l) / float(len(l)) )

def DrawAndSave(stack , hist , name , LogY = 0 , external_title = '' , errors_to_draw=[]):

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

            #Errors[nBin] = sqrt( errors_to_draw[nBin-1]*errors_to_draw[nBin-1] + hSum.GetBinError(nBin)*hSum.GetBinError(nBin) )

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
    c.SaveAs(name + '.gif' )
    c.SaveAs(name + '.C' )
    c.SaveAs(name + '.pdf' )
    # c.SaveAs(self.Channel + '/' + name + '.eps' )
    c.Close()
    return total_difference    


def TransferredRec( rec , transfer ):
    
    transfer_rec = transfer.ProjectionX()
    nRecBins = transfer.GetNbinsX()
    nGenBins = transfer.GetNbinsY()
    twoD_allTransRec = TH2D( "htwoD_allTransRec" , "" , nRecBins , -1 , 1 , nGenBins , -1 , 1)
    twoD_allRec = TH2D( "htwoD_allRec" , "" , nRecBins , -1 , 1 , nGenBins , -1 , 1)

    for rec_bin in range(1 , nRecBins + 1):
        val = transfer_rec.GetBinContent( rec_bin )
        err = transfer_rec.GetBinError( rec_bin )

        val_rec = rec.GetBinContent( rec_bin )
        err_rec = rec.GetBinError( rec_bin )
        for gen_bin in range ( 1 , nGenBins + 1 ):
            twoD_allTransRec.SetBinContent( rec_bin , gen_bin , val )
            twoD_allTransRec.SetBinError( rec_bin , gen_bin , err )

            twoD_allRec.SetBinContent( rec_bin , gen_bin , val_rec )
            twoD_allTransRec.SetBinError( rec_bin , gen_bin , err_rec )

    transfer.Divide( twoD_allTransRec )
    transfer.Write()
    transfer.Multiply( twoD_allRec )

    transfer_gen = transfer.ProjectionY()

    return transfer_gen

def ReweightRec(twoD , fL , f0 , norm , method , name):
    WF1 = WeightFunction.GetWeightFunction('W', 0.3 , 0.7)
    WF1.SetParameters(fL, f0 )
    if method == 1:
        oneD = twoD.ProjectionX(name)
        oneD.Multiply( WF1 , norm )
        return oneD
    elif method == 2:
        hWeights = TH1D('%(name)s' % {'name':name} , 'Weighted TTBar' , twoD.GetNbinsX() , -1 , 1)
        for rec_bin in range(1 , twoD.GetNbinsX()+1):
            hprojection = twoD.ProjectionY('%(name)s_%(nbin)d_py' % {'name':name , 'nbin':rec_bin} , rec_bin , rec_bin , 'o')
            hprojection.Multiply( WF1 , norm )
            hWeights.SetBinContent( rec_bin ,  hprojection.Integral() )

        return hWeights
    else:
        return None

WhichChannel = 'Combined'
TTBarSampleName = 'TTBarSummer2011'
sorted_samples = ['DYSummer2011', 'DY10To50Summer2011' ,'SingleTopTWSummer2011', 'WJetsSummer2011' , 'WWSummer2011' , 'SingleTopSummer2011' , 'WZSummer2011', 'ZZSummer2011' ]
SamplesInfo = {}

Data = None
DataCombined = None

TTBarSampleFull = None
TTBarSample = None

gStyle.SetPalette(20 )

WhatToDo = 6 #  0 : TransferredRec , 1 : CosThetaPlots, 2 : CosThetaVSTopEta, 4 : Make1DSmearing Plot, 6 : FinalChi2

for sample in sorted_samples:
    print sample
    SamplesInfo[sample] = SampleInfo(sample, True)

Data = DataInfo(True)
DataCombined = Data.Combined

TTBarSampleFull = SampleInfo( TTBarSampleName , True )
TTBarSample = TTBarSampleFull.Combined


if WhatToDo == 6:
    f = TFile.Open("fit_res.root")
    Data_ = f.Get("hData")
    BKG1 = f.Get("hBKG")
    RecTTBarFIT = f.Get("hRWSignal")
    hErr = f.Get("hErr")

    BKG = None
    for sample in SamplesInfo:
        if BKG == None :
            BKG = SamplesInfo[sample].Combined.hCosTheta[ '10' ]
        else:
            BKG.Add( SamplesInfo[sample].Combined.hCosTheta[ '10' ] )


    BKG.SetTitle("All Backgrounds")
    BKG.SetFillColor( kWhite )
    BKG.SetLineColor( kBlack )

    SystematicErrors = [0,0,0,0,0,0,0,0,0,0]
    #SystematicErrors = [90.59697042396071, 192.23064811574682, 226.70406193978965, 229.8165524935053, 200.57849747667385, 154.48757903145494, 164.76872147042945, 106.78847199019181, 100.0665947007292, 61.810640451300976]

    ErrorsFromFVariation = []
    StatErrorFromData = []
    Errors = []

    Differences = []

    Chi2 = 0
    #print allErrValues
    for bin in range(1 , 11):
        ErrorsFromFVariation.append( hErr.GetBinContent( bin ) )
        StatErrorFromData.append( Data_.GetBinError(bin)*0.9 )

        #Errors.append( sqrt(StatErrorFromData[bin-1]*StatErrorFromData[bin-1] + SystematicErrors[bin-1]*SystematicErrors[bin-1]) )        
        Errors.append( sqrt( SystematicErrors[bin-1]*SystematicErrors[bin-1] + ErrorsFromFVariation[bin-1]*ErrorsFromFVariation[bin-1]) )
        
        Differences.append( abs(RecTTBarFIT.GetBinContent( bin ) + BKG.GetBinContent( bin )  - Data_.GetBinContent(bin) ) )
        
        Chi2 += ( (Differences[bin-1]*Differences[bin-1])/(Errors[bin-1]*Errors[bin-1]) )

    print Chi2
    print "Normalized Chi2 %(chi2)f"  % {'chi2':(Chi2/6)}

    RecTTBarFIT.Add( BKG , -1 )
    RecTTBarFIT.Add( BKG1, +1 )
    st_FIT = THStack("stack_FIT", "")
    st_FIT.Add( BKG )
    st_FIT.Add( RecTTBarFIT )
    DrawAndSave( st_FIT , Data_ , "ReweightedCosTheta" , 0 , '' , Errors)

elif WhatToDo == 0:
    Data_ = DataCombined.hCosTheta[ '10' ]
    TransferMatrix = TTBarSample.CosTheta2D

    BKG = None
    for sample in SamplesInfo:
        if BKG == None :
            BKG = SamplesInfo[sample].Combined.hCosTheta[ '10' ]
        else:
            BKG.Add( SamplesInfo[sample].Combined.hCosTheta[ '10' ] )


    BKG.SetTitle("All Backgrounds")
    BKG.SetLineColor( kYellow-8 )
    BKG.SetMarkerColor( kYellow-8 )

    Data_.SetTitle("Data")
    Data_.SetLineColor( kBlack )
    Data_.SetMarkerColor( kBlack )

    BKGIntegral = BKG.Integral()
    DataIntegral = Data_.Integral()
    TTBarIntegral = TransferMatrix.Integral()

    TTBarWeightIntegral = (DataIntegral - BKGIntegral)/TTBarIntegral

    Data_.SetFillColor( kWhite )
    BKG.SetFillColor( kWhite )

    file = TFile("newHist.root" , "RECREATE")

    WF1 = WeightFunction.GetWeightFunction('W', 0.3 , 0.7)
    canvas = TCanvas("canvas")
    canvas.Divide( 2 , 2 )

    current = canvas.cd(1)
    RecTTBarFL = ReweightRec( TransferMatrix , 1 , 0 , TTBarWeightIntegral , 1 , "_RecTTBar_FL")
    RecTTBarFL.SetTitle( "Signal, reweighted to F_{L}=1" )
    RecTTBarFL.SetMarkerStyle( 20 )
    RecTTBarFL.SetMarkerColor ( kRed )
    RecTTBarFL.SetLineColor( kRed )
    RecTTBarFL.SetFillColor( kWhite )
    #WF1.SetParameters(1, 0 )
    #RecTTBarFL.Multiply( WF1 , TTBarWeightIntegral )

    st_FL = THStack("stack_FL1" , "")
    st_FL.Add( BKG )
    st_FL.Add( RecTTBarFL )
    st_FL.Draw()

    Data_.Draw("SAME")
    leg = current.BuildLegend()
    leg.SetLineColor( kWhite )
    leg.SetFillColor( kWhite )

    current = canvas.cd(2)
    RecTTBarF0 = ReweightRec( TransferMatrix , 0 , 1 , TTBarWeightIntegral , 1 , "_RecTTBar_F0")
    RecTTBarF0.SetTitle( "Signal, reweighted to F_{0}=1" )
    RecTTBarF0.SetMarkerStyle( 20 )
    RecTTBarF0.SetMarkerColor ( kRed )
    RecTTBarF0.SetLineColor( kRed )
    RecTTBarF0.SetFillColor( kWhite )
    #WF1.SetParameters( 0 , 1 )
    #RecTTBarF0.Multiply( WF1, TTBarWeightIntegral )

    Data_.Draw()

    st_F0 = THStack("stack_F01" , "")
    st_F0.Add( BKG )
    st_F0.Add( RecTTBarF0 )
    st_F0.Draw("SAME")
    leg = current.BuildLegend()
    leg.SetLineColor( kWhite )
    leg.SetFillColor( kWhite )

    current = canvas.cd(3)
    RecTTBarFR = ReweightRec( TransferMatrix , 0 , 0 , TTBarWeightIntegral , 1 , "_RecTTBar_FR")
    RecTTBarFR.SetTitle( "Signal, reweighted to F_{R}=1" )
    RecTTBarFR.SetMarkerStyle( 20 )
    RecTTBarFR.SetMarkerColor ( kRed )
    RecTTBarFR.SetLineColor( kRed )
    RecTTBarFR.SetFillColor( kWhite )
    #WF1.SetParameters( 0 , 0 )
    #RecTTBarFR.Multiply( WF1,   TTBarWeightIntegral )

    st_FR = THStack("stack_FR1" , "")
    st_FR.Add( BKG )
    st_FR.Add( RecTTBarFR )
    st_FR.Draw()

    Data_.Draw("SAME")
    leg = current.BuildLegend()
    leg.SetLineColor( kWhite )
    leg.SetFillColor( kWhite )


    current = canvas.cd(4)
    #current = TCanvas("asdf")
    #current.SetGrayscale(True)

    RecTTBarFIT = ReweightRec( TransferMatrix , 0.288 , 0.698 , TTBarWeightIntegral , 1 , "_RecTTBar_FIT")
    #RecTTBarFIT = ReweightRec( TransferMatrix , 0.3 , 0.7 , TTBarWeightIntegral , 1 , "_RecTTBar_FIT")
    RecTTBarFIT.SetTitle( "Signal, reweighted to F_{L,0,R} from fit values" )
    RecTTBarFIT.SetMarkerStyle( 20 )
    RecTTBarFIT.SetMarkerColor ( kRed )
    RecTTBarFIT.SetLineColor( kRed )
    RecTTBarFIT.SetFillColor( kWhite )
    #WF1.SetParameters( 0.288 , 0.698 )
    #RecTTBarFIT.Multiply( WF1  , TTBarWeightIntegral )

    Data_.Draw()

    st_FIT = THStack("stack_FIT" , "")
    st_FIT.Add( BKG )
    RecTTBarFIT0= RecTTBarFIT.Clone("RecTTBarFIT0")
    st_FIT.Add( RecTTBarFIT0 )
    st_FIT.Draw("SAME")
    leg = current.BuildLegend()
    leg.SetLineColor( kWhite )
    leg.SetFillColor( kWhite )

    #current.SaveAs("B.C")
    print TTBarWeightIntegral
    canvas.SaveAs("A.C")

    DataNonTouched = Data_.Clone("DataNonTouched")
    Data_.Add( BKG , -1 )

    RecTTBarFIT.Add( Data_ , -1 )

    FLStat = 0.035
    FLSys = 0 #.050
    FLUncert = sqrt( FLSys*FLSys + FLStat*FLStat )

    F0Stat = 0.057
    F0Sys = 0 #.063
    F0Uncert = sqrt( F0Sys*F0Sys + F0Stat*F0Stat )    

    allErrValues = {}

    RecTTBarFITErr1 = ReweightRec( TransferMatrix , 0.288+FLUncert , 0.698+F0Uncert , TTBarWeightIntegral , 1 , "_RecTTBar_FITErr1")
    RecTTBarFITErr1.Add( Data_ , -1 )
    for bin in range(1, 11):
        allErrValues[bin] = []
        #allErrValues[bin].append( abs(RecTTBarFITErr1.GetBinContent( bin )) )

    RecTTBarFITErr2 = ReweightRec( TransferMatrix , 0.288-FLUncert , 0.698-F0Uncert , TTBarWeightIntegral , 1 , "_RecTTBar_FITErr2")
    RecTTBarFITErr2.Add( Data_ , -1 )
    #for bin in range(1, 11):
    #    allErrValues[bin].append( abs(RecTTBarFITErr2.GetBinContent( bin )) )  

    RecTTBarFITErr3 = ReweightRec( TransferMatrix , 0.288-FLUncert , 0.698+F0Uncert , TTBarWeightIntegral , 1 , "_RecTTBar_FITErr3")
    RecTTBarFITErr3.Add( Data_ , -1 )
    for bin in range(1, 11):
        allErrValues[bin].append( abs(RecTTBarFITErr3.GetBinContent( bin )) )  
        
    RecTTBarFITErr4 = ReweightRec( TransferMatrix , 0.288+FLUncert , 0.698-F0Uncert , TTBarWeightIntegral , 1 , "_RecTTBar_FITErr4")
    RecTTBarFITErr4.Add( Data_ , -1 )
    for bin in range(1, 11):
        allErrValues[bin].append( abs(RecTTBarFITErr4.GetBinContent( bin )) )  

    ErrorsFromFVariation = []
    Differences = []
    StatErrorFromData = []
    SystematicErrors =  [90.59697042396071, 192.23064811574682, 226.70406193978965, 229.8165524935053, 200.57849747667385, 154.48757903145494, 164.76872147042945, 106.78847199019181, 100.0665947007292, 61.810640451300976]
    Errors = []

    Chi2 = 0
    #print allErrValues
    for bin in range(1 , 11):
        ErrorsFromFVariation.append( Average( allErrValues[bin] ) )
        #Errors.append( ErrorsFromFVariation[bin-1] )

        #SystematicErrors.append(0)
        StatErrorFromData.append( DataNonTouched.GetBinError( bin ) )
        Errors.append( sqrt(StatErrorFromData[bin-1]*StatErrorFromData[bin-1] + SystematicErrors[bin-1]*SystematicErrors[bin-1]) )
        
        #Errors.append( sqrt(StatErrorFromData[bin-1]*StatErrorFromData[bin-1] + ErrorsFromFVariation[bin-1]*ErrorsFromFVariation[bin-1]) )
        
        Differences.append( abs(RecTTBarFIT.GetBinContent( bin )) )
        
        Chi2 += ( (Differences[bin-1]*Differences[bin-1])/(Errors[bin-1]*Errors[bin-1]) )
    print Chi2
    print "Normalized Chi2 %(chi2)f"  % {'chi2':(Chi2/6)}
    #print Errors
    #print ErrorsFromFVariation
    DrawAndSave( st_FIT , DataNonTouched , "ReweightedCosTheta" , 0 , '' , ErrorsFromFVariation)

    hTrans = TransferredRec( Data_ , TransferMatrix )
    hTrans.Rebin(10)
    hTrans.SetName("Data")
    hTrans.Write()
    file.Close()
elif WhatToDo == 1:
    cAllPlots = TCanvas("AllPlots")
    cAllPlots.Divide( 2 , 2)
    cAllPlots.SetTitle("cos(#theta) for top and anti-top")

    PLOT = TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos
    #PLOT.Add( TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_TTBar )
    for sample in SamplesInfo:
        print sample
        PLOT.Add( SamplesInfo[sample].Combined.hCosThetaNeg_vs_Pos )
    cAllPlots.cd(1)
    PLOT.SetTitle("MC (signal+bkg)")
    PLOT.RebinX(10)
    PLOT.RebinY(10)
    #PLOT.Draw("COLZ")

    cAllPlots.cd(1)
    Data.Combined.hCosThetaNeg_vs_Pos.SetTitle("Data - BKG")
    Data.Combined.hCosThetaNeg_vs_Pos.RebinX(10)
    Data.Combined.hCosThetaNeg_vs_Pos.RebinY(10)
    Data.Combined.hCosThetaNeg_vs_Pos.Add( PLOT , -1.0 )
    Data.Combined.hCosThetaNeg_vs_Pos.Draw("COLZ")

    cAllPlots.cd(2)
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_TTBar.SetTitle("MC (signal only)")
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_TTBar.RebinX(10)
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_TTBar.RebinY(10)
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_TTBar.Draw("COLZ")

    cAllPlots.cd(3)
    hDivided = Data.Combined.hCosThetaNeg_vs_Pos.Clone("Divided")
    hDivided.SetTitle( "(Data-BKG)/Signal")
    hDivided.Divide( TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_TTBar )
    hDivided.Draw( "COLZ" )

    cAllPlots.cd(4)
    #TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_Gen.SetTitle("TTBar@Generator Level")
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_Gen.SetTitle("Devision errors")
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_Gen.RebinX(10)
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_Gen.RebinY(10)
    for i in range(1 , hDivided.GetNbinsX()+1):
        for j in range(1 , hDivided.GetNbinsY()+1):
            TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_Gen.SetBinContent( i , j , hDivided.GetBinError( i , j) )
    TTBarSampleFull.Combined.hCosThetaNeg_vs_Pos_Gen.Draw("COLZ")
elif WhatToDo == 2:
    cAllPlots = TCanvas("AllPlots")
    cAllPlots.Divide( 2 , 1)
    cAllPlots.SetTitle("cos(#theta) vs. Top #eta")

    PLOT = TTBarSampleFull.Combined.hCosThetaVSTopEta.Clone("Plot")
    for sample in SamplesInfo:
        print sample
        PLOT.Add( SamplesInfo[sample].Combined.hCosThetaVSTopEta )
    cAllPlots.cd(1)
    PLOT.SetTitle("MC (signal+bkg)")
    PLOT.RebinX(10)
    PLOT.RebinY(10)
    PLOT.Draw("COLZ")

    cAllPlots.cd(2)
    Data.Combined.hCosThetaVSTopEta.SetTitle("Data")
    Data.Combined.hCosThetaVSTopEta.RebinX(10)
    Data.Combined.hCosThetaVSTopEta.RebinY(10)
    Data.Combined.hCosThetaVSTopEta.Draw("COLZ")
    
    cAllPlots.cd(3)
    TTBarSampleFull.Combined.hCosThetaVSTopEta.SetTitle("MC (signal only)")
    TTBarSampleFull.Combined.hCosThetaVSTopEta.RebinX(10)
    TTBarSampleFull.Combined.hCosThetaVSTopEta.RebinY(10)
    #TTBarSampleFull.Combined.hCosThetaVSTopEta.Draw("COLZ")

    cAllPlots.cd(4)
    TTBarSampleFull.Combined.hCosThetaVSTopEta_Gen.SetTitle("TTBar@Generator Level")
    TTBarSampleFull.Combined.hCosThetaVSTopEta_Gen.RebinX(10)
    TTBarSampleFull.Combined.hCosThetaVSTopEta_Gen.RebinY(10)
    #TTBarSampleFull.Combined.hCosThetaVSTopEta_Gen.Draw("text")
elif WhatToDo == 4:
    hSmearing = TH1D( "hSmearing" , "Smearing" , 5000 , -2.5 , 2.5)
    twoDH = TTBarSampleFull.Combined.CosTheta2DOriginal
    for i in range(1 , twoDH.GetNbinsX()+1):
        recValue = twoDH.GetXaxis().GetBinCenter( i )
        for j in range(1, twoDH.GetNbinsY() + 1):
            genValue = twoDH.GetYaxis().GetBinCenter( j )
            
            binContent = twoDH.GetBinContent( i , j)
            diff = recValue - genValue
            
            hSmearing.Fill( diff , binContent)

    hSmearing.Draw()

