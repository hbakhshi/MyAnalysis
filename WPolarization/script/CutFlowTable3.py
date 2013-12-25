#! /usr/bin/python
# -*- coding: utf-8 -*-
import os

#to disable x session during the run
os.environ['DISPLAY'] = '0'

from ROOT import TFile, TTree, THStack, TCanvas, TH1D, TH1F, gROOT, TGraph, TGraphErrors 
from Table import *
from SampleManager import *
from Fit import *
from subprocess import call, Popen, PIPE
from math import sqrt

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

sorted_samples = ['TTBarSummer2011', 'DYSummer2011', 'DY10To50Summer2011' ,'SingleTopTWSummer2011', 'WJetsSummer2011' , 'WWSummer2011' , 'SingleTopSummer2011' , 'WZSummer2011', 'ZZSummer2011' ]
sorted_samples.reverse()
systematic_samples = []#'SysWJetsQU' , 'SysTTQU' , 'SysTTQD' , 'SysZJetsQD' , 'SysZJetsQU' , 'SysTTM175' , 'SysWJetsQD' , 'SysTTM169']
SamplesInfo = {}
SysSamplesInfo = {}

stack_costheta_ee = THStack("stackCosTheta",'CosTheta for ee Events')
stack_costheta_mm = THStack("stackCosTheta",'CosTheta for mm Events')
stack_costheta_em = THStack("stackCosTheta",'CosTheta for em Events')
stack_costheta_comb = THStack("stackCosTheta",'CosTheta for all Events')

PropertiesToDraw = {'NumberOfJets':{} , 'PFMET':{}, 'Electrons_InvariantMass':{}, 'JetsHT':{} , 'NumberOfBJets':{} , 'FirstLeptonEta':{} , 'FirstLeptonPt':{} , 'SecondLeptonEta':{} , 'SecondLeptonPt':{} , 'FirstJetPt':{} , 'SecondJetPt':{} , 'ThirdJetPt':{} }

PrintCutFlowTables = True
DrawCosThetaPlots = True
DrawOtherPlots = True
DrawControlPlots = True
DoFit = True
CalcSystematics = DoFit and True
DrawFitComparisons = CalcSystematics and True
WriteAllCombinedFile =  True
ExtractAllPlots = DrawControlPlots or DrawOtherPlots

for sample in sorted_samples:
    SamplesInfo[sample] = SampleInfo(sample, ExtractAllPlots)

#for sample in systematic_samples:
#    SysSamplesInfo[sample] = SampleInfo(sample , False)

Data = DataInfo(ExtractAllPlots)

SampleInfoType = type( SamplesInfo[SamplesInfo.keys()[0] ])

gFGenRec = TGraphErrors()
gFNeg = TGraphErrors()
gF0 = TGraphErrors()
gFPos = TGraphErrors()
hAxis = TH1D('hAxis' , 'Axis' , 4 , 0.5 ,4.5 )
hAxis.SetStats(0)
hAxis2 = hAxis.Clone('hAxis_0')
hAxis3 = hAxis.Clone('hAxis_p')
hAxis4 = hAxis.Clone('hAxis_genrec')

class SystematicsCalculator:
    def __init__(self, f0 , statF0 , fL , statFL , fR , statFR , fGenRec , statFGenRec , correlation , cosThetaVals):
        self.F0 = Variable(f0, statF0)
        self.FL = Variable(fL, statFL)
        self.FR = Variable(fR, statFR)
        self.FGenRec = Variable(fGenRec , statFGenRec)
        self.cosThetaVals = cosThetaVals

        self.SysF0 = Variable( 0. , 0.)
        self.SysFR = Variable( 0. , 0.)
        self.SysFL = Variable( 0. , 0.)
        self.SysFGenRec = Variable( 0. , 0.)

        self.SysCosTheta = []
        for i in range(0,10):
            self.SysCosTheta.append(0.0)

    def RUNFit(self, ttbarname , zwqscale , samplexsection , other_syst):
        p = Popen( ["/home/hbakhshi/Documents/Analysis/bin/Goraz/FitValidation" , WhichChannel_ , "~/Documents/WPolarization/WPol/" , "3" , ttbarname , "1" , str(zwqscale) , str(samplexsection) , str(other_syst) ] , stdout=PIPE)
        out, err = p.communicate()
        #print out
        if out.find("MINUIT") == -1:
            allFitVals = [ float(k) for b in out.split("|")  for k in b.split("+-")  if not k == "" and not k == "\n"]

            #print ["/home/hbakhshi/Documents/Analysis/bin/Goraz/FitValidation" , WhichChannel_ , "~/Documents/WPolarization/WPol/" , "3" , ttbarname , "1" , str(zwqscale) , str(samplexsection) , str(other_syst) ]
            #print out
            #print allFitVals
            #print err

            if(self.Upper):
                self.LastUpperCosThetaVals = allFitVals[10:]
                self.LastUpperFRecGen =Variable( allFitVals[8], allFitVals[9])

                self.LastUpperFL = Variable( allFitVals[1], allFitVals[2])
                self.LastUpperF0 = Variable(allFitVals[3],allFitVals[4])
                self.LastUpperCorrelation = allFitVals[5]/(allFitVals[2]*allFitVals[4])
                self.LastUpperFR = Variable(allFitVals[6],allFitVals[7])

            else:
                self.LastLowerCosThetaVals = allFitVals[10:]
                self.LastLowerFRecGen = Variable(allFitVals[8], allFitVals[9])

                self.LastLowerFL = Variable( allFitVals[1], allFitVals[2])
                self.LastLowerF0 = Variable(allFitVals[3],allFitVals[4])
                self.LastLowerCorrelation = allFitVals[5]/(allFitVals[2]*allFitVals[4])
                self.LastLowerFR = Variable(allFitVals[6],allFitVals[7])

            return True
        else :
            print "error in fitting : " + ttbarname + "-" + str(zwqscale) +"-" + str(samplexsection) +"-" + str(other_syst) 
            return False

    def GetUpperError(self, WhichF): #-1:L , 0:0, +1:R, -100:FGenRec
        if WhichF == -100:
            return (self.LastUpperFRecGen - self.FGenRec)
        elif WhichF < 0:
            return (self.LastUpperFL - self.FL)
        elif WhichF == 0:
            return (self.LastUpperF0 - self.F0)
        elif WhichF > 0:
            return (self.LastUpperFR - self.FR)

    def GetLowerError(self, WhichF):
        if WhichF == -100:
            return (self.LastLowerFRecGen - self.FGenRec)
        if WhichF < 0:
            return (self.LastLowerFL - self.FL)
        elif WhichF == 0:
            return (self.LastLowerF0 - self.F0)
        elif WhichF > 0:
            return (self.LastLowerFR - self.FR)

    def Mean(self, whichF):
        return (( uabs(self.GetLowerError(whichF)) + uabs(self.GetUpperError(whichF)))/2.0)

    def MeanVal(self, num1 , num2):
        #return max(num1 , num2)
        return (num1 + num2) / 2.0

    def SetCosThetaSystematics(self):
        for i in range(0,10):
            upper = abs(self.LastUpperCosThetaVals[i] - self.cosThetaVals[i])
            lower = abs(self.LastLowerCosThetaVals[i] - self.cosThetaVals[i])

            mean_val = self.MeanVal(upper , lower)

            self.SysCosTheta[i] = sqrt( self.SysCosTheta[i]*self.SysCosTheta[i] + mean_val*mean_val )

    def SetTotalSystematics(self):
        
        self.SysFL = usqrt( self.SysFL*self.SysFL + self.Mean(-1)*self.Mean(-1) )
        self.SysF0 = usqrt( self.SysF0*self.SysF0 + self.Mean(0)*self.Mean(0) )
        self.SysFR = usqrt( self.SysFR*self.SysFR + self.Mean(1)*self.Mean(1) )
        self.SysFGenRec = usqrt( self.SysFGenRec*self.SysFGenRec + self.Mean(-100)*self.Mean(-100) )

    def CalcSysTopMass( self, dir1, ttsysSampleName1 , dir2, ttsysSampleName2 , name):
        pwd = os.getcwd()
        os.chdir(dir1)
        print os.getcwd()
        self.Upper = True
        if self.RUNFit(ttsysSampleName1 , 0 , 0 , 0):
            os.chdir(dir2)
            self.Upper = False
            if self.RUNFit(ttsysSampleName2 , 0 , 0 , 0):
                self.SetTotalSystematics()
                self.SetCosThetaSystematics()

                os.chdir(pwd)

                return self.Print( name)

        os.chdir(pwd)
        return "|"+name+"|||"
        

    def CalcSysTT(self, ttsysSampleName1 , ttsysSampleName2 , name):
        
        self.Upper = True
        if self.RUNFit(ttsysSampleName1 , 0 , 0 , 0):
            self.Upper = False
            if self.RUNFit(ttsysSampleName2 , 0 , 0 , 0):
                self.SetTotalSystematics( )
                self.SetCosThetaSystematics()

                return self.Print( name)

        return "|"+name+"|||"
      
    def CalcSysWZQScale(self):
        
        self.Upper = True
        if self.RUNFit("TTBarSummer2011" , 1 , 0 , 0):
            self.Upper = False
            if self.RUNFit("TTBarSummer2011" , -1 , 0 , 0):
                self.SetTotalSystematics(  )
                self.SetCosThetaSystematics()

                return self.Print( "WZQScale" )

        return "|WZQScale|||"

    def CalcSysPDF(self):
        allPosVariations_L = []
        allPosVariations_0 = []
        allPosVariations_R = []
        allPosVariations_RecGen = []

        allNegVariations_L = []
        allNegVariations_0 = []
        allNegVariations_R = []
        allNegVariations_RecGen = []

        for mem in range(0, 20):
            self.Upper = True
            if self.RUNFit("TTBarSummer2011" , 0 , 0 , 101+2*mem):
                self.Upper = False
                if self.RUNFit("TTBarSummer2011" , 0 , 0 , 101+2*mem+1):
                    allPosVariations_L.append( max( [ self.GetUpperError( -1 ) , self.GetLowerError( -1 ) , 0 ] )**2 )
                    allPosVariations_0.append( max( [ self.GetUpperError( 0 ) , self.GetLowerError( 0 ) , 0 ] )**2 )
                    allPosVariations_R.append( max( [ self.GetUpperError( 1 ) , self.GetLowerError( 1 ) , 0 ] )**2 )
                    allPosVariations_RecGen.append( max( [ self.GetUpperError( -100 ) , self.GetLowerError( -100 ) , 0 ] )**2 )

                    allNegVariations_L.append( max( [ -self.GetUpperError( -1 ) , -self.GetLowerError( -1 ) , 0 ] )**2 )
                    allNegVariations_0.append( max( [ -self.GetUpperError( 0 ) , -self.GetLowerError( 0 ) , 0 ] )**2 )
                    allNegVariations_R.append( max( [ -self.GetUpperError( 1 ) , -self.GetLowerError( 1 ) , 0 ] )**2 )
                    allNegVariations_RecGen.append( max( [ -self.GetUpperError( -100 ) , -self.GetLowerError( -100 ) , 0 ] )**2 )




        F0_Err0 = Variable( self.F0.nominal_value , 0.0)
        FL_Err0 = Variable( self.FL.nominal_value , 0.0)
        FR_Err0 = Variable( self.FR.nominal_value , 0.0)
        FGenRec_Err0 = Variable( self.FGenRec.nominal_value , 0.0 )

        self.LastUpperFL = FL_Err0 + uabs(usqrt( sum(allPosVariations_L) ))
        self.LastLowerFL = FL_Err0 - uabs(usqrt( sum(allNegVariations_L) ))
        pdf_l = self.Mean( -1 )
        self.SysFL = usqrt( self.SysFL*self.SysFL + pdf_l**2 )

        self.LastUpperF0 = F0_Err0 + uabs(usqrt( sum(allPosVariations_0) ))
        self.LastLowerF0 = F0_Err0 - uabs(usqrt( sum(allNegVariations_0) ))
        pdf_0 = self.Mean(0)
        self.SysF0 = usqrt( self.SysF0*self.SysF0 + pdf_0**2 )

        self.LastUpperFR = FR_Err0 + uabs(usqrt( sum(allPosVariations_R) ))
        self.LastLowerFR = FR_Err0 - uabs(usqrt( sum(allNegVariations_R) ))
        pdf_r = self.Mean(1)
        self.SysFR = usqrt( self.SysFR*self.SysFR + pdf_r**2 )

        self.LastUpperFRecGen = FGenRec_Err0 + usqrt( sum(allPosVariations_RecGen) )
        self.LastLowerFRecGen = FGenRec_Err0 - usqrt( sum(allNegVariations_RecGen) )
        pdf_recgen = self.Mean(-100)
        self.SysFGenRec = usqrt( self.SysFGenRec*self.SysFGenRec + pdf_recgen**2 )

        self.LastUpperCorrelation = -1.
        self.LastLowerCorrelation = -1.
        
        return self.Print("PDF")


        # up_l = usqrt( sum(allPosVariations_L) )
        # dn_l = usqrt( sum(allNegVariations_L) )
        # pdf_l = self.MeanVal(up_l , dn_l)
        # self.SysFL = usqrt( self.SysFL*self.SysFL + pdf_l**2 )

        # up_0 = usqrt( sum(allPosVariations_0) )
        # dn_0 = usqrt( sum(allNegVariations_0) )
        # pdf_0 = self.MeanVal(up_0 , dn_0)
        # self.SysF0 = usqrt( self.SysF0*self.SysF0 + pdf_0**2 )

        # up_r = usqrt( sum(allPosVariations_R) )
        # dn_r = usqrt( sum(allNegVariations_R) )
        # pdf_r = self.MeanVal(up_r , dn_r)
        # self.SysFR = usqrt( self.SysFR*self.SysFR + pdf_r**2 )

        # up_recgen = usqrt( sum(allPosVariations_RecGen) )
        # dn_recgen = usqrt( sum(allNegVariations_RecGen) )
        # pdf_recgen = self.MeanVal(up_recgen , dn_recgen)
        # self.SysFGenRec = usqrt( self.SysFGenRec*self.SysFGenRec + pdf_recgen**2 )

        # #self.SetTotalSystematics(  )
        # #self.SetCosThetaSystematics()
       
        # ret =  "|PDF|"+str(rounddf_l.nominal_value())+"||"+str(pdf_0)+"||"+str(pdf_r)+"||"+str(pdf_recgen)+"||" # % {'fl':pdf_l.nominal_value() , 'f0':pdf_0.nominal_value() , 'fr':pdf_r.nominal_value() , 'frecgen':pdf_recgen.nominal_value()}
        # return ret

        


    def CalcSysXSection(self , sample):


        name = ""
        if sample== 1 :
            name = "DY"
        elif sample == 2 :
            name = "W"
        elif sample == 3 :
            name = "SingleTopTW"
        elif sample == 4 :
            name = "SignleTopTChannel"
        elif sample == 5 :
            name = "WW"
        elif sample == 6 :
            name = "WZ"
        elif sample == 7 :
            name = "ZZ"

        name += " XSection"

        self.Upper = True
        if self.RUNFit("TTBarSummer2011" , 0 , sample , 0):
            self.Upper = False
            if self.RUNFit("TTBarSummer2011" , 0 , -sample , 0):
                self.SetTotalSystematics( )
                self.SetCosThetaSystematics()

                return self.Print( name)

        return "|"+name+"||||"

    def CalcSysOther(self , other_sys , name):
        
        self.Upper = True
        if self.RUNFit("TTBarSummer2011" , 0 , 0 , other_sys):

            self.Upper = False
            if self.RUNFit("TTBarSummer2011" , 0 , 0 , -other_sys):

                self.SetTotalSystematics()
                self.SetCosThetaSystematics()

                return self.Print( name)

        return "|" + name + "|||"
      
    def Print(self, name):

        ret = "   |" + name

        the_vals = []
        if name == "Total":
            the_vals = [ uround(self.SysFL, 4), 0.0 , uround(self.SysF0,4) , 0.0 , uround(self.SysFR,4) , 0.0 , uround(self.SysFGenRec,4) , 0.0 ]
        elif name == "Name" :
            the_vals = [ "FL", "FL_ERR" , "F0" , "F0_ERR" , "FR" , "FR_ERR" , "FGenRec" , "Correlation" ]
        else :
            the_vals = [ uround(self.Mean(-1),4) ,uround(self.Mean(0),4) , uround(self.Mean(+1),4) , uround(self.Mean(-100),4) ]
            the_vals_up = [ uround(self.LastUpperFL,4) ,uround(self.LastUpperF0,4) , uround(self.LastUpperFR,4) , uround(self.LastUpperFRecGen ,4) ]
            the_vals_low = [ uround(self.LastLowerFL,4) ,uround(self.LastLowerF0,4) , uround(self.LastLowerFR,4) , uround(self.LastLowerFRecGen,4) ]
            ret +=  "|" + str(the_vals[0].nominal_value) + '(' + str(the_vals_low[0].nominal_value) + ',' + str(the_vals_up[0].nominal_value)  + ')'
            ret +=  "|" + str(the_vals[0].std_dev() ) + '(' + str(the_vals_low[0].std_dev()) + ',' + str(the_vals_up[0].std_dev() )  + ')'
            ret +=  "|" + str(the_vals[1].nominal_value) + '(' + str(the_vals_low[1].nominal_value) + ',' + str(the_vals_up[1].nominal_value)  + ')'
            ret +=  "|" + str(the_vals[1].std_dev() ) + '(' + str(the_vals_low[1].std_dev() ) + ',' + str(the_vals_up[1].std_dev() )  + ')'
            ret +=  "|" + str(the_vals[2].nominal_value) + '(' + str(the_vals_low[2].nominal_value) + ',' + str(the_vals_up[2].nominal_value)  + ')'
            ret +=  "|" + str(the_vals[2].std_dev() ) + '(' + str(the_vals_low[2].std_dev() ) + ',' + str(the_vals_up[2].std_dev() )  + ')'
            ret +=  "|" + str(the_vals[3].nominal_value) + '(' + str(the_vals_low[3].nominal_value) + ',' + str(the_vals_up[3].nominal_value)  + ')'
            ret +=  "|" + str( round(self.LastLowerCorrelation*100 , 2) ) + '-' + str( round(100*self.LastUpperCorrelation) )
            return ret


        ret +=  "|" + str(the_vals[0])
        ret +=  "|" + str(the_vals[1])
        ret +=  "|" + str(the_vals[2])
        ret +=  "|" + str(the_vals[3])
        ret +=  "|" + str(the_vals[4])
        ret +=  "|" + str(the_vals[5])
        ret +=  "|" + str(the_vals[6])
        ret +=  "|" + str(the_vals[7])
        
        ret += "|"

        return ret
        

for  WhichChannel_ in ['Combined']: # ,'EM' , 'MM' , 'EE' ]:
     WhichChannel = WhichChannel_
     
     t_mc = Table()
     t_mc_w = Table()

     #AllSamples = [ SampleInfoType. __getattribute__( SamplesInfo[sample] , WhichChannel) for sample in SamplesInfo ]
     AllSamples = dict( ( sample , SampleInfoType. __getattribute__( SamplesInfo[sample] , WhichChannel) ) for sample in SamplesInfo  )

     print >>org_file,'* ' + WhichChannel
     if PrintCutFlowTables:
         print AllSamples
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

         stack = SamplesStack( AllSamples , sorted_samples , DrawCosThetaPlots , DrawControlPlots , DrawOtherPlots )
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
         last_sum = sumRow_w[last_column_name_w].nominal_value
         nCols = len(t_mc_w.rows[0])

         tbl_mc_weighted_formula_str = "    #+TBLFM: $%(newColIndex)d=100-( %(lastSum).8f - $%(lastColIndex)d )*100/ %(lastSum).8f" % {'newColIndex':nCols+1 ,'lastSum':last_sum , 'lastColIndex':nCols }

         tbl_data_tmp = Table(False, False)
         tbl_data_tmp.append( Data.AllPlots[WhichChannel_].RowW )

         # r_data_mc_diff = RowObject()        
         # for column in Data.AllPlots[WhichChannel_].RowW.keys():
         #     if column.find( 'DataSet' ) > -1:
         #         r_data_mc_diff[column] = 'Data-MC'
         #     else:
         #         r_data_mc_diff[column] = Data.AllPlots[WhichChannel_].RowW[column] - sumRow_w[column]
         # tbl_data_tmp.append( r_data_mc_diff )

         # r_weight_effect = RowObject()      
         # for column in sumRow.keys():
         #     if column.find( 'DataSet' ) > -1:
         #         r_weight_effect[column] = 'Weight-Effect'
         #     else:
         #         r_weight_effect[column] = sumRow_w[column]/sumRow[column]


         #r_weight_effect[Data.AllPlots[WhichChannel_].LastColumnW] = 0.0
         #tbl_data_tmp.append(r_weight_effect)

         print >> org_file, tbl_data_tmp
         print >> org_file, tbl_mc_weighted_formula_str

     if DrawCosThetaPlots:
         print >>org_file, '** Plot of Cos(\\theta)'
         print >> org_file, stack.DrawCosTheta( Data )

     if DrawControlPlots:
         print >>org_file,'** Control Plots'
         print >>org_file, stack.DrawAllProperties(Data)

     if DrawOtherPlots:
         print >>org_file,'** Other Plots'
         print >>org_file, stack.DrawTopRecPlots(Data)


     if DoFit:

         #start fitting 
         print >>org_file, '** Fit Results'


         print >>org_file, '    |Method|F_{L} | F_{0} |Correlation| F_{R} | F_{Rec/Gen} |'
         org_file.flush()
         call(["/home/hbakhshi/Documents/Analysis/bin/Goraz/FitValidation" , WhichChannel_ , "~/Documents/WPolarization/WPol/" , "3" , "TTBarSummer2011" , "20" , "0" , "0" , "0"] , stdout = org_file )

         #save the canvasas like what is saved via Fit.SaveCanvas method in Fit.py
         #save the stackAllButTTBar

         p = Popen( ["/home/hbakhshi/Documents/Analysis/bin/Goraz/FitValidation" , WhichChannel_ , "~/Documents/WPolarization/WPol/" , "3" , "TTBarSummer2011" , "1" , "0" , "0" , "0"] , stdout=PIPE)
         out, err = p.communicate()
         print out
         allFitVals = [ float(k) for b in out.split("|")  for k in b.split("+-")  if not k == "" and not k == "\n"]

         FL = allFitVals[1]
         FLErr = allFitVals[2]
         F0 = allFitVals[3]
         F0Err = allFitVals[4]
         Correlation = allFitVals[5]
         FR = allFitVals[6]
         FRErr = allFitVals[7]
         FRecGen = allFitVals[8]
         FRecGenErr = allFitVals[9]
         
         StandardCosThetaValues = allFitVals[10:]

         if CalcSystematics:
             calculator = SystematicsCalculator(F0 , F0Err , FL , FLErr  , FR , FRErr , FRecGen , FRecGenErr , Correlation , StandardCosThetaValues )
             print >>org_file, '** Systematics'
             print >>org_file, '*** Table'
             print >>org_file, calculator.Print( "Name" )
             
             #("SysTTMatchingU" , "SysTTMatchingD" , "Top Matching") , 
             for ttbarsystematicsample in [ ("SysTTQD" , "SysTTQU" , "Top QScale") ]:
                print >>org_file, calculator.CalcSysTT( ttbarsystematicsample[0], ttbarsystematicsample[1] , ttbarsystematicsample[2] )
             
             print >>org_file, calculator.CalcSysTopMass( "../RunM169/" ,  "SysTTM169" , "../RunM175/"  , "SysTTM175" , "Top Mass")

             print >>org_file, calculator.CalcSysWZQScale()

             for sampleId in range( 1 , 8):
                print >>org_file, calculator.CalcSysXSection( sampleId )

                 
             print >>org_file, calculator.CalcSysPDF()
             print >>org_file, calculator.CalcSysOther(1240, "Spin Correlation")
             print >>org_file, calculator.CalcSysOther(1, "PU")
             print >>org_file, calculator.CalcSysOther(2, "Lepton SF")
             print >>org_file, calculator.CalcSysOther(3, "Trigger SF")

             calculator.LastUpperFRecGen = Variable(FRecGen , FRecGenErr)
             calculator.LastLowerFRecGen = Variable(FRecGen , FRecGenErr)

             bTagSFSys = [0.0001 , 0.0001 , 0.0001]
             calculator.LastUpperFL = Variable( FL+bTagSFSys[0] , FLErr)
             calculator.LastLowerFL = Variable( FL+bTagSFSys[0] , FLErr)
             calculator.LastUpperF0 = Variable( F0+bTagSFSys[1] , F0Err)
             calculator.LastLowerF0 = Variable( F0+bTagSFSys[1] , F0Err)
             calculator.LastUpperFR = Variable( FR+bTagSFSys[2] , FRErr)
             calculator.LastLowerFR = Variable( FR+bTagSFSys[2] , FRErr)
             print >>org_file, calculator.Print( 'b-Tag SF' )
             calculator.SetTotalSystematics()

             JESSys =  [0.010 , 0.006 , 0.010 ]
             calculator.LastUpperFL = Variable( FL+JESSys[0] , FLErr)
             calculator.LastLowerFL = Variable( FL+JESSys[0] , FLErr)
             calculator.LastUpperF0 = Variable( F0+JESSys[1] , F0Err)
             calculator.LastLowerF0 = Variable( F0+JESSys[1] , F0Err)
             calculator.LastUpperFR = Variable( FR+JESSys[2] , FRErr)
             calculator.LastLowerFR = Variable( FR+JESSys[2] , FRErr)
             print >>org_file, calculator.Print( 'JES')
             calculator.SetTotalSystematics()

             print >>org_file, "|-"
             print >>org_file, calculator.Print( "Total" )

             FLErr = usqrt( FLErr*FLErr +  calculator.SysFL*calculator.SysFL )
             F0Err = usqrt( F0Err*F0Err +  calculator.SysF0*calculator.SysF0 )
             FRErr = usqrt( FRErr*FRErr +  calculator.SysFR*calculator.SysFR )
             FRecGenErr = usqrt( FRecGenErr*FRecGenErr + calculator.SysFGenRec*calculator.SysFGenRec )

             print >> org_file, stack.DrawCosTheta( Data , calculator.SysCosTheta )
             print >> org_file, calculator.SysCosTheta

         pointID = gFNeg.GetN()

         hAxis.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)
         hAxis2.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)
         hAxis3.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)
         hAxis4.GetXaxis().SetBinLabel( pointID+1 , WhichChannel_)

         gFNeg.SetPoint( pointID , FL , pointID + 1 )
         gFNeg.SetPointError( pointID ,FLErr.nominal_value , 0.0 )

         gF0.SetPoint( pointID , F0 , pointID + 1 )
         gF0.SetPointError( pointID , F0Err.nominal_value , 0.0 )

         gFPos.SetPoint( pointID , FR , pointID + 1 )
         gFPos.SetPointError( pointID ,  FRErr.nominal_value , 0.0 )

         gFGenRec.SetPoint( pointID , FRecGen , pointID+1)
         gFGenRec.SetPointError( pointID , FRecGenErr.nominal_value , 0.0)


allHorizontalHistos = {}
def DrawAndCompStandardVal( haxis  , graph , std , stdErr , name , title):  
    c = TCanvas(name)
    c.cd()

    xMin = std - 15*stdErr
    xMax = std + 15*stdErr 
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
    c.SaveAs('FitResults/'+name+'.pdf')
    return 'FitResults/'+name+'.gif'

if DrawFitComparisons:
    print >>org_file, '* Fit Comparison'
    if not os.access( 'FitResults' , os.F_OK) :
        os.mkdir( 'FitResults' )


    print >>org_file, '** F_{L}'
    #flStd = 0.3
    #flStdErr = 0.02

    print >>org_file, '   [[['+ DrawAndCompStandardVal(hAxis , gFNeg , 0.29 , 0.02  , 'FL' , 'F_{L}')  +']]]'

    print >>org_file, '** F_{0}'
    print >>org_file, '   [[['+ DrawAndCompStandardVal(hAxis2 , gF0 , 0.71 , 0.02 , 'F0' , 'F_{0}')  +']]]'

    print >>org_file, '** F_{R}'
    print >>org_file, '   [[[' +DrawAndCompStandardVal(hAxis3 , gFPos , 0.0 , 0.02 , 'FR' , 'F_{R}')+ ']]]'

    print >>org_file, '** F_{RecGen}'
    print >>org_file, '   [[[' +DrawAndCompStandardVal(hAxis4 , gFGenRec , 1.0 , 0.1 , 'FGenRec' , 'F_{GenRec}')+ ']]]'


if WriteAllCombinedFile:
    fDGamma1 = dGammaFunctin.GetFunction('fnc1')
    fOut = TFile('AllCombined.root' , 'RECREATE')
    for sample in SamplesInfo:
        hCosTheta = SamplesInfo[sample].Combined.hCosTheta['10']

        hCosTheta.Fit(fDGamma1)

        hCosTheta.Write()

        if sample.find('TTBarSummer2011') >= 0:
            SamplesInfo[sample].Combined.CosTheta2D.Write()
    

    for sample in SysSamplesInfo:
        SysSamplesInfo[sample].Combined.hCosTheta['10'].Write()

        if sample.find('SysTT') >= 0:
            SysSamplesInfo[sample].Combined.CosTheta2D.Write()

    Data.Combined.hCosTheta['10'].Write()
    fOut.Close()

os.environ['DISPLAY'] = ':0.0'
