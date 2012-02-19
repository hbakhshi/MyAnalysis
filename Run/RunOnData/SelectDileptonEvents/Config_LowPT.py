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

HTCutValue = 30
if os.environ.has_key( 'HTCutValue' ):
    HTCutValue = os.environ['HTCutValue']


CPUN = 1
if os.environ.has_key( 'CPUNumber' ):
    CPUN = int(os.environ['CPUNumber'])

__CopyTreeFileName =  STARTUPDIR + "/" + COPYTREEFILENAME

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
    OutFileName=cms.string("DiLep_Low_" + INPUT + ".root"),
    Inputs=cms.vstring(INPUT),
    IntLumi=cms.double(INPUTPSet.TotalNumberOfEvents.value() / INPUTPSet.XSec.value() ),
    
    CopyTree=cms.bool(False),
    CopyTreeFileName=cms.string(__CopyTreeFileName),
    
    HLTs=cms.vstring("HLTNoTrigger"),
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        ElectronSelection=cms.PSet(
            ID=cms.int32(3), #3
            IsoMethod=cms.int32(21),
            IsoCutValue=cms.double(0.15), # 0.1
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(44), #44

            RejectCrack=cms.bool(True),

            D0Method=cms.int32(53), #change to 53
            D0Value=cms.double(0.02), #0.02

            RejectTrackerDrivensOnly=cms.bool(True),
            PtCut=cms.double(10.0),

            deltaRNearestMuon=cms.untracked.double(0.1), #0.1
            allChargesAgree=cms.untracked.bool(False), #True
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(0.0) #10.0
        ),
        MuonSelection=cms.PSet(
            GlobalMuon=cms.bool(True),
            TrackerMuon=cms.bool(True),
            PtCut=cms.double(5.0),
            Eta=cms.double(2.4),
            NChi2=cms.double(10.0),
            EEm=cms.double(4),
            EHad=cms.double(6),
            IsoCut=cms.double(0.15), # 0.1
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
            DrMuon=cms.double(0.4)
        ),
        RejectInvMassLessThan12=cms.bool(True), #True
        RejectZRegion=cms.bool(True), #True #This rejects the event if after all cuts, the two leptons inv-mass lies between 76 and 106
        RejectLeptonsFromZ=cms.bool(True), #True #This removes all of the leptons from the same flavour that their inv-mass lies between 76 and 106
        METCutOF=cms.double(30.0),
        METCutSF=cms.double(30.0),
        HTCut=cms.double(HTCutValue),
        NJets=cms.int32(2),
        HighLeptonPt=cms.double(5.0), #20.0

        LooseSelection=cms.bool(True),
        EnableEventControlPlots=cms.bool(True),
        EnableElectronControlPlots=cms.bool(True),
        EnableMuonControlPlots=cms.bool(True),
        EnableJetControlPlots=cms.bool(True),
        TriggerNames=cms.vstring("HLTNoTrigger"), #"HLTElectronTTBar","HLTSusySS","HLTElectronZandW","HLTMuon")

        SelectedEventTypesByDSName=cms.PSet( #Event types : 0.5 ee, 1.5 mm, 2.5 em, 3.5 me ; SS 0.25, OS -0.25
            DiLeptonLowPTRunB=cms.vdouble(0.75,1.75,2.75,3.75),
            DiLeptonLowPtRunA=cms.vdouble(0.75,1.75,2.75,3.75),
            #DiLeptonSelectionDSElectron=cms.vdouble(0.75,1.75,2.75,3.75)
        )
    ),
    Analyzers=cms.VPSet(
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(102), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(1+4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationAllCharge"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(1+4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationGSFCTF_D0"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(1+4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationGSF_D0"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(1+4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationMajorityCTF_D0"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(1+4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationMajority_D0"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationGSFCTF"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationGSF"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationMajorityCTF"),
            Type=cms.string("EstimateMisChargeds")
        ),
        cms.PSet(
            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso15.root"),
            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
            Cuts=cms.int32(4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
            Name=cms.string("EstimationMajority"),
            Type=cms.string("EstimateMisChargeds")
        )
#         cms.PSet(
#            Name=cms.string("Printer"),
#            Type=cms.string("Printer")
#        )
        )
    )

#process.ElectronCharge.SelectorConfigs.ElectronSelection.ConvRejMethod = 45
prepare_ds(process.vpsets[INPUT],20,False)
