import FWCore.ParameterSet.Config as cms

Z = cms.VPSet(
   cms.PSet(
    #			File = cms.string("/castor/cern.ch/user/h/hbakhshi/ETH/Zee-Py6-Spring10-v2.root"),
    #			XSec = cms.double(1300),
    #			Name = cms.string("ZEE"),
    #			MaxInput = cms.int32(1300),
    #			PreSelEfficiency = cms.double(1.0)
    #		),
    File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/mc/V01-11-02/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/"),
    IsDCap=cms.untracked.bool(True),
    DCapPort=cms.int32(22125),
    SRMPort=cms.int32(8443),
    Server=cms.string("t3se01.psi.ch"),
    LCG_LS=cms.string("/opt/lcg/bin/lcg-ls"),
    TotalNumberOfEvents=cms.int32(2552567),
    XSec=cms.double(3048),
    Name=cms.string("DYJetsLL"),
    MaxInput=cms.int32(-1),
    PreSelEfficiency=cms.double(1.0)
    )
)

ZPreselected = cms.VPSet(
cms.PSet(
    File=cms.string("/afs/cern.ch/user/h/hbakhshi/scratch0/CMSSW_3_8_2/src/PMM/ReproduceNumbers/TREE_ZMC.root"),
    XSec=cms.double(3048),
    Name=cms.string("ZJets preselected"),
    MaxInput=cms.int32(-1),
    PreSelEfficiency=cms.double(108268.0 / 1074073.0)
    )
)

HLTElectronZMC = cms.VPSet(
cms.PSet(
    MinRun=cms.int32(0),
    MaxRun=cms.int32(10000),
    HLTNames=cms.vstring("HLT_Ele10_LW_EleId_L1R"), #They shall OR together
    Name=cms.string("All")
    ),
)

QCD = cms.VPSet(
# cms.PSet(
# File=cms.string("/castor/cern.ch/user/h/hbakhshi/ETH/QCD_Pt50To100-madgraph-v3.root"),
# XSec=cms.double(3000000),
# Name=cms.string("QCD50TO100"),
# MaxInput=cms.int32(-1),
# PreSelEfficiency=cms.double(1.0)
# ),
cms.PSet(
    File=cms.string("/castor/cern.ch/user/f/fahim/tmp/QCD_Pt100to250-madgraph-V01-03-01.root"),
    XSec=cms.double(7000000),
    Name=cms.string("QCD100TO250"),
    MaxInput=cms.int32(-1),
    PreSelEfficiency=cms.double(1.0)
    )
)

W = cms.VPSet(
cms.PSet(
    File=cms.string("/pnfs/psi.ch/cms/trivcat/store/user/susy/ntuples/mc/V01-11-02/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/"),
    IsDCap=cms.untracked.bool(True),
    DCapPort=cms.int32(22125),
    SRMPort=cms.int32(8443),
    Server=cms.string("t3se01.psi.ch"),
    LCG_LS=cms.string("/opt/lcg/bin/lcg-ls"),
    TotalNumberOfEvents=cms.int32(14350756),
    XSec=cms.double(31314),
    Name=cms.string("WLNu"),
    MaxInput=cms.int32(-1),
    PreSelEfficiency=cms.double(1.0)
    )
)

VV = cms.VPSet(
cms.PSet(
    File=cms.string("/castor/cern.ch/user/f/fahim/tmp/VVJets-madgraph-V01-03-01.root"),
    XSec=cms.double(4.8),
    Name=cms.string("VVJets"),
    MaxInput=cms.int32(-1),
    PreSelEfficiency=cms.double(1.0)
    )
)
