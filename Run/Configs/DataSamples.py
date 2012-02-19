import FWCore.ParameterSet.Config as cms

ElDSRunAReRec = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/f/fahim/ntp/data/V01-06-04/EG-Run2010A-Sep17ReReco_v2-last_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        XSec=cms.double(-1),
        Name=cms.string("EG_RunA_ReReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

ElDSRunBPRec = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/f/fahim/ntp/data/V01-06-04/Electron-Run2010B-PromptReco-v2_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(26139065),
        XSec=cms.double(-1),
        Name=cms.string("EG_RunB_PromptReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

MuDSRunAReRec = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/f/fahim/ntp/data/V01-06-04/Mu_Run2010A_Sep17ReReco_v2_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(18181793),
        XSec=cms.double(-1),
        Name=cms.string("MUON_RunA_ReReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

MuDSRunBPRec = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/f/fahim/ntp/data/V01-06-04/Mu-Run2010B-PromptReco-v2_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(26591973),
        XSec=cms.double(-1),
        Name=cms.string("MUON_RunB_PromptReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

import os
DiLeptonSelectionDSElectron = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-06-04/DiLeptonSelectionDS/Electron/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("DiLeptonSelectionDSElectron"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DiLeptonSelectionDSMuon = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-06-04/DiLeptonSelectionDS/Muon/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("DiLeptonSelectionDSMuon"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DiLeptonSelectionALL = cms.VPSet(
    DiLeptonSelectionDSMuon[0],
    DiLeptonSelectionDSElectron[0]
    )

DiLeptonLowPTRunA = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-06-04/DiLeptonSelectionDS_LowPt/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("DiLeptonLowPtRunA"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DiLeptonLowPTRunB = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-11-02/DiLeptonSelectionDS_LowPt/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("DiLeptonLowPTRunB"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DiLeptonLowPTALL = cms.VPSet(
    DiLeptonLowPTRunA[0],
    DiLeptonLowPTRunB[0]
    )

SingleElectronSelectionRunA = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-06-04/SingleElectron/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SingleLeptonSelectionRunA"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

SingleElectronSelectionRunB = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-11-02/SingleElectron/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SingleLeptonSelectionRunB"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

SingleElectronSelectionALL = cms.VPSet(
    SingleElectronSelectionRunA[0],
    SingleElectronSelectionRunB[0]
    )

SingleMuonSelectionRunA = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-06-04/SingleMuon/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SingleMuonSelectionRunA"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

SingleMuonSelectionRunB = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/data/V01-11-02/SingleMuon/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SingleMuonSelectionRunB"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

SingleMuonSelectionALL = cms.VPSet(
    SingleMuonSelectionRunA[0],
    SingleMuonSelectionRunB[0]
    )
    