#! /usr/bin/python -i

from ROOT import TCanvas,  TFile, THStack, TH1D, gROOT, TF1, TF3, TVirtualFitter, Double, TGraphErrors, TMultiGraph, TGraph, TF2
import array
from math import sqrt, exp, pi, pow, factorial, log
import os

class dGammaFunctin:
    def __init__(self):
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
    
        ret = (FNeg*3.0*aa / 8.0)
        ret += (F0*3.0*bb / 4.0 )
        ret += (FPos*3.0*cc / 8.0)

        return norm_factor*ret

    @staticmethod
    def GetFunction(name):
        functor = dGammaFunctin()
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

class LLFunction:

    def NCosThetaBins(self):
        return self.hData.GetNbinsX()

    def CosThetaRange(self):
        return range(1, self.hData.GetNbinsX()+1)

    def __init__(self , stack , hData , hTTBar , distribution_function, std_fneg, std_f0):
        self.hData = hData
        #SET DATA ERROR
        for nBin in self.CosThetaRange():
            err = sqrt(self.hData.GetBinContent(nBin))
            self.hData.SetBinError(nBin, err)

        #MAKE A TH OUT OF THE STACK, and calc errors
        self.hSum = TH1D('hSum' , 'SUM_MC' , self.NCosThetaBins() , -1 , 1)

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
            WeightVal = self.WF1.Eval(CosTheta)
            N_TT = x[2]*WeightVal*self.TTBar.GetBinContent(nBin)

            N_MC = N_BKG + N_TT

            N_Data = self.hData.GetBinContent(nBin)

            LL += self.DistributionFunction( N_MC , N_Data )
        
        return LL


    @staticmethod
    def GetLLFunction(name , stack , hData , hTTBar , distribution_function,std_fneg ,std_f0):
        
        functor = LLFunction( stack , hData , hTTBar , distribution_function, std_fneg, std_f0)

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

def GetMinimum(F,CalcError = True):
    # based on the documentation of TF3::GetMinimumXYZ from
    # http://root.cern.ch/root/html532/src/TF3.cxx.html#QUjxjE
    x_m = Double(0.0)
    y_m = Double(0.0)
    z_m = Double(0.0)

    F.GetMinimumXYZ(x_m , y_m , z_m)
    if not CalcError:
        return [x_m.real , y_m.real , z_m.real]
    #print "%(x)F ; %(y)F ; %(z)F" % {'x':x_m.real , 'y':y_m.real , 'z':z_m.real}

    xl = Double(0.0)
    xu = Double(0.0)
    yl = Double(0.0)
    yu = Double(0.0)
    zl = Double(0.0)
    zu = Double(0.0)
    #F.GetRange(xl , yl , zl , xu , yu , zu )

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
    #print '%F , %F' % (parabolic , globcc)
    minuit.GetErrors( 1 , yu , yl , parabolicy , globcc )
    #print '%F , %F' % (parabolic , globcc)
    minuit.GetErrors( 2 , zu , zl , parabolicz , globcc )
    #print '%F , %F' % (parabolic , globcc)
    #print "%(x)F + %(xu)F - %(xl)F ; %(y)F  + %(yu)F - %(yl)F; %(z)F  + %(zu)F - %(zl)F" % {'x':x_m.real , 'xu':xu.real , 'xl':xl.real , 'yl':yl.real , 'yu':yu.real , 'zu':zu.real , 'zl':zl.real , 'y':y_m.real , 'z':z_m.real}
    return [x_m.real , y_m.real , z_m.real ,parabolicx.real , parabolicy.real , parabolicz.real ]



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
