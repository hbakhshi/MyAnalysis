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

COPYTREEFILENAME = ''
if os.environ.has_key( 'COPYTREEFILENAME' ):
    COPYTREEFILENAME = os.environ['COPYTREEFILENAME']
else :
    COPYTREEFILENAME = "CLONETREE.root"

CPUN = 1
if os.environ.has_key( 'CPUNumber' ):
    CPUN = int(os.environ['CPUNumber'])

process.INPUT = cms.VPSet(
    cms.PSet(
        File=cms.string("/tmp/hbakhshi/" + COPYTREEFILENAME),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PreselctedFiles"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

process.RunSetup = cms.untracked.PSet(
    CPU_Number = cms.untracked.int32(CPUN)
    )

process.ElectronCharge = cms.PSet(
    Name=cms.untracked.string("FakeEstimator"),
    TotalInput=cms.untracked.int32(-1),
    NumberOfEventsPerPrintLoop=cms.untracked.int32(100),
    Verbosity=cms.untracked.int32(0),
    OutFileName=cms.string("FakeEstimate_" + process.INPUT[0].Name.value() + ".root"),
    Inputs=cms.vstring('INPUT'),
    IntLumi=cms.double(35.0),

    CopyTree=cms.bool(False),
    CopyTreeFileName=cms.string(''),

    HLTs=cms.vstring("HLTNoTrigger"), #"HLTElectronZandW"),
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        ElectronSelection=cms.PSet(
            ID=cms.int32(-1), #95
            IsoMethod=cms.int32(-1),
            IsoCutValue=cms.double(0.1), # 0.1
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(45), #44

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
            allChargesAgree=cms.untracked.bool(True),
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(10.0)
        ),
        RejectCategoriesWithJustOneLepton = cms.bool(False),
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
        MuonCategories = cms.VPSet(
            cms.PSet(
                Name = cms.string("Loose"),
                ISOCone = cms.double(1.0), #1.0 means hybrid with cone 0.3
                ISOCut = cms.double(1.0)
            ),
            cms.PSet(
                Name = cms.string("Tight0.1"),
                ISOCone = cms.double(1.0), #1.0 means hybrid with cone 0.3
                ISOCut = cms.double(0.1)
            ),
            cms.PSet(
                Name = cms.string("Tight0.15"),
                ISOCone = cms.double(1.0), #1.0 means hybrid with cone 0.3
                ISOCut = cms.double(0.15)
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
            IsoCut=cms.double(1.0), # 0.1
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
                allChargesAgree=cms.untracked.bool(True),
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
            Type=cms.string("SameFlavorFakeEstimator"),
            Name = cms.string("TEST"),
            isElectron = cms.bool(True),
            TightTag = cms.string("GoodEle"),
            LooseTag = cms.string("IDISO"),

            FakeRateFileName = cms.string("../FakeRateCalcElec/FR_SingleElectronSelectionALL.root"),
            FakeRateDirectoryName = cms.string("FakeRates_Full80_Loose95"),
            binnedPt = cms.bool(True),
            AbsEta = cms.bool(True),
            EndcapBarrel = cms.bool(True)
          )
        )
    )

#prepare_ds(process.vpsets[INPUT],20,False)
