#! /usr/bin/python -i

from ROOT import TCanvas,  TFile, THStack, TH1D, gROOT, TF1, TF3, TVirtualFitter, Double, TGraphErrors, TMultiGraph, TGraph
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


def GetCosThetaPlot(WhichChannel , wpol_file, weighted = True, gen = False):
    file = TFile( 'WPol_SelectedTTBars_%s.root' % wpol_file , "READ")
    cosThetaPlotName = 'costheta_%(channel)s/hCosThetaAllLepton' % {'channel':WhichChannel.lower()}
    if gen:
        cosThetaPlotName = 'costheta_%(channel)s/hCosThetaNegLepton_Gen_Fitted_Nins10' % {'channel':WhichChannel.lower()}
    if not weighted:
        cosThetaPlotName = 'costheta_%(channel)s/hCosThetaAllLeptonsUnWeighted' % {'channel':WhichChannel.lower()}

    hCosTheta = (file.Get(cosThetaPlotName))
    gROOT.cd()
    
    hret = None
    if gen:
        hret = hCosTheta.Clone("costheta_gen_"+wpol_file)
    else:
        hret = hCosTheta.Rebin(10, "costheta_"+wpol_file)
    file.Close()

    return hret


files_ee_weights = { 'TTBarSummer2011':0.192709, 'DYSummer2011':0.475306 ,'WJetsSummer2011':1.896160 , 'WWSummer2011':0.017588 , 'SingleTopSummer2011':0.077388 , 'SingleTopTWSummer2011':0.090660 , 'ZZSummer2011':0.005090 ,'WZSummer2011':0.002272  }
files_mm_weights = { 'TTBarSummer2011':0.189709, 'DYSummer2011':0.467907 ,'WJetsSummer2011':1.866643 , 'WWSummer2011':0.017314 , 'SingleTopSummer2011':0.076184 , 'SingleTopTWSummer2011':0.089249 , 'ZZSummer2011':0.005011 ,'WZSummer2011':0.002237 }
files_em_weights = { 'TTBarSummer2011':0.197058, 'DYSummer2011':0.486031 ,'WJetsSummer2011':1.938946 , 'WWSummer2011':0.017985 , 'SingleTopSummer2011':0.079134 , 'SingleTopTWSummer2011':0.092706 , 'ZZSummer2011':0.005205 ,'WZSummer2011':0.002323  }

files_weights = {'EM':files_em_weights , 'EE':files_ee_weights , 'MM':files_mm_weights }
files_data = {'EM':'ElectronMuon2011' , 'EE':'DoubleEle2011' , 'MM':'DoubleMuon2011'}

def MakeStack(WhichChannel):
    sorted_samples = ['DYSummer2011' ,'SingleTopTWSummer2011', 'WJetsSummer2011' , 'WWSummer2011' , 'SingleTopSummer2011' , 'WZSummer2011', 'ZZSummer2011' ]
    sorted_samples.reverse()

    files_xsec = files_weights[WhichChannel]

    stack_costheta = THStack("stackCosTheta",'CosTheta for ' + WhichChannel +' Events')

    colors = {'TTBarSummer2011':41, 'DYSummer2011':46 ,'WJetsSummer2011':31 , 'WWSummer2011':29 , 'SingleTopSummer2011':4 , 'SingleTopTWSummer2011':7 , 'WZSummer2011':90 , 'ZZSummer2011':66 }

    for wpol_file in sorted_samples:
        lumi_weight = files_xsec[ wpol_file ]
        hCosTheta = GetCosThetaPlot(WhichChannel , wpol_file)
        hCosTheta.Scale(lumi_weight)
        hCosTheta.SetTitle(wpol_file)
        hCosTheta.SetFillStyle(3004)
        hCosTheta.SetFillColor(colors[wpol_file])
        hCosTheta.SetLineColor(colors[wpol_file])
        stack_costheta.Add(hCosTheta)

    return stack_costheta


def CreateLLFunction(WhichChannel):
    stack = MakeStack(WhichChannel)

    hTTBar = GetCosThetaPlot(WhichChannel , 'TTBarSummer2011')

    hTTBarGen = GetCosThetaPlot(WhichChannel , 'TTBarSummer2011', True)
    fFitGen = hTTBarGen.GetListOfFunctions().At(0)
    fneg_std = 0.3 #fFitGen.GetParameter("FNeg")
    f0_std = 0.7 #fFitGen.GetParameter("F0")
    print "%F, %F" % (fneg_std , f0_std)
    
    lumi_weight = files_weights[WhichChannel]['TTBarSummer2011']
    hTTBar.Scale(lumi_weight)

    hData = GetCosThetaPlot(WhichChannel , files_data[WhichChannel])

    return LLFunction.GetLLFunction(WhichChannel , stack , hData , hTTBar , PoissonDistribution , fneg_std, f0_std)[0]


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

gFNeg = TGraphErrors()
gF0 = TGraphErrors()
gFPos = TGraphErrors()
gFNegstandard = TGraph()
hAxis = TH1D('hAxis' , 'Axis' , 3 , 0.5 ,3.5 )
hAxis.SetStats(0)
hAxis2 = hAxis.Clone('hAxis_0')
hAxis3 = hAxis.Clone('hAxis_p')
cNeg = TCanvas("cFNEG")
c0 = TCanvas("cF0")
cPos = TCanvas("cFPos")

mgFNeg = TMultiGraph("mgFNeg", "F_{-}")
def RunFit():
    pointID = 0
    for channel in ['MM' , 'EE' , 'EM']:
        print channel
        F = CreateLLFunction(channel)
        RetVals = GetMinimum(F)
        print RetVals
        hAxis.GetXaxis().SetBinLabel( pointID+1 , channel)
        hAxis2.GetXaxis().SetBinLabel( pointID+1 , channel)
        hAxis3.GetXaxis().SetBinLabel( pointID+1 , channel)

        gFNeg.SetPoint( pointID , RetVals[0] , pointID + 1 )
        gFNeg.SetPointError( pointID , RetVals[3] , 0.0 )

        gF0.SetPoint( pointID , RetVals[1] , pointID + 1 )
        gF0.SetPointError( pointID , RetVals[4] , 0.0 )

        gFPos.SetPoint( pointID , 1.0 - RetVals[0] - RetVals[1] , pointID + 1 )
        gFPos.SetPointError( pointID ,  sqrt( RetVals[3]*RetVals[3] + RetVals[4]*RetVals[4] ) , 0.0 )

        pointID = pointID + 1

    #prepare graph+shade
    for point in range(0,10 ,1 ):
        gFNegstandard.SetPoint( 2*point , 0.3 + 0.02 , 0.5+point*0.3)
    for point in range(1,10 ,1):
        gFNegstandard.SetPoint( 1+2*point , 0.3 - 0.02 , 0.5+point*0.3)


    cNeg.cd()
    hAxis.SetTitle('F_{-}')
    hAxis.Draw('HBAR')
    #mgFNeg.Add( gFNeg )
    #mgFNeg.Add(gFNegstandard)
    gFNeg.Draw('P')
    gFNegstandard.Draw('L')
    #mgFNeg.Draw('P')
    c0.cd()

    hAxis2.SetTitle('F_{0}')
    hAxis2.Draw('HBAR')
    gF0.Draw('P')
    
    cPos.cd()

    hAxis3.SetTitle('F_{+}')
    hAxis3.Draw('HBAR')
    gFPos.Draw('P')
    

    
RunFit()

        
