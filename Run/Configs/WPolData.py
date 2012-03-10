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

WJetsSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DYSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01//DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("DYSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

SingleTopSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/SingleTop_tchannel/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SingleTopSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

WWSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/WW.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WWSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DoubleEle2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DEAMay.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DEAPRoV4.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v4"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DEAPRoV6.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v6"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DOEB.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    )

DoubleMuon2011 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-May10ReReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(T),
        XSec=cms.double(-1),
        Name=cms.string("May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-PromptReco-v4-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v4"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-05Aug2011-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("Aug5 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-PromptReco-v6-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v6"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011B-PromptReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    )

ElectronMuon2011 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-May10ReReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(T),
        XSec=cms.double(-1),
        Name=cms.string("May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-PromptReco-v4-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v4"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-05Aug2011-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("Aug5 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-PromptReco-v6-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v6"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011B-PromptReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        ),
    )

