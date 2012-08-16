#! /usr/bin/python -i

from ROOT import TCanvas,  TFile, THStack, TH1D, gROOT, TF1, TF3, TVirtualFitter, Double, TGraphErrors, TMultiGraph, TGraph, TF2, TRandom, kBlue, kRed, kAzure, TTree
import array
from math import sqrt, exp, pi, pow, factorial, log
import os, re

class dGammaFunctin:
    def __init__(self , justOne = -2):
        self.JustOne = justOne
        pass

    def __call__(self, x , p):
        #help(x)
        FNeg = p[0]
        F0 = p[1]
        FPos = (1.0 - FNeg - F0)

        norm_factor = p[2]

        costheta = x[0]

        aa = (1.0 - costheta )*(1.0 - costheta )
        bb = (1.0 - costheta*costheta)
        cc = (1.0 + costheta )*(1.0 + costheta )
    
        retL = (FNeg*3.0*aa / 8.0)
        ret0 = (F0*3.0*bb / 4.0 )
        retR = (FPos*3.0*cc / 8.0)

        if self.JustOne == -2:
            return norm_factor*(retL+ret0+retR)
        elif self.JustOne == -1:
            return norm_factor*retL
        elif self.JustOne == 0:
            return norm_factor*ret0
        elif self.JustOne == 1:
            return norm_factor*retR

    @staticmethod
    def GetFunction(name , justOne = -2):
        functor = dGammaFunctin(justOne)
        gROOT.cd()
        ret = TF1(name , functor , -1.0 , 1.0 , 3)

        ret.SetParName(0 , "FNeg" );
        ret.SetParName(1 , "F0" );
        ret.SetParName(2 , "NormaFactor" );
            
        ret.SetParLimits(0 , 0.0 , 1.0);
        ret.SetParLimits(1 , 0.0 , 1.0);
            
        ret.SetParameters( 0.3 , 0.7 , 1.0 );
            
        return ret;

class WeightFunction:
    def __init__(self, std_fneg = 0.3 , std_f0 = 0.7):
        self.Function = dGammaFunctin()
        self.StandardParams = [ std_fneg , std_f0 , 1.0 ]

    def __call__(self, x , p):
        val_standard = self.Function( x , self.StandardParams )

        params = [ p[0] , p[1] , 1.0 ]
        val_non_standard = self.Function( x , params )

        #print "%{std}f/%{nostd}f" % {'std':val_standard, 'nostd':val_non_standard}

        return val_non_standard / val_standard

    @staticmethod
    def GetWeightFunction(name,std_fneg = 0.3 , std_f0 = 0.7):
        functor = WeightFunction(std_fneg, std_f0)
        ret = TF1( name , functor , -1.0 , 1.0 , 2)

        ret.SetParName(0 , "FNeg" );
        ret.SetParName(1 , "F0" );

        ret.SetParLimits(0 , 0.0 , 1.0);
        ret.SetParLimits(1 , 0.0 , 1.0);
            
        ret.SetParameters( 0.3 , 0.7 );
            
        return ret;

class costheta_t:
    def __init__(self):
        self.genCosTheta = Double(0.0)
        self.eventWeight = Double(0.0)
        self.lumiWeight = Double(0.0)

class LLFunction:

    def NCosThetaBins(self):
        return self.hData.GetNbinsX()

    def CosThetaRange(self):
        return range(1, self.hData.GetNbinsX()+1)

    def __init__(self , stack , hData , hTTBar , distribution_function, std_fneg, std_f0 , name , TwoD = False):
        self.Tree = False
        self.BKGStack = stack
        self.TwoD = TwoD
        self.hData = hData
        #SET DATA ERROR
        for nBin in self.CosThetaRange():
            err = sqrt(self.hData.GetBinContent(nBin))
            self.hData.SetBinError(nBin, err)

        #MAKE A TH OUT OF THE STACK, and calc errors
        self.hSum = TH1D('hSum'+name , 'All Backgrounds' , self.NCosThetaBins() , -1 , 1)

        Errors = {}
        for nBin in self.CosThetaRange():
            Errors[nBin] = 0.0

        for h_MC in stack.GetHists():
            self.hSum.Add( h_MC )
            for nBin in range(1, h_MC.GetNbinsX()+1):
                Errors[nBin] += 0 #(h_MC.GetBinContent(nBin)*h_MC.GetBinContent(nBin))

        for nBin in self.CosThetaRange():
            self.hSum.SetBinError( nBin , sqrt(Errors[nBin]) )

        self.TTBar = hTTBar

        self.WF1 = WeightFunction.GetWeightFunction('W', std_fneg , std_f0)
        self.DistributionFunction = distribution_function

        self.ErrorVariations = {}
        self.ErrorRandomGenerators = {}
        self.SeedGenerator = TRandom(65458782)


    def SetTreeFile(self , fileName , treeNameFormat , begin , end):
        self.Tree = True
        self.TreeFile = TFile(fileName , "READ")
        self.TreeNameFormat = treeNameFormat        

        self.TreeInfo = {}

        for nBin in range(begin , end+1):
            treeName = self.TreeNameFormat % {'bin':nBin}
            tree = self.TreeFile.Get(treeName)

            genCosTheta = array.array('d' , [Double(0.0)])
            eventWeight = array.array('d' , [Double(0.0)])
            lumiWeight  = array.array('d' , [Double(0.0)])

            tree.SetBranchAddress("GenCosTheta" , genCosTheta)
            tree.SetBranchAddress("EventWeight" , eventWeight)
            tree.SetBranchAddress("LumiWeight" , lumiWeight)            

            thisBinInfo = []

            for entry in range(0 , tree.GetEntries() ):
                tree.GetEntry(entry)                

                evtweight = eventWeight[0]*lumiWeight[0]

                thisBinInfo.append( ( genCosTheta[0] , evtweight) )

            print "Tree_%d is read" % (nBin)
            self.TreeInfo[nBin] = thisBinInfo

    def SetErrorVariations(self):
        for nBin in self.CosThetaRange():
            if not self.ErrorRandomGenerators.has_key( nBin ):
                self.ErrorRandomGenerators[nBin] = TRandom( self.SeedGenerator.Integer( 10000000000 ) )

            self.ErrorVariations[ nBin ] = self.ErrorRandomGenerators[nBin].Uniform( -1.0 , 1.0 )


    def SaveCanvas(self, fileName , fL , f0 ):

        c0 = TCanvas(fileName+'WeightFuncion')
        c0.cd()

        wf = WeightFunction.GetWeightFunction(fileName )
        wf.SetParameters( fL , f0)


        if self.TwoD:
            hWeights = TH1D( 'hFinal_Weights' , 'Final Weights' , len(self.CosThetaRange()) , -1 , 1)

            for binCosTheta in self.CosThetaRange():
                hprojection = self.TTBar.ProjectionY('_py' , binCosTheta , binCosTheta , 'o')
                hprojection.Multiply( wf )
                hWeights.SetBinContent( binCosTheta ,  hprojection.Integral() )

            hWeights.Draw("text L")
            
            hRecCosTheta = self.TTBar.ProjectionX('_px')
            hRecCosTheta.Multiply( wf )

            hRecCosTheta.Draw("text L same")

        else:    
            wf.Draw()

        c0.SaveAs( c0.GetName() + '.C')


        c = TCanvas(fileName)
        c.cd()


        self.hData.Draw()

        stack1 = self.BKGStack
        

        TTBar2 = None
        if not self.TwoD :
            TTBar2 = self.TTBar.Clone("TTBar1")
        else:
            TTBar2  = self.TTBar.ProjectionX('TTBar1_px' , 0 , -1 , 'o')

        TTBar2.SetFillStyle(1001)
        TTBar2.SetFillColor(8)
        stack1.Add(TTBar2)

        stack1.Draw('SAME')
        

        c.SaveAs(fileName + '.C')

        fileName = 'ttbar_' + fileName
        c2 = TCanvas(fileName)
        c2.cd()
        

        gROOT.cd()

        FinalDGammaFunction = dGammaFunctin.GetFunction('FinalDGammaFunction')
        FinalDGammaFunction.SetParameters(fL, f0, 0.2)
        FinalDGammaFunction.Draw()

        hTTBarNorm = None

        if not self.TwoD :
            hTTBarNorm = self.TTBar.Clone('hTTBarNorm')
        else :
            hTTBarNorm = self.TTBar.ProjectionX('hTTBarNorm' , 0 , -1 , 'o')
        hTTBarNorm.Scale( 1.0 / self.TTBar.Integral() )
        hTTBarNorm.SetLineStyle(0)
        hTTBarNorm.SetFillStyle(3005)        

        stackTTBar = THStack('Stack' , 'TTBar')
        FinalDGammaFunctionR = dGammaFunctin.GetFunction('FinalDGammaFunctionR' , 1)
        FinalDGammaFunctionR.SetParameters(fL, f0, 0.2)
        #FinalDGammaFunctionR.Draw('SAME')        
        hTTBarR = hTTBarNorm.Clone('hTTBarNormR')
        hTTBarR.Multiply(FinalDGammaFunctionR)
        hTTBarR.SetTitle('TTbar R')
        hTTBarR.Divide(FinalDGammaFunction)
        hTTBarR.SetFillColor(kRed)
        stackTTBar.Add(hTTBarR)

        FinalDGammaFunctionL = dGammaFunctin.GetFunction('FinalDGammaFunctionL' , -1)
        FinalDGammaFunctionL.SetParameters(fL, f0, 0.2)
        #FinalDGammaFunctionL.Draw('SAME')
        hTTBarL = hTTBarNorm.Clone('hTTBarNormL')
        hTTBarL.Multiply(FinalDGammaFunctionL)
        hTTBarL.Divide(FinalDGammaFunction)
        hTTBarL.SetTitle('TTbar L')
        hTTBarL.SetFillColor(kAzure)
        hTTBarL.SetFillStyle(3003)
        stackTTBar.Add(hTTBarL)

        FinalDGammaFunction0 = dGammaFunctin.GetFunction('FinalDGammaFunction0' , 0)
        FinalDGammaFunction0.SetParameters(fL, f0, 0.2)
        #FinalDGammaFunction0.Draw('SAME')
        hTTBar0 = hTTBarNorm.Clone('hTTBarNorm0')
        hTTBar0.Multiply(FinalDGammaFunction0)
        hTTBar0.Divide(FinalDGammaFunction)
        hTTBar0.SetTitle('TTbar 0')
        hTTBar0.SetFillColor(kBlue)
        hTTBar0.SetFillStyle(3001)
        stackTTBar.Add(hTTBar0)


        stackTTBar.Draw('hist same')


        hTTBarFromData = TH1D('hTTBarFromData' , 'data - background' , 10 , -1 , 1)
        hTTBarFromData.Add( self.hData , self.hSum , 1.0 , -1.0)
        print self.hData.Integral() 
        print self.hSum.Integral()
        scale1 = self.hData.Integral() - self.hSum.Integral()
        print scale1
        hTTBarFromData.Scale( 1.0 / scale1 )
        hTTBarFromData.Draw('SAME')
        

        c2.SaveAs(fileName + '.C')
        
    def __call__(self , x , p=0):
        #x[0] = f_Neg
        #x[1] = f_0
        #x[2] = rec_gen factor
        #no parameter is needed

        LL = 0.0

        for nBin in self.CosThetaRange():
            N_BKG = self.hSum.GetBinContent(nBin)

            CosTheta = self.hSum.GetBinCenter(nBin)
            self.WF1.SetParameters(x[0] , x[1])
            N_TT = 0.0

            if self.Tree:
                # treeName = self.TreeNameFormat % {'bin':nBin}
                # tree = self.TreeFile.Get(treeName)

                # genCosTheta = array.array('d' , [Double(0.0)])
                # eventWeight = array.array('d' , [Double(0.0)])
                # lumiWeight  = array.array('d' , [Double(0.0)])

                # tree.SetBranchAddress("GenCosTheta" , genCosTheta)
                # tree.SetBranchAddress("EventWeight" , eventWeight)
                # tree.SetBranchAddress("LumiWeight" , lumiWeight)
                
                # for entry in range(0 ,  tree.GetEntries() ):
                #     tree.GetEntry(entry)

                #     evtweight = eventWeight[0]*lumiWeight[0]
                #     WeightVal = self.WF1.Eval(genCosTheta[0])

                for entry in self.TreeInfo[nBin]:

                    evtweight = entry[1]
                    WeightVal = self.WF1.Eval(entry[0])
                    
                    N_TT += (evtweight*WeightVal)

            elif not self.TwoD :
                WeightVal = self.WF1.Eval(CosTheta)
                N_TT = WeightVal*self.TTBar.GetBinContent(nBin)
            else:
                gROOT.cd()
                hGenInfo = self.TTBar.ProjectionY( '_py' , nBin , nBin  , 'o')
                hGenInfo.Multiply( self.WF1 )
                N_TT = hGenInfo.Integral()

            N_MC = N_BKG + x[2]*N_TT

            N_Data = self.hData.GetBinContent(nBin)
            if not len(self.ErrorVariations) == 0:
                variation = self.ErrorVariations[nBin]
                N_Data += variation*sqrt(N_Data)

            LL += self.DistributionFunction( N_MC , N_Data )
        
        return LL


    @staticmethod
    def GetLLFunction(name , stack , hData , hTTBar , distribution_function,std_fneg ,std_f0 , isTwoD = False):
        
        functor = LLFunction( stack , hData , hTTBar , distribution_function, std_fneg, std_f0 , name , isTwoD)

        #functor.hData.Print('ALL')
        #functor.hSum.Print('ALL')
        #functor.TTBar.Print('ALL')
        #functor.WF1.Print('ALL')
        #functor.WF1.Draw()

        ret = TF3( name , functor , 0.0 , 1.0 , 0.0 , 0.1 , 0.0 , 2.0 , 0)

        ret.SetRange( 0.0 , 0.0 , 0.000001 , 1.0 , 1.0 , 2.0)

        return [ret, functor];


def GaussianDistribution( n_mc , n_data ):
    x_mean = n_mc
    x = n_data
    if n_data+n_mc < 0:
        return 0.0

    sigma = sqrt( n_mc + n_data )

    coeff = 1.0/(sigma*sqrt(2*pi))

    return (x-x_mean)*(x-x_mean) / (2*sigma*sigma) #coeff*exp( -(x-x_mean)*(x-x_mean) / (2*sigma*sigma) )

def PoissonDistribution(n_mc , n_data):
    x_mean = n_mc
    x = n_data

    #print '%f-%d' % (n_mc , n_data)
    if x_mean <= 0.0 :
        return 0.0

    log_pow_1 = x*log(x_mean)
    log_exp_1 = -x_mean

    log_factoral =  x*log(x) - x + ( log(x*(1+4*x*(1+2*x))) / 6 ) + log(pi)/2 

    return log_factoral - log_exp_1 - log_pow_1 #pow_1*exp_1/factorial_1

def GetMinimum(F_,CalcError = True , LoopsForMCErrors = 0):
    # based on the documentation of TF3::GetMinimumXYZ from
    # http://root.cern.ch/root/html532/src/TF3.cxx.html#QUjxjE

    F = F_[0]

    x_m = Double(0.3)
    y_m = Double(0.7)
    z_m = Double(1.0)

    if not CalcError:
        F.GetMinimumXYZ(x_m , y_m , z_m)        
        return [x_m.real , y_m.real , z_m.real]

    if not LoopsForMCErrors == 0:
        #MC ERROR CALC GOES HERE
        F.GetMinimumXYZ(x_m , y_m , z_m)        
        AllF0 = []
        AllFP = []
        AllFN = []
        AllFRatio = []

        x_m_tmp = Double(0.0)
        y_m_tmp = Double(0.0)
        z_m_tmp = Double(0.0)
        for MC in range(0 , LoopsForMCErrors):
            if MC % 5 ==0:
                print MC
            F_[1].SetErrorVariations()
            F.GetMinimumXYZ(x_m_tmp , y_m_tmp , z_m_tmp)

            AllFN.append( x_m_tmp.real )
            AllF0.append( y_m_tmp.real )
            AllFP.append( 1.0 - y_m_tmp.real  - x_m_tmp.real )
            AllFRatio.append( z_m_tmp.real)
            #print AllFN


        FRatioMin = min(AllFRatio)
        FRatioMax = max(AllFRatio)
        FRatioErr = (FRatioMax - FRatioMin)/2.0

        FNMin = min(AllFN)
        FNMax = max(AllFN)
        FNErr = (FNMax - FNMin)/2.0

        F0Min = min(AllF0)
        F0Max = max(AllF0)
        F0Err = (F0Max - F0Min)/2.0

        FPMin = min(AllFP)
        FPMax = max(AllFP)
        FPErr = (FPMax - FPMin)/2.0
        ret = [x_m.real , y_m.real , z_m.real, FNErr , F0Err , FRatioErr]
        print FPErr
        print ret
        return ret
    else:
        xl = Double(0.0)
        xu = Double(0.0)
        yl = Double(0.0)
        yu = Double(0.0)
        zl = Double(0.0)
        zu = Double(0.0)

        #I assume that the TVirtualFitter::GetFitter is set to MINUTE
        minuit = TVirtualFitter.Fitter(F,3)

        minuit.Clear()
    
        minuit.SetFitMethod("F3Minimizer")
        arg_list = array.array('d', [Double(-1.0)] )
        nNargs = int(1)
        minuit.ExecuteCommand("SET PRINT" , arg_list, nNargs)

        minuit.SetParameter(0, "x", x_m, 0.1, xl , xu )
        minuit.SetParameter(1, "y", y_m, 0.1, yl , yu )
        minuit.SetParameter(2, "z", z_m, 0.1, zl , zu )
        
        arg_list = array.array('d', [Double(1.0)] )
        fitResult = minuit.ExecuteCommand("MIGRAD", arg_list, 0)
        if not fitResult == 0 :
            print "Abnormal termination of minimization"
            return [-1.0 , -1.0 , -1.0]

        x_m = minuit.GetParameter(0)
        y_m = minuit.GetParameter(1)
        z_m = minuit.GetParameter(2)

        parabolicx = Double(0.0)
        parabolicy = Double(0.0)
        parabolicz = Double(0.0)
        globcc = Double(0.0)
        minuit.GetErrors( 0 , xu , xl , parabolicx , globcc )
        minuit.GetErrors( 1 , yu , yl , parabolicy , globcc )
        minuit.GetErrors( 2 , zu , zl , parabolicz , globcc )
        covarianceM = minuit.GetCovarianceMatrix()
        covariance = [ covarianceM[i] for i in range( 0 , 9) ]
        return [x_m.real , y_m.real , z_m.real ,parabolicx.real , parabolicy.real , parabolicz.real , covariance ]



def GetMin2D(F , CalcError = True ):
    x_m = Double(0.0)
    y_m = Double(0.0)

    F.GetMinimumXY(x_m , y_m)
    if not CalcError:
        return [x_m.real , y_m.real]    
    
    xl = Double(0.0)
    xu = Double(0.0)
    yl = Double(0.0)
    yu = Double(0.0)

    minuit = TVirtualFitter.Fitter(F,2)
    minuit.Clear()

    minuit.SetFitMethod("F2Minimizer")

    arg_list = array.array('d', [Double(-1.0)] )
    nNargs = int(1)

    minuit.ExecuteCommand("SET PRINT" , arg_list, nNargs)
    
    minuit.SetParameter(0, "x", x_m, 0.1, xl , xu )
    minuit.SetParameter(1, "y", y_m, 0.1, yl , yu )

    arg_list = array.array('d', [Double(1.0)] )
    fitResult = minuit.ExecuteCommand("MIGRAD", arg_list, 0)

    if not fitResult == 0 :
        print "Abnormal termination of minimization"
        return [-1.0 , -1.0]

    x_m = minuit.GetParameter(0)
    y_m = minuit.GetParameter(1)

    parabolicx = Double(0.0)
    parabolicy = Double(0.0)

    globcc = Double(0.0)
    minuit.GetErrors( 0 , xu , xl , parabolicx , globcc )
    minuit.GetErrors( 1 , yu , yl , parabolicy , globcc )
    return [x_m.real , y_m.real ,parabolicx.real , parabolicy.real]
    

class F32F2:
    def __init__(self , F3 , VarToFix , Value):
        self.F3 = F3
        self.VarToFix = VarToFix 
        self.Value = Value

    def __call__(self , x , p = 0):

        if not p == 0:
            self.F3.SetParameters( p )

        X = x[0]
        Y = x[1]
        Z = self.Value

        if self.VarToFix == 1:
            return self.F3.Eval( Z , X , Y)
        elif self.VarToFix == 2:
            return self.F3.Eval( X , Z , Y)
        elif self.VarToFix == 3:
            return self.F3.Eval( X , Y , Z)

    @staticmethod
    def GetFunction(name ,F3 , VarToFix , Value ):
        functor = F32F2(F3 , VarToFix , Value)
        gROOT.cd()
        ret = None

        if VarToFix == 1:
            ret = TF2( name , functor , F3.GetYmin() , F3.GetYmax() , F3.GetZmin() , F3.GetZmax() , F3.GetNpar() )
        elif VarToFix == 2:
            ret = TF2( name , functor , F3.GetXmin() , F3.GetXmax() , F3.GetZmin() , F3.GetZmax() , F3.GetNpar() )
        elif VarToFix == 3:
            ret = TF2( name , functor , F3.GetXmin() , F3.GetXmax() , F3.GetYmin() , F3.GetYmax() , F3.GetNpar() )

        return [ret , functor]
