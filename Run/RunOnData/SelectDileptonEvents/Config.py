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

CPUN = 1
if os.environ.has_key( 'CPUNumber' ):
    CPUN = int(os.environ['CPUNumber'])

__CopyTreeFileName =  STARTUPDIR + "/" + COPYTREEFILENAME

process.INPUT = cms.VPSet(
    cms.PSet(
        File=cms.string("./TreeSS_DiLeptonSelectionDSElectron.root"),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PreselctedFiles"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

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
    Name=cms.untracked.string("SusySelector"),
    TotalInput=cms.untracked.int32(-1),
    NumberOfEventsPerPrintLoop=cms.untracked.int32(100),
    Verbosity=cms.untracked.int32(0),
    OutFileName=cms.string("SUSY_" + INPUT + ".root"),
    Inputs=cms.vstring(INPUT),
    IntLumi=cms.double(INPUTPSet.TotalNumberOfEvents.value() / INPUTPSet.XSec.value() ),
    
    CopyTree=cms.bool(False), #True
    CopyTreeFileName=cms.string(__CopyTreeFileName),
    
    HLTs=cms.vstring("HLTNoTrigger" ), #"HLTElectronTTBar","HLTSusySS","HLTElectronZandW","HLTMuon"),
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        EventsRunID=cms.VPSet(
#            cms.PSet(
#                Run=cms.int32(147754),
#                Events=cms.vint32(272129813),
#                Note=cms.string("Fake Study ???")
#            ),
#            cms.PSet(
#                Run=cms.int32(149291),
#                Events=cms.vint32(572246179),
#                Note=cms.string("Fake Study ???")
#            )
#           ,cms.PSet(
#                Run=cms.int32(148822),
#                Events=cms.vint32(156279004),
#                Note=cms.string("Low Pt selection, Electron Data Set")
#            ),
#            cms.PSet(
#                Run=cms.int32(144089),
#                Events=cms.vint32(1408415137),
#                Note=cms.string("Common between me and benjamin -- lumisection  1330 in dataset ElectronA / JetMETA")
#            ),
#            cms.PSet(
#                Run=cms.int32(146944),
#                Events=cms.vint32(609465692),
#                Note=cms.string("Just me")
#            ),
#            cms.PSet(
#                Run=cms.int32(149181),
#                Events=cms.vint32(743699443),
#                Note=cms.string("Just Benjamin -- lumisection   772 in dataset     MuonB / MultiJet")
#            ),
#            cms.PSet(
#                Run=cms.int32(148864),
#                Events=cms.vint32(594577419),
#                Note=cms.string("Just Benjamin -- lumisection   522 in dataset     MuonB / MultiJet")
#            )
        ),
        ElectronSelection=cms.PSet(
            ID=cms.int32(3), #3
            IsoMethod=cms.int32(30),
            IsoCutValue=cms.double(0.1), # 0.1
#            EleIsoBarrel=cms.untracked.double(1.0), # 1.0 for loose selection
#            EleIsoEndCap=cms.untracked.double(0.6), # 0.6 for loose selection
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(44), #44

            RejectCrack=cms.bool(False),

            D0Method=cms.int32(53), #change to 53
            D0Value=cms.double(0.02), #0.02

            RejectTrackerDrivensOnly=cms.bool(True),
            PtCut=cms.double(10.0),

            deltaRNearestMuon=cms.untracked.double(0.1), #0.1
            allChargesAgree=cms.untracked.bool(True), #True
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(10.0) #10.0
        ),
        MuonSelection=cms.PSet(
            GlobalMuon=cms.bool(True),
            TrackerMuon=cms.bool(True),
            PtCut=cms.double(10.0),
            Eta=cms.double(2.4),
            NChi2=cms.double(10.0),
            EEm=cms.double(4),
            EHad=cms.double(6),
            IsoCut=cms.double(0.1), # 0.1
            d0Cut=cms.double(0.02),
            NTrkHits=cms.int32(10),
            NMuHits=cms.int32(0)
        ),
        JetSelection=cms.PSet(
            checkID=cms.bool(True),
            PtCut=cms.double(30), #30
            Eta=cms.double(2.5),
            RejectCrack=cms.bool(False),
            DrElectron=cms.double(0.4), #0.4
            ElectronSelection=cms.PSet(
                ID=cms.int32(3), #3
                IsoMethod=cms.int32(30),
                IsoCutValue=cms.double(0.1), # 0.1
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
            RejectElectronsInGapAfterJetSelection=cms.bool(False),
            DrMuon=cms.double(0.4), #0.4
        ),
        RejectInvMassLessThan12=cms.bool(True), #True
        RejectZRegion=cms.bool(True), #True #This rejects the event if after all cuts, the two leptons inv-mass lies between 76 and 106
        RejectLeptonsFromZ=cms.bool(True), #True #This removes all of the leptons from the same flavour that their inv-mass lies between 76 and 106
        METCutOF=cms.double(20.0), #20
        METCutSF=cms.double(30.0), #30
        HTCut=cms.double(60),
        NJets=cms.int32(2), #2
        HighLeptonPt=cms.double(20.0), #20.0

        LooseSelection=cms.bool(False),
        EnableEventControlPlots=cms.bool(True),
        EnableElectronControlPlots=cms.bool(True),
        EnableMuonControlPlots=cms.bool(True),
        EnableJetControlPlots=cms.bool(True),
        TriggerNames=cms.vstring("HLTNoTrigger"), #"HLTElectronTTBar","HLTSusySS","HLTElectronZandW","HLTMuon")

        SelectedEventTypesByDSName=cms.PSet( #Event types : 0.5 ee, 1.5 mm, 2.5 em, 3.5 me ; SS 0.25, OS -0.25
            DiLeptonSelectionDSElectron=cms.vdouble(0.75),
            DiLeptonSelectionDSMuon=cms.vdouble(0.25,0.75,2.25,2.75,3.25,3.75,1.25)
        )
    ),
    Analyzers=cms.VPSet(
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(102), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(1+4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationAllCharge"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(1+4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationGSFCTF_D0"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(1+4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationGSF_D0"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(1+4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationMajorityCTF_D0"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(1+4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationMajority_D0"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationGSFCTF"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(100), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationGSF"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(4+8), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationMajorityCTF"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
#        cms.PSet(
#            PMMFileName=cms.string("../SelectZEvents/PMM_WP80_Iso10.root"),
#            ChargeSource=cms.int32(101), #GSFCharge = 100,MajorityCharge = 101,AllChargesMatch = 102
#            Cuts=cms.int32(4), #NoCUT = 0,D0 = 1,ConvRej1 = 2,ConvRej0 = 4,CTFMatch = 8
#            Name=cms.string("EstimationMajority"),
#            Type=cms.string("EstimateMisChargeds")
#        ),
        cms.PSet(
            Name=cms.string("Printer"),
            Type=cms.string("Printer")
        )
        )
    )

#process.ElectronCharge.SelectorConfigs.ElectronSelection.ConvRejMethod = 45
prepare_ds(process.vpsets[INPUT],20,False)
