import FWCore.ParameterSet.Config as cms
from PrepareDataset import *

process = cms.Process("Test")
process.load("DataSamples")
process.load("DataSamples_DCap")
process.load("MCSamples")
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

LowPtHighPt = "Low"
if os.environ.has_key( 'LowPtHighPt' ):
    LowPtHighPt = os.environ['LowPtHighPt']

HTCutValue = 0.0
EleIso = 0.0
EleIsoMethod = 0
EleEt = 0
MuIso = 0.0
MuPt = 0.0
HighLeptonPt = 0.0
METSF = 0.0
if LowPtHighPt == "Low":
    HTCutValue = 300.0
    EleIso = 0.15
    EleIsoMethod = 21
    MuIso = 0.15
    MuPt = 5.0    
    EleEt = 0.0
    HighLeptonPt = 10.0
    METSF = 30
else:
    HTCutValue = 60.0
    EleIso = 0.1
    EleIsoMethod = 30
    MuIso = 0.1
    MuPt = 10.0    
    EleEt = 0.0
    HighLeptonPt = 20.0
    METSF = 20.0

CPUN = 1
if os.environ.has_key( 'CPUNumber' ):
    CPUN = int(os.environ['CPUNumber'])

__CopyTreeFileName =  "/tmp/hbakhshi/" + COPYTREEFILENAME

INPUT = ''
INPUTPSet = cms.PSet()
if os.environ.has_key( 'INPUT' ):
    INPUT = os.environ['INPUT']
    INPUTPSet = process.vpsets[INPUT][0]
else :
    raise Exception, "You should specify the input source via env-variable INPUT"

process.RunSetup = cms.untracked.PSet(
    CPU_Number = cms.untracked.int32(CPUN)
    )

process.ElectronCharge = cms.PSet(
    Name=cms.untracked.string("LowPtDiLeptonSkimmer"),
    TotalInput=cms.untracked.int32(-1),
    NumberOfEventsPerPrintLoop=cms.untracked.int32(100),
    Verbosity=cms.untracked.int32(0),
    OutFileName=cms.string("DiLep_" + LowPtHighPt + "_" + INPUT + ".root"),
    Inputs=cms.vstring(INPUT),
    IntLumi=cms.double(INPUTPSet.TotalNumberOfEvents.value() / INPUTPSet.XSec.value() ),
    
    CopyTree=cms.bool(True),
    CopyTreeFileName=cms.string(__CopyTreeFileName),
    
    HLTs=cms.vstring("HLTNoTrigger"),
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        ElectronSelection=cms.PSet(
            ID=cms.int32(0), #3
            IsoMethod=cms.int32(EleIsoMethod),
            IsoCutValue=cms.double(1.0), # 0.1
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(44), #44

            RejectCrack=cms.bool(True),

            D0Method=cms.int32(53), #change to 53
            D0Value=cms.double(0.02), #0.02

            RejectTrackerDrivensOnly=cms.bool(True),
            PtCut=cms.double(10.0),

            deltaRNearestMuon=cms.untracked.double(0.1), #0.1
            MuonSelection=cms.PSet(
                GlobalMuon=cms.bool(True),
                TrackerMuon=cms.bool(True),
                PtCut=cms.double(5.0),
                Eta=cms.double(2.4),
                NChi2=cms.double(10.0),
                EEm=cms.double(4),
                EHad=cms.double(6),
                IsoCut=cms.double(MuIso), # 0.1
                d0Cut=cms.double(0.02),
                NTrkHits=cms.int32(10),
                NMuHits=cms.int32(0)
            ),
            allChargesAgree=cms.untracked.bool(True), #True
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(EleEt) #10.0
        ),
        MuonSelection=cms.PSet(
            GlobalMuon=cms.bool(True),
            TrackerMuon=cms.bool(True),
            PtCut=cms.double(MuPt),
            Eta=cms.double(2.4),
            NChi2=cms.double(10.0),
            EEm=cms.double(4),
            EHad=cms.double(6),
            IsoCut=cms.double(1.0), # 0.1
            d0Cut=cms.double(0.02),
            NTrkHits=cms.int32(10),
            NMuHits=cms.int32(0)
        ),
        JetSelection=cms.PSet(
            checkID=cms.bool(True),
            PtCut=cms.double(30), #30
            Eta=cms.double(2.5),
            RejectCrack=cms.bool(False),
            DrElectron=cms.double(0.4),
            RejectElectronsInGapAfterJetSelection=cms.bool(False),
            DrMuon=cms.double(0.4),
            ElectronSelection=cms.PSet(
                ID=cms.int32(3), #3
                IsoMethod=cms.int32(EleIsoMethod),
                IsoCutValue=cms.double(EleIso), # 0.1
                IsoRelative=cms.bool(True),

                ConvRejMethod=cms.int32(44), #44

                RejectCrack=cms.bool(True),

                D0Method=cms.int32(53), #change to 53
                D0Value=cms.double(0.02), #0.02

                RejectTrackerDrivensOnly=cms.bool(True),
                PtCut=cms.double(10.0),

                deltaRNearestMuon=cms.untracked.double(0.1), #0.1
                allChargesAgree=cms.untracked.bool(True), #True
                CTFGSFChargesAgree=cms.untracked.bool(False),

                CaloEnergy=cms.untracked.double(EleEt) #10.0
            ),
            MuonSelection=cms.PSet(
                GlobalMuon=cms.bool(True),
                TrackerMuon=cms.bool(True),
                PtCut=cms.double(MuPt),
                Eta=cms.double(2.4),
                NChi2=cms.double(10.0),
                EEm=cms.double(4),
                EHad=cms.double(6),
                IsoCut=cms.double(0.15), # 0.1
                d0Cut=cms.double(0.02),
                NTrkHits=cms.int32(10),
                NMuHits=cms.int32(0)
            ),
        ),
        RejectInvMassLessThan12=cms.bool(True), #True
        RejectZRegion=cms.bool(True), #True #This rejects the event if after all cuts, the two leptons inv-mass lies between 76 and 106
        RejectLeptonsFromZ=cms.bool(True), #True #This removes all of the leptons from the same flavour that their inv-mass lies between 76 and 106
        METCutOF=cms.double(30.0),
        METCutSF=cms.double(METSF),
        HTCut=cms.double(HTCutValue),
        NJets=cms.int32(2),
        HighLeptonPt=cms.double(HighLeptonPt), #20.0

        LooseSelection=cms.bool(LowPtHighPt == "Low"),
        EnableEventControlPlots=cms.bool(True),
        EnableElectronControlPlots=cms.bool(True),
        EnableMuonControlPlots=cms.bool(True),
        EnableJetControlPlots=cms.bool(True),
        TriggerNames=cms.vstring("HLTNoTrigger"), #"HLTElectronTTBar","HLTSusySS","HLTElectronZandW","HLTMuon")

        SelectedEventTypesByDSName=cms.PSet( #Event types : 0.5 ee, 1.5 mm, 2.5 em, 3.5 me ; SS 0.25, OS -0.25
            #DiLeptonLowPTRunB=cms.vdouble(0.75,1.75,2.75,3.75),
            #DiLeptonLowPtRunA=cms.vdouble(0.75,1.75,2.75,3.75),
            #DiLeptonSelectionDSElectron=cms.vdouble(0.75,1.75,2.75,3.75)
        )
    ),
    Analyzers=cms.VPSet(
        )
    )

#process.ElectronCharge.SelectorConfigs.ElectronSelection.ConvRejMethod = 45
prepare_ds(process.vpsets[INPUT],20,False)
