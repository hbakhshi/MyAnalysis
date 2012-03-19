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
        TotalNumberOfEvents = cms.int32(75102403),
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

DoubleEle2011May10 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleElectron-Run2011A-May10ReReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(9462175),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleEle2011V4 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DEAPRoV4.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(21613377),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v4"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleEle2011Aug = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleElectron-Run2011A-05Aug2011-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(3940010),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("Aug5 ReReco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleEle2011V6 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DEAPRoV6.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(6660401),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v6"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleEle2011B = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DOEB.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(14313686),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

DoubleMuon2011May10 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-May10ReReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(7935262),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleMuon2011V4 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-PromptReco-v4-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(15712151),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v4"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleMuon2011Aug5 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-05Aug2011-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(3421060),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("Aug5 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleMuon2011V6 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011A-PromptReco-v6-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(6153779),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v6"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
DoubleMuon2011B = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/DoubleMu-Run2011B-PromptReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(21550089),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

ElectronMuon2011May10 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-May10ReReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(5416001),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
ElectronMuon2011V4 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-PromptReco-v4-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(13543944),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v4"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
ElectronMuon2011Aug5 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-05Aug2011-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(3501760),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("Aug5 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
ElectronMuon2011V6 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011A-PromptReco-v6-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(6631888),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011A v6"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
ElectronMuon2011B = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/data/V02-05-01/MuEG-Run2011B-PromptReco-v1-AOD/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(20237878),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

