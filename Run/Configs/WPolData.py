import FWCore.ParameterSet.Config as cms
import os

TTBarMadSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + "/TTMad_status3.root"),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("TTBarMadSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

TTBarSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/TTJet_status3.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("TTBarSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
