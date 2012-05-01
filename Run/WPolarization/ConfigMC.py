import FWCore.ParameterSet.Config as cms
from PrepareDataset import *

process = cms.Process("Test")
process.load("DataSamples")
process.load("DataSamples_DCap")
process.load("MCSamples")
process.load("WPolData")
process.load("Triggers")

from ElectronSelection import *
from EventSelection import *

import os
STARTUPDIR = ''
if os.environ.has_key( 'STARTUPDIR' ):
    STARTUPDIR = os.environ['STARTUPDIR']
else :
    STARTUPDIR = os.getcwd()

COPYTREEFILENAME = ''
if os.environ.has_key( 'COPYTREEFILENAME' ):
    COPYTREEFILENAME = os.environ['COPYTREEFILENAME']
else :
    COPYTREEFILENAME = "CLONETREE.root"

CPUN = 1
if os.environ.has_key( 'CPUNumber' ):
    CPUN = int(os.environ['CPUNumber'])

__CopyTreeFileName =  STARTUPDIR + "/" + COPYTREEFILENAME

process.SyncTTBars = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/Syn/TTSyn.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SyncTTBars"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
process.Temp = cms.VPSet(
    cms.PSet(
        File=cms.string('/tmp/hbakhshi/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("Temp"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

INPUT = ''
INPUTPSet = cms.PSet()
if os.environ.has_key( 'INPUT' ):
    INPUT = os.environ['INPUT']    
else :
    raise Exception, "You should specify the input source via env-variable INPUT"

process.SelectedEvents = cms.VPSet(
    cms.PSet(
        File=cms.string('/home/hbakhshi/Documents/WPolarization/NTuples/TreeTTbar_' + INPUT.replace( 'SELECTED' , '') +  '.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SelectedTTBars_" + INPUT.replace( 'SELECTED' , '')),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

IsTTBar = False
if INPUT.find( 'TTBarSummer2011' ) >= 0:
    IsTTBar = True
if INPUT.find( 'SELECTED' ) >= 0:
    INPUT = 'SelectedEvents'
INPUTPSet = process.vpsets[INPUT][0]

process.RunSetup = cms.untracked.PSet(
    CPU_Number = cms.untracked.int32(CPUN)
    )

process.WPolarization = cms.PSet(
    Name=cms.untracked.string("WPolarization"),
    TotalInput=cms.untracked.int32(-1),
    NumberOfEventsPerPrintLoop=cms.untracked.int32(1000),
    Verbosity=cms.untracked.int32(0),
    OutFileName=cms.string("WPol_" + INPUTPSet.Name.value() + ".root"),
    Inputs=cms.vstring(INPUT),
    IntLumi=cms.double(INPUTPSet.TotalNumberOfEvents.value() / INPUTPSet.XSec.value() ),

    CopyTree=cms.bool(True), #True
    CopyTreeFileName=cms.string(__CopyTreeFileName),
    
    #HLTs=cms.vstring(),
    HLTs=cms.vstring( "HLTDiMuonMCTTBar2011","HLTDiElectronMCTTBar2011", "HLTMuElMCTTBar2011"),  #MC
    #HLTs=cms.vstring("HLTEETTBar2011Data", "HLTEMuTTBar2011Data" , "HLTMuMuTTBar2011Data"),  #DATA
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        EventsRunID=cms.VPSet(
        ),
        ElectronSelection=cms.PSet(
            ID=cms.int32(8), #tight
            IsoMethod=cms.int32(21), #combined 03 (ElRelIso03)
            IsoCutValue=cms.double(0.15), # 0.15
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(46), #hybrid2

            RejectCrack=cms.bool(True), #i am not sure if it is needed, twiki : not

            D0Method=cms.int32(53), #beamspot
            D0Value=cms.double(0.04), #0.04

            RejectTrackerDrivensOnly=cms.bool(True),
            PtCut=cms.double(20.0),

            deltaRNearestMuon=cms.untracked.double(0.1), #0.1 from all muons, in twiki : all muons
            MuonSelection=cms.PSet(
                GlobalMuon=cms.bool(True),
                TrackerMuon=cms.bool(True),
                PtCut=cms.double(0.0),
                Eta=cms.double(8.0),
                NChi2=cms.double(1000000.0),
                EEm=cms.double( 10000000.0),
                EHad=cms.double(10000000.0),
                IsoCut=cms.double(10000000.0), # 0.2
                d0Cut=cms.double(10000000.0),
                NTrkHits=cms.int32(-1),
                NMuHits=cms.int32(-1)
            ),
            allChargesAgree=cms.untracked.bool(False), #True
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(0.0) # no cut is needed
        ),
        MuonSelection=cms.PSet(
            GlobalMuon=cms.bool(True),
            TrackerMuon=cms.bool(True),
            PtCut=cms.double(20.0),
            Eta=cms.double(2.4),
            NChi2=cms.double(10.0),
            EEm=cms.double( 1000000.0),
            EHad=cms.double(1000000.0),
            IsoCut=cms.double(0.2), # 0.2
            d0Cut=cms.double(0.02),
            NTrkHits=cms.int32(10),
            NMuHits=cms.int32(0)
        ),
        JetSelection=cms.PSet(
            checkID=cms.bool(True),
            PtCut=cms.double(30), #30
            Eta=cms.double(2.5),
            RejectCrack=cms.bool(False),
            DrElectron=cms.double(-1.0), #0.4
            RejectElectronsInGapAfterJetSelection=cms.bool(False),
            DrMuon=cms.double(-1.0), #0.4
        ),
        TTBarSample = cms.untracked.bool( IsTTBar ),
        MCStudy = cms.untracked.bool(False),
        RejectInvMassLessThan12=cms.bool(True), #True
        RejectZRegion=cms.bool(True), #True #This rejects the event if after all cuts, the two leptons inv-mass lies between 76 and 106
        RejectLeptonsFromZ=cms.bool(False), #True #This removes all of the leptons from the same flavour that their inv-mass lies between 76 and 106
        METCutOF=cms.double(20.0), #20
        METCutSF=cms.double(30.0), #30
        NJets=cms.int32(2), #2
        btag_algo = cms.string('simpleSecondaryVertexHighEff'), #TrackCountingHighEff , simpleSecondaryVertexHighEff
        BJetSelectionBTag = cms.double( 1.74 ), #to count the number of bjets
        NBJets = cms.int32(-1),
        BTag1 = cms.double(-100000.0),
        BTag2 = cms.double(-100000.0),

        DRJetsLeptons = cms.double(0.4), #it applies on jet after the two leptons of ttbar are selected

        PUWFiles=cms.string('./Pileup.root'),
        Data=cms.bool(False),

        LooseSelection=cms.bool(False),
        EnableEventControlPlots=cms.bool(True),
        EnableElectronControlPlots=cms.bool(True),
        EnableMuonControlPlots=cms.bool(True),
        EnableJetControlPlots=cms.bool(True),
        #TriggerNames=cms.vstring(), 
        TriggerNames=cms.vstring( "HLTDiMuonMCTTBar2011", "HLTDiElectronMCTTBar2011",  "HLTMuElMCTTBar2011"), #MC
        #TriggerNames=cms.vstring("HLTEETTBar2011Data", "HLTEMuTTBar2011Data" , "HLTMuMuTTBar2011Data"), #DATA
        
        ##NONE
        #DiElectronTrigger=cms.vstring(), 
        DiElectronTrigger_Veto=cms.vstring(), 
        #DiMuonTrigger=cms.vstring(), 
        DiMuonTrigger_Veto=cms.vstring(), 
        #ElectronMuonTrigger=cms.vstring(), 
        ElectronMuonTrigger_Veto=cms.vstring(), 
        ##END_NONE

        ##DATA
        #DiElectronTrigger=cms.vstring("HLTEETTBar2011Data"), 
        #DiElectronTrigger_Veto=cms.vstring("HLTEMuTTBar2011Data" , "HLTMuMuTTBar2011Data"), 
        #DiMuonTrigger=cms.vstring("HLTMuMuTTBar2011Data"), 
        #DiMuonTrigger_Veto=cms.vstring("HLTEETTBar2011Data", "HLTEMuTTBar2011Data"), 
        #ElectronMuonTrigger=cms.vstring("HLTEMuTTBar2011Data"), 
        #ElectronMuonTrigger_Veto=cms.vstring("HLTEETTBar2011Data", "HLTMuMuTTBar2011Data"), 
        ##END_DATA
        
        ##MC
        DiElectronTrigger=cms.vstring("HLTDiElectronMCTTBar2011"), 
        # DiElectronTrigger_Veto=cms.vstring("HLTMuElMCTTBar2011" , "HLTDiMuonMCTTBar2011"), 
        DiMuonTrigger=cms.vstring("HLTDiMuonMCTTBar2011"), 
        # DiMuonTrigger_Veto=cms.vstring("HLTDiElectronMCTTBar2011", "HLTMuElMCTTBar2011"), 
        ElectronMuonTrigger=cms.vstring("HLTMuElMCTTBar2011"), 
        # ElectronMuonTrigger_Veto=cms.vstring("HLTDiElectronMCTTBar2011", "HLTDiMuonMCTTBar2011"), 
        ##END_MC
        
        SelectedEventTypesByDSName=cms.PSet( #Event types : 0.5 ee, 1.5 mm, 2.5 em, 3.5 me ; SS 0.25, OS -0.25
            #TTBarSummer2011 = cms.vdouble(0.25 , 0.75)
        )
    ),
    Analyzers=cms.VPSet(
        cms.PSet(
            Name=cms.string("neutrino_solver_ee"),
            Type=cms.string("neutrino_solver"),
            EventTypes=cms.vdouble( 0.25 ),
            bJetAssigner = cms.PSet (
                method = cms.string("ranodm_firstbs"),
                Name=cms.string("random_bAssigner")
                )
            ),
        cms.PSet(
            Name=cms.string("neutrino_solver_mm"),
            Type=cms.string("neutrino_solver"),
            EventTypes=cms.vdouble( 1.25 ),
            bJetAssigner = cms.PSet (
                method = cms.string("ranodm_firstbs"),
                Name=cms.string("random_bAssigner")
                )
            ),
        cms.PSet(
            Name=cms.string("neutrino_solver_em"),
            Type=cms.string("neutrino_solver"),
            EventTypes=cms.vdouble( 2.25 , 3.25 ),
            bJetAssigner = cms.PSet (
                method = cms.string("ranodm_firstbs"),
                Name=cms.string("random_bAssigner")
                )
            ),
        cms.PSet(
            Name=cms.string("neutrino_solver_all"),
            Type=cms.string("neutrino_solver"),
            EventTypes=cms.vdouble( 2.25 , 3.25 , 1.25 , 0.25),
            bJetAssigner = cms.PSet (
                method = cms.string("ranodm_firstbs"),
                Name=cms.string("random_bAssigner")
                )
            ),
        cms.PSet(
            Name=cms.string("costheta_DiLep"),
            Type=cms.string("costheta"),
            EventTypes=cms.vdouble( 2.25 , 3.25 , 1.25 , 0.25 ),
            SolverName = cms.string("neutrino_solver_all"),
            SolverSolution = cms.int32(0),
            FillGen=cms.bool(IsTTBar),
            GenDecayModes=cms.vint32(1,2,3,4),
            FillRec=cms.bool(True)
            ),
        cms.PSet(
            Name=cms.string("costheta_ee"),
            Type=cms.string("costheta"),
            EventTypes=cms.vdouble( 0.25 ),
            SolverName = cms.string("neutrino_solver_ee"),
            SolverSolution = cms.int32(0),
            FillGen=cms.bool(IsTTBar),
            GenDecayModes=cms.vint32(3),
            FillRec=cms.bool(True)
            ),
        cms.PSet(
            Name=cms.string("costheta_mm"),
            Type=cms.string("costheta"),
            EventTypes=cms.vdouble( 1.25 ),
            SolverName = cms.string("neutrino_solver_mm"),
            SolverSolution = cms.int32(0),
            FillGen=cms.bool(IsTTBar),
            GenDecayModes=cms.vint32(4),
            FillRec=cms.bool(True)
            ),
        cms.PSet(
            Name=cms.string("costheta_em"),
            Type=cms.string("costheta"),
            EventTypes=cms.vdouble( 2.25 , 3.25 ),
            SolverName = cms.string("neutrino_solver_em"),
            SolverSolution = cms.int32(0),
            FillGen=cms.bool(IsTTBar),
            GenDecayModes=cms.vint32(1 , 2), #is hard-coded as em==me
            FillRec=cms.bool(True)
            )
#       cms.PSet(
#           Name=cms.string("GenRecComparison_ee"),
#           Type=cms.string("GenRecComparison"),
#           EventTypes=cms.vdouble( 0.25  ),
#           SolverName = cms.string("neutrino_solver_ee"),
#           SolverSolution = cms.int32(0)
#       )
    )
)

#prepare_ds(process.vpsets[INPUT],20,False)
