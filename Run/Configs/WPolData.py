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

WJetsSummer2011_1a = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_1a"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_1[01].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_1b = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_1b"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_1[23].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_1c = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_1c"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_1[45].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_1d = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_1d"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_1[67].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_1e = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_1e"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_1[89].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_23 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_23"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_[23].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_45 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_45"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_[45].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_67 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_67"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_[67].*_.*\.root"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
WJetsSummer2011_89 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/MC/V02-05-01/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("WJetsSummer2011_89"),
        fileNamePattern=cms.untracked.string(".*NTupleProducer_42X_MC_[89].*_.*\.root"),
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

SingleTopTWSummer2011 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/SingleToptw_channel.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SingleTopTWSummer2011"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0),
        #Note=cms.untracked.string("Just TOP, XSec=7.9, Anti-Top should be added separately")
        )
    )

WZSummer2011 = cms.VPSet(
        cms.PSet(
            File=cms.string(os.environ['ntpHome'] + '/Samples/WZ.root'),
                    IsDirectory = cms.untracked.bool(False),
                    TotalNumberOfEvents = cms.int32(-1),
                    IsCastor   = cms.bool(False),
                    XSec=cms.double(-1),
                    Name=cms.string("WZSummer2011"),
                    MaxInput=cms.int32(-1),
                    PreSelEfficiency=cms.double(1.0),
                    #Note=cms.untracked.string("Just TOP, XSec=7.9, Anti-Top should be added separately")
                    )
            )

ZZSummer2011 = cms.VPSet(
        cms.PSet(
            File=cms.string(os.environ['ntpHome'] + '/Samples/ZZ.root'),
                    IsDirectory = cms.untracked.bool(False),
                    TotalNumberOfEvents = cms.int32(-1),
                    IsCastor   = cms.bool(False),
                    XSec=cms.double(-1),
                    Name=cms.string("ZZSummer2011"),
                    MaxInput=cms.int32(-1),
                    PreSelEfficiency=cms.double(1.0),
                    #Note=cms.untracked.string("Just TOP, XSec=7.9, Anti-Top should be added separately")
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
        Name=cms.string("EE May10 Rereco"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0),
        fileNamePattern=cms.untracked.string(".*\.root")
        )
    )
DoubleEle2011V4 = cms.VPSet(
    cms.PSet(
        File=cms.string(os.environ['ntpHome'] + '/Samples/DEAPRoV4.root'),
        IsDirectory = cms.untracked.bool(False),
        TotalNumberOfEvents = cms.int32(21613377),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("EE PromptReco Run2011A v4"),
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
        Name=cms.string("EE PromptReco Run2011A v6"),
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
        Name=cms.string("EE PromptReco Run2011B v1"),
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
        Name=cms.string("MM May10 Rereco"),
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
        Name=cms.string("MM PromptReco Run2011A v4"),
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
        Name=cms.string("MM Aug5 Rereco"),
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
        Name=cms.string("MM PromptReco Run2011A v6"),
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
        Name=cms.string("MM PromptReco Run2011B v1"),
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
        Name=cms.string("EM May10 Rereco"),
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
        Name=cms.string("EM PromptReco Run2011A v4"),
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
        Name=cms.string("EM Aug5 Rereco"),
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
        Name=cms.string("EM PromptReco Run2011A v6"),
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
        Name=cms.string("EM PromptReco Run2011B v1"),
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )



SysTTM175 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/TTJets_TuneZ2_mass175_5_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysTTM175")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
SysTTM169 = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/TTJets_TuneZ2_mass169_5_7TeV-madgraph-tauola'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysTTM169")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
SysTTQD = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/TTjets_TuneZ2_scaledown_7TeV-madgraph-tauola'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysTTQD")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
SysTTQU = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/TTjets_TuneZ2_scaleup_7TeV-madgraph-tauola'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysTTQU")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )

SysWJetsQD = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/WJetsToLNu_TuneZ2_scaledown_7TeV-madgraph-tauola'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysWJetsQD")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
SysWJetsQU = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/WJetsToLNu_TuneZ2_scaleup_7TeV-madgraph-tauola'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysWJetsQU")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
SysZJetsQD = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/ZJetsToLL_TuneZ2_scaledown_7TeV-madgraph-tauola'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysZJetsQD")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
SysZJetsQU = cms.VPSet(
    cms.PSet(
        File=cms.string('/public/wpolar/SYS/V02-05-01/ZJetsToLL_TuneZ2_scaleup_7TeV-madgraph-tauola/'),
        IsDirectory = cms.untracked.bool(True),
        TotalNumberOfEvents = cms.int32(-1),
        IsCastor   = cms.bool(False),
        XSec=cms.double(-1),
        Name=cms.string("SysZJetsQU")
        MaxInput=cms.int32(-1),
        PreSelEfficiency=cms.double(1.0)
        )
    )
