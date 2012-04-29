#! /usr/bin/python
#! /afs/cern.ch/sw/lcg/external/Python/2.6.5/x86_64-slc5-gcc43-opt/bin/python

from ROOT import  TH2D,  TRandom, TH1F
import sys

from Fit import *

SeedGenerator = TRandom( 1928374 )
SeedGeneratorLumiEQ = TRandom( 874546620 )

class DistributionProducerFromSelected:
    def __init__(self , hSelected , MCName , lumi, N0):
        self.files_xsec = { 'TTBarSummer2011':157.5, 'DYSummer2011':3048.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 , 'WZSummer2011':0.6 , 'ZZSummer2011':4.65 }
        

        self.N0 = N0
        self.MCName = MCName
        self.hSelected = hSelected
        self.Lumi = lumi

        RandomGenerator = TRandom( SeedGenerator.Integer(10000000) )
        self.Events = {}
        for BinId in range( 1, self.hSelected.GetNbinsX()+1 ):
            BinValue = self.hSelected.GetBinCenter(BinId)
            for EventId in range(1 , int(self.hSelected.GetBinContent( BinId ))+1 ):
                envRndId = RandomGenerator.Integer( 10000000000  )
                while self.Events.has_key( envRndId ):
                    envRndId = RandomGenerator.Integer( 10000000000 )

                self.Events[ envRndId ] = BinValue
                
        self.SelEff = float( len(self.Events) ) / float(self.N0)
        
    def GeneratePartialSample(self, fraction, nPEX):
        RandomGenerator = TRandom( SeedGenerator.Integer(10000000) )
        SelectedValues = []
        for EvtID in self.Events.keys():
            if RandomGenerator.Uniform() < fraction:
                SelectedValues.append( self.Events[EvtID] )
        
        Weight = 0.0
        if not len(SelectedValues)==0 :            
            Weight = self.Lumi*self.files_xsec[ self.MCName ] / (fraction*self.N0)
        hName = ("%(SampleName)s_" +  self.hSelected.GetName() + "_%(NPEX)d") % {'SampleName':self.MCName , 'NPEX':nPEX}
        hTitle = ("%(SampleName)s : " +  self.hSelected.GetTitle() + " (%(NPEX)d)") % {'SampleName':self.MCName , 'NPEX':nPEX}
        gROOT.cd()
        hRet = TH1F( hName , hTitle , self.hSelected.GetNbinsX() , self.hSelected.GetXaxis().GetXmin() , self.hSelected.GetXaxis().GetXmax() )
        hRet.Sumw2()
        for i in SelectedValues:
            hRet.Fill( i , Weight )

        return hRet
    def GeneratePartialSampleLumiEQ(self, nPEX):        
        RandomGenerator = TRandom( SeedGeneratorLumiEQ.Integer(10000000) )
        SelectedValues = []
        nSelectedEventsInLumi_ = self.Lumi*self.files_xsec[ self.MCName ]*self.SelEff
        nSelectedEventsInLumi = RandomGenerator.Gaus(nSelectedEventsInLumi_, sqrt(nSelectedEventsInLumi_))
        EventIndices = self.Events.keys()
        for nEvt in range(0,int(nSelectedEventsInLumi) ):
            EvtID = RandomGenerator.Integer( len(EventIndices) )
            SelectedValues.append( self.Events[ EventIndices[EvtID] ] )
              
        hName = ("%(SampleName)s_LumiEQ_" +  self.hSelected.GetName() + "_%(NPEX)d") % {'SampleName':self.MCName , 'NPEX':nPEX}
        hTitle = ("%(SampleName)s : LumiEQ " +  self.hSelected.GetTitle() + " (%(NPEX)d)") % {'SampleName':self.MCName , 'NPEX':nPEX}
        gROOT.cd()
        hRet = TH1F( hName , hTitle , self.hSelected.GetNbinsX() , self.hSelected.GetXaxis().GetXmin() , self.hSelected.GetXaxis().GetXmax() )
        hRet.Sumw2()
        for i in SelectedValues:
            hRet.Fill( i )

        return hRet

class DistributionProducerFromAll:
    def __init__(self , N0 , hSelected , MCName , lumi ,valForNotSelectedEvents = -100000):
        self.files_xsec = { 'TTBarSummer2011':157.5, 'DYSummer2011':3048.0 ,'WJetsSummer2011':31314.0 , 'WWSummer2011':4.65 , 'SingleTopSummer2011':64.5 , 'SingleTopTWSummer2011':2*7.9 , 'WZSummer2011':0.6 , 'ZZSummer2011':4.65 }
        self.N0 = N0
        self.MCName = MCName
        self.hSelected = hSelected

        self.Lumi = lumi

        self.ValueForNotSelectedEvents =  valForNotSelectedEvents
        RandomGenerator = TRandom( SeedGenerator.Integer(10000000) )

        self.Events = {}

        for BinId in range( 1 , self.hSelected.GetNbinsX()+1 ):
            BinValue = self.hSelected.GetBinCenter(BinId)
            for EventId in range( 1 , int(self.hSelected.GetBinContent( BinId ))+1 ):
                envRndId = RandomGenerator.Integer( int(self.N0) )
                while self.Events.has_key( envRndId ):
                    envRndId = RandomGenerator.Integer( int(self.N0) )

                self.Events[ envRndId ] = BinValue

    def GetEvent( self, eventId ):
        if eventId < self.N0 :
            if self.Events.has_key(eventId):
                return self.Events[eventId]
            else:
                return self.ValueForNotSelectedEvents

    def GeneratePartialSample( self, fraction , nPEX):
        NPartial = self.N0*fraction
        RandomGenerator = TRandom( SeedGenerator.Integer(10000000) )

        __SelectedEventsIDs = []
        SelectedEvents = []
        while len(SelectedEvents) < NPartial:
            rnd = RandomGenerator.Integer( int(self.N0) )
            while not __SelectedEventsIDs.count( rnd ) == 0 :
                rnd = RandomGenerator.Integer( int(self.N0) )
            
            __SelectedEventsIDs.append(rnd)
            SelectedEvents.append( self.GetEvent(rnd) )

        Weight = self.Lumi*self.files_xsec[ self.MCName ] / (NPartial)
        hName = ("%(SampleName)s_" +  self.hSelected.GetName() + "_%(NPEX)d") % {'SampleName':self.MCName , 'NPEX':nPEX}
        hTitle = ("%(SampleName)s : " +  self.hSelected.GetTitle() + " (%(NPEX)d)") % {'SampleName':self.MCName , 'NPEX':nPEX}
        gROOT.cd()
        hRet = TH1F( hName , hTitle , self.hSelected.GetNbinsX() , self.hSelected.GetXaxis().GetXmin() , self.hSelected.GetXaxis().GetXmax() )
        hRet.Sumw2()
        for i in SelectedEvents:
            if not i == self.ValueForNotSelectedEvents :
                hRet.Fill( i , Weight )

        return hRet

def ReadN0( Sample , Channel , Wpol_all_location):
    file = TFile( '%(AllWPols)s/WPol_%(Sample)s.root' % { 'Sample':Sample , 'AllWPols':Wpol_all_location} , "READ")
    selectionPlotName = 'Selection/hEventSelection%(channel)s' % {'channel':Channel}
    hSelection = file.Get( selectionPlotName )
    N0 = hSelection.GetBinContent(1)
    file.Close()
    return N0

def RunFitValidation(Channel , Wpol_all_location , StartPEX, LPEX, StartPEXPull , LPEXPull):
    print '%(File)s %(Channel)s %(AllEventsHistogramDirectory)s %(FirstPEX)d %(PEXs)d %(StartPEXPull)s %(LPEXPull)s' % {'File':sys.argv[0] , 'Channel':Channel, 'AllEventsHistogramDirectory':Wpol_all_location, 'FirstPEX':StartPEX , 'PEXs':LPEX , 'StartPEXPull':StartPEXPull , 'LPEXPull':LPEXPull}
    BKG_samples = {'DYSummer2011':None ,'SingleTopTWSummer2011':None, 'WJetsSummer2011':None , 'WWSummer2011':None , 'SingleTopSummer2011':None , 'WZSummer2011':None, 'ZZSummer2011':None }
    Channels = {'EE':4529.518 , 'MM':4459.007 , 'EM':4631.724 }
    Lumi = Channels[Channel]
    FPosFixed = 0.0009

    TTBarWeightor = WeightFunction.GetWeightFunction('TTBarWeightor' )

    hTTBar = GetCosThetaPlot(Channel , 'TTBarSummer2011' , False)
    hTTBar.Sumw2()
    nTTBar = ReadN0( 'TTBarSummer2011', Channel , Wpol_all_location )
    #TTBarDistributionProducer = DistributionProducerFromAll( nTTBar , hTTBar , 'TTBarSummer2011' , Lumi )
    TTBarDistributionProducer = DistributionProducerFromSelected(  hTTBar , 'TTBarSummer2011' , Lumi , nTTBar)

    TTasMC = hTTBar.Clone('TTasMC')
    TTasMC.Scale(float(Lumi*TTBarDistributionProducer.files_xsec['TTBarSummer2011'])/float(nTTBar))

    gROOT.cd()
    stack_costheta = THStack("stackCosTheta",'CosTheta for ' + Channel +' Events')

    for sample in BKG_samples.keys():
        h = GetCosThetaPlot( Channel , sample, False )
        h.Sumw2()
        NTot = ReadN0(sample, Channel , Wpol_all_location )
        #dpfa = DistributionProducerFromAll( NTot , h , sample , Lumi)
        dpfa = DistributionProducerFromSelected(  h , sample , Lumi, NTot)
        BKG_samples[sample] = dpfa
        h.Scale(float(Lumi*dpfa.files_xsec[sample])/float(NTot))
        stack_costheta.Add(h)

    #stack_costheta.Add(TTasMC)
    gROOT.cd()
    hFinalFNeg = TH2D('hFinalFNeg','Linearirty Check for F_{-};F_{-} input;F_{-} output',300,0.,1.,300,0.,1.)
    hFinalF0 = TH2D('hFinalF0','Linearirty Check for F_{0};F_{0} input;F_{0} output',300,0.,1.,300,0.,1.)
    hFinalFPos = TH2D('hFinalFPos','Linearirty Check for F_{+};nPEX;F_{+} output',300,0.,300., 1000 , -0.5 , 0.5)

    for nPEX in range( 0 , 300):
        print 'PEX: %(PEX)d'%{'PEX':nPEX},

        if nPEX >= StartPEX + LPEX :
            print ' ... Skipped'
            sys.stdout.flush()
            continue

        if nPEX < StartPEX:  
            for i in range(0 , len(BKG_samples)+1):
                SeedGenerator.Integer( 10000 )
            print ' ... Skipped'
            sys.stdout.flush()
            continue
         

        name__ = 'CosTheta_%(Channel)s_%(nPEX)d' % {'Channel':Channel , 'nPEX':nPEX}
        title__ = 'cos(#theta) in %(Channel)s events : PEX=%(nPEX)d' % {'Channel':Channel , 'nPEX':nPEX}
               
        hSumBG = TH1F( name__ , title__ , hTTBar.GetNbinsX() , hTTBar.GetXaxis().GetXmin() , hTTBar.GetXaxis().GetXmax() )
        hSumBG.Sumw2()

        for sample in BKG_samples.keys():
            print '...' + sample + '...' ,
            sys.stdout.flush()
            hSumBG.Add( BKG_samples[sample].GeneratePartialSample( 1.0/3.0 , nPEX ) )

        hTTBarPartial = TTBarDistributionProducer.GeneratePartialSample(  1.0/3.0 , nPEX )
        print 'TTBar'

        nFSteps = 300
        for FNegValue in [ float(nFStep)/nFSteps for nFStep in  range( 0 , nFSteps) ]:
            if (FNegValue*nFSteps) % 50 == 0:
                print '\tFStep: %(FStep)F' % {'FStep':FNegValue }
            sys.stdout.flush()
            if FNegValue > 1.0-FPosFixed:
                continue

            F0Value = 1.0 - FPosFixed - FNegValue

            TTBarWeightor.SetParameters( FNegValue , F0Value )
            name__ =  hSumBG.GetName()+'%(FNegValue)f' % {'FNegValue': FNegValue}
            title__ = hSumBG.GetTitle()+' for F_ = %(FNegValue)f' % {'FNegValue': FNegValue}
            TTinPEX = hTTBarPartial.Clone(name__)
            TTinPEX.SetTitle(title__)
            TTinPEX.Multiply(TTBarWeightor,1)
            TTinPEX.Add(hSumBG)
            LLinPEXforFNegValueArray = LLFunction.GetLLFunction('F_'+name__ , stack_costheta,TTinPEX,TTasMC,PoissonDistribution,0.3,0.7)
            LLinPEXforFNegValue = LLinPEXforFNegValueArray[0]
            #stack_costheta.Draw()
            #hSumBG.Draw('SAME')
            #TTinPEX.Draw('SAME')
            x = GetMinimum(LLinPEXforFNegValue,False)
            hFinalFNeg.Fill(FNegValue,x[0])
            hFinalF0.Fill(F0Value,x[1])
            hFinalFPos.Fill(nPEX,1.0-x[0]-x[1])

            del LLinPEXforFNegValueArray[1].hSum
            del LLinPEXforFNegValueArray[1]
            del LLinPEXforFNegValue


    hPullFNeg = TH1D('hPullFNeg','Pull distribution for F_{-}',1000,-5,5)
    hPullF0 = TH1D('hPullF0','Pull distribution for F_{0}',1000,-5,5)
    hPullFPos = TH1D('hPullFPos','Pull distribution for F_{+}',1000,-5,5)
    hPullFResp = TH1D('hPullFResp','Pull distribution for F_{resp}',1000,-5,5)

    hResFNeg  = TH1D('hResFNeg','Residual for F_{-}',1000,-0.5,0.5)
    hResF0 = TH1D('hResF0','Residual for F_{0}',1000,-0.5,0.5)
    hResFPos = TH1D('hResFPos','Residual for F_{+}',1000,-0.5,0.5)
    hResFResp = TH1D('hResFResp','Residual for F_{resp}',1000,-0.5,0.5)

    for nPEXPull in range( 0 , 1000):
        print 'PEXPull: %(PEX)d'%{'PEX':nPEXPull},

        if nPEXPull >= StartPEXPull + LPEXPull :
            print ' ... Skipped pull'
            sys.stdout.flush()
            continue

        if nPEXPull < StartPEXPull:  
            for i in range(0 , len(BKG_samples)+1):
                SeedGeneratorLumiEQ.Integer( 10000 )
            print ' ... Skipped pull'
            sys.stdout.flush()
            continue
         

        name__ = 'CosTheta_%(Channel)s_Pull_%(nPEX)d' % {'Channel':Channel , 'nPEX':nPEXPull}
        title__ = 'cos(#theta) in %(Channel)s events : PEXPull=%(nPEX)d' % {'Channel':Channel , 'nPEX':nPEXPull}
               
        hData = TH1F( name__ , title__ , hTTBar.GetNbinsX() , hTTBar.GetXaxis().GetXmin() , hTTBar.GetXaxis().GetXmax() )
        hData.Sumw2()

        for sample in BKG_samples.keys():
            print '...' + sample + '...' ,
            sys.stdout.flush()
            hData.Add( BKG_samples[sample].GeneratePartialSampleLumiEQ(  nPEXPull ) )

        hData.Add( TTBarDistributionProducer.GeneratePartialSampleLumiEQ(  nPEXPull ) )
        print 'TTBar'

        LLinPEXforFNegValueArray = LLFunction.GetLLFunction('F_'+name__ , stack_costheta,hData,TTasMC,PoissonDistribution,0.3,0.7)
        LLinPEXforFNegValue = LLinPEXforFNegValueArray[0]
        x = GetMinimum(LLinPEXforFNegValue,True)
        fneg = x[0]
        f0 = x[1]
        fpos = 1.0 - x[1] - x[0]

        fresponse = x[2]
        errfrespo = x[5]
        resfresp0 = fresponse - 1.0
        
        hPullFResp.Fill( resfresp0/errfrespo )
        hResFResp.Fill( resfresp0 )
        
        errfneg = x[3]
        errf0 = x[4]
        errfpos = sqrt( errf0*errf0 + errfneg * errfneg)

        resneg = fneg-0.3
        respos = fpos - FPosFixed
        resf0 = f0 - 0.7

        hPullFNeg.Fill(float(resneg) / errfneg)
        hPullF0.Fill(float(resf0) / errf0)
        hPullFPos.Fill(float(respos) / errfpos)

        hResFPos.Fill( respos )
        hResFNeg.Fill( resneg )
        hResF0.Fill( resf0 )

        del LLinPEXforFNegValueArray[1].hSum
        del LLinPEXforFNegValueArray[1]
        del LLinPEXforFNegValue
        
    

    f = TFile('ValidationFor%(Channel)s_%(From)03d_%(To)03d_Pull%(FromPull)03d_%(ToPull)03d.root'%{'Channel':Channel , 'From':StartPEX , 'To':LPEX+StartPEX , 'ToPull':StartPEXPull+LPEXPull , 'FromPull':StartPEXPull},'recreate')
    f.cd()
    hFinalFNeg.Write()
    hFinalFNeg.ProfileX().Write()
    hFinalFNeg.ProfileY().Write()
    hFinalFPos.Write()
    hFinalF0.Write()

    hPullFPos.Write()
    hPullFNeg.Write()
    hPullF0.Write()
    hPullFResp.Write()

    hResF0.Write()
    hResFNeg.Write()
    hResFPos.Write()
    hResFResp.Write()

    f.Close()

if not len(sys.argv) == 7:
    print 'wrong argument list'
    print 'usage : %(File)s ChannelName AllEventsHistogramDirectory #FirstPEX #PEXs #FirstPullPEX #PullPEXs' % {'File':sys.argv[0]}
else:
    ChannelName = sys.argv[1]
    AllEventsHistogramDirectory = sys.argv[2]
    StartPEX = int( sys.argv[3] )
    LPEX = int( sys.argv[4] )
    StartPEXPull = int( sys.argv[5] )
    LPEXPull = int( sys.argv[6] )
    RunFitValidation( ChannelName , AllEventsHistogramDirectory , StartPEX, LPEX , StartPEXPull , LPEXPull)
    
