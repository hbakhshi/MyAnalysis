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
    Name=cms.untracked.string("FRCalculator"),
    TotalInput=cms.untracked.int32(-1),
    NumberOfEventsPerPrintLoop=cms.untracked.int32(10000),
    Verbosity=cms.untracked.int32(0),
    OutFileName=cms.string("FR_" + INPUT + ".root"),
    Inputs=cms.vstring(INPUT),
    IntLumi=cms.double(INPUTPSet.TotalNumberOfEvents.value() / INPUTPSet.XSec.value() ),
    
    CopyTree=cms.bool(False),
    CopyTreeFileName=cms.string(__CopyTreeFileName),
    
    HLTs=cms.vstring("HLTNoTrigger"), #"HLTElectronZandW"),
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        ElectronSelection=cms.PSet(
            ID=cms.int32(-1), #95
            IsoMethod=cms.int32(-1),
            IsoCutValue=cms.double(0.1), # 0.1
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(42), #44

            RejectCrack=cms.bool(True),

            D0Method=cms.int32(53), #change to 53
            D0Value=cms.double(0.02), #0.02

            RejectTrackerDrivensOnly=cms.bool(True),
            PtCut=cms.double(10.0),

            deltaRNearestMuon=cms.untracked.double(0.1),
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
            allChargesAgree=cms.untracked.bool(False),
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(0)
        ),
        RejectCategoriesWithJustOneLepton = cms.bool(True),
#        IsElectron = cms.untracked.bool(True),
        ElectronCategories = cms.VPSet(
            cms.PSet(
                Name = cms.string("ID"),
                WP = cms.int32(95),
                AcceptedValues = cms.vint32(1, 3, 5, 7),
                ISOMethod = cms.int32(-1)
            ),
            cms.PSet(
                Name = cms.string("ISO"),
                WP = cms.int32(95),
                AcceptedValues = cms.vint32(0,1,2, 3,4,5, 6, 7),
                ISOMethod = cms.int32(30),
                ISOValEC = cms.double(0.6),
                ISOValBRL = cms.double(1.0)
            ),
            cms.PSet(
                Name = cms.string("IDISO"),
                WP = cms.int32(95),
                AcceptedValues = cms.vint32(1, 3, 5, 7),
                ISOMethod = cms.int32(30),
                ISOValEC = cms.double(0.6),
                ISOValBRL = cms.double(1.0)
            ),
            cms.PSet(
                Name = cms.string("GoodEleNonIso"),
                WP = cms.int32(80),
                AcceptedValues = cms.vint32(5,7),
                ISOMethod = cms.int32(30),
                ISOValEC = cms.double(0.6),
                ISOValBRL = cms.double(1.0)
            ),
            cms.PSet(
                Name = cms.string("GoodEle"),
                WP = cms.int32(80),
                AcceptedValues = cms.vint32(5,7),
                ISOMethod = cms.int32(30),
                ISOValEC = cms.double(0.1),
                ISOValBRL = cms.double(0.1)
            )
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
            RejectCrack=cms.bool(True),
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
                allChargesAgree=cms.untracked.bool(False),
                CTFGSFChargesAgree=cms.untracked.bool(False),

                CaloEnergy=cms.untracked.double(7.0)
            ),
            RejectElectronsInGapAfterJetSelection=cms.bool(False),
            DrMuon=cms.double(0.4), #0.4
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
        ),

        LooseSelection=cms.bool(True),
        EnableEventControlPlots=cms.bool(True),
        EnableElectronControlPlots=cms.bool(True),
        EnableMuonControlPlots=cms.bool(True),
        EnableJetControlPlots=cms.bool(True),
        TriggerNames=cms.vstring("HLTNoTrigger")
    ),
    Analyzers=cms.VPSet(
        cms.PSet(
            Type=cms.string("FRCalc"),
            Name=cms.string("FRCalc1"),
            AllFR=cms.VPSet(
                cms.PSet(
                    Name=cms.string("Full80_80NonIso"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)"),
                    FakableObjectTag = cms.string("GoodEleNonIso"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_ID95_Barrel"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)&EtaRegion(1,1)"),
                    FakableObjectTag = cms.string("ID"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_ID95_EndCap"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)&EtaRegion(2,2)"),
                    FakableObjectTag = cms.string("ID"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_IsoLoose_Barrel"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)&EtaRegion(1,1)"),
                    FakableObjectTag = cms.string("ISO"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_IsoLoose_EndCap"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)&EtaRegion(2,2)"),
                    FakableObjectTag = cms.string("ISO"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_Loose95_Barrel"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)&EtaRegion(1,1)"),
                    FakableObjectTag = cms.string("IDISO"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_Loose95_EndCap"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)&EtaRegion(2,2)"),
                    FakableObjectTag = cms.string("IDISO"),
                    ElectronTag = cms.string("GoodEle")
                ),
                cms.PSet(
                    Name=cms.string("Full80_Loose95"),
                    FOParams=cms.string(" "),
                    EleParams=cms.string(" "),
                    Type=cms.int32(0),
                    AlgParams=cms.string("ObjectType(0,0)"),
                    FakableObjectTag = cms.string("IDISO"),
                    ElectronTag = cms.string("GoodEle")
                )
            )
          )
        )
    )

prepare_ds(process.vpsets[INPUT],20,False)
