import FWCore.ParameterSet.Config as cms

ElDSRunAReRec_DCap = cms.VPSet(
    cms.PSet(
        File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-06-04/EG-Run2010A-Sep17ReReco_v2_RECO/"),
        IsDCap = cms.untracked.bool(True),
        DCapPort = cms.int32(22125),
        SRMPort = cms.int32(8443),
        Server = cms.string("t3se01.psi.ch"),
        LCG_LS = cms.string("/opt/lcg/bin/lcg-ls"),
        TotalNumberOfEvents = cms.int32(39186392),
        XSec=cms.double(-1),
        Name=cms.string("EG_RunA_ReReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

ElDSRunBPRec_DCap = cms.VPSet(
    cms.PSet(
        File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-06-04/Electron-Run2010B-PromptReco-v2_RECO/"),
        IsDCap = cms.untracked.bool(True),
        DCapPort = cms.int32(22125),
        SRMPort = cms.int32(8443),
        Server = cms.string("t3se01.psi.ch"),
        LCG_LS = cms.string("/opt/lcg/bin/lcg-ls"),
        TotalNumberOfEvents = cms.int32(26148066),
        XSec=cms.double(-1),
        Name=cms.string("EG_RunB_PromptReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

MuDSRunAReRec_DCap = cms.VPSet(
    cms.PSet(
        File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-06-04/Mu_Run2010A_Sep17ReReco_v2_RECO/"),
        IsDCap = cms.untracked.bool(True),
        DCapPort = cms.int32(22125),
        SRMPort = cms.int32(8443),
        Server = cms.string("t3se01.psi.ch"),
        LCG_LS = cms.string("/opt/lcg/bin/lcg-ls"),
        TotalNumberOfEvents = cms.int32(18510858),
        XSec=cms.double(-1),
        Name=cms.string("MUON_RunA_ReReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

MuDSRunBPRec_DCap = cms.VPSet(
    cms.PSet(
        File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-06-04/Mu-Run2010B-PromptReco-v2_RECO/"),
        IsDCap = cms.untracked.bool(True),
        DCapPort = cms.int32(22125),
        SRMPort = cms.int32(8443),
        Server = cms.string("t3se01.psi.ch"),
        LCG_LS = cms.string("/opt/lcg/bin/lcg-ls"),
        TotalNumberOfEvents = cms.int32(26488919),
        XSec=cms.double(-1),
        Name=cms.string("MUON_RunB_PromptReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

JetDSRunBPRec_DCap = cms.VPSet(
    cms.PSet(
        File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-11-02/Jet-Run2010B-Nov4ReReco_v1_RECO/"),
        IsDCap = cms.untracked.bool(True),
        DCapPort = cms.int32(22125),
        SRMPort = cms.int32(8443),
        Server = cms.string("t3se01.psi.ch"),
        LCG_LS = cms.string("/opt/lcg/bin/lcg-ls"),
        TotalNumberOfEvents = cms.int32(11742837),
        XSec=cms.double(-1),
        Name=cms.string("JetDSRunBPRec_DCap"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

JetMETDSRunAReRec_DCap = cms.VPSet(
    cms.PSet(
        File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/data/V01-06-04/JetMETTau-Run2010A-Sep17ReReco_v2_RECO/"),
        IsDCap = cms.untracked.bool(True),
        DCapPort = cms.int32(22125),
        SRMPort = cms.int32(8443),
        Server = cms.string("t3se01.psi.ch"),
        LCG_LS = cms.string("/opt/lcg/bin/lcg-ls"),
        TotalNumberOfEvents = cms.int32(21393943),
        XSec=cms.double(-1),
        Name=cms.string("JetMETDSRunAReRec_DCap"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
