ElRunAPR1 = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/h/hbakhshi/ntp/data/V01-04-01/EG-Run2010A-PromptReco-v4_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(42353616),
        XSec=cms.double(42353616 / 2.999486663),
        Name=cms.string("RealDataElSD"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

ElDSRunAReRec2 = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/h/hbakhshi/ntp/data/V01-05-00/EG-Run2010A-Sep17ReReco_v2_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(42276270),
        XSec=cms.double(42276270 / 2.948364833 ),
        Name=cms.string("EG_RunA_ReReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
)

ElDSRunBPRec1 = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/h/hbakhshi/ntp/data/V01-05-00/Electron-Run2010B-PromptReco-v2_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(12942384),
        XSec=cms.double(12942384 / 7.702531022),
        Name=cms.string("EG_RunB_PromptReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

ElDSRunBPRec2 = cms.VPSet(
    cms.PSet(
        File=cms.string("/castor/cern.ch/user/h/hbakhshi/ntp/data/V01-06-00/Electron-Run2010B-PromptReco-v2_RECO/"),
        IsDirectory = cms.untracked.bool(True),
        IsCastor   = cms.bool(True),
        TotalNumberOfEvents = cms.int32(20332669+70257),
        XSec=cms.double(-1),
        Name=cms.string("EG_RunB_PromptReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

ElSDSubset = cms.VPSet(
    cms.PSet(
        File=cms.string("/afs/cern.ch/user/h/hbakhshi/scratch0/PMM/RunOnData/Results/EventSelection/results/"),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("RealDataElSD"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
