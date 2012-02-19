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
    Name=cms.untracked.string("ZElecSelector"),
    TotalInput=cms.untracked.int32(-1),
    NumberOfEventsPerPrintLoop=cms.untracked.int32(1000),
    Verbosity=cms.untracked.int32(0),
    OutFileName=cms.string("PMM_" + INPUT + ".root"),
    Inputs=cms.vstring(INPUT),
    IntLumi=cms.double(INPUTPSet.TotalNumberOfEvents.value() / INPUTPSet.XSec.value() ),
    
    CopyTree=cms.bool(False),
    CopyTreeFileName=cms.string(__CopyTreeFileName),
    
    HLTs=cms.vstring("HLTNoTrigger"), #"HLTElectronZandW"),
    TriggerHistosEnable=cms.bool(True),

    SelectorConfigs=cms.PSet(
        ElectronSelection=cms.PSet(
            ID=cms.int32(3), #3
            IsoMethod=cms.int32(30),
            IsoCutValue=cms.double(0.1), # 0.1
            IsoRelative=cms.bool(True),

            ConvRejMethod=cms.int32(45), #44

            RejectCrack=cms.bool(True),

            D0Method=cms.int32(55), #change to 53
            D0Value=cms.double(0.02), #0.02

            RejectTrackerDrivensOnly=cms.bool(True),
            PtCut=cms.double(10.0),

            deltaRNearestMuon=cms.untracked.double(0.1),
            allChargesAgree=cms.untracked.bool(False),
            CTFGSFChargesAgree=cms.untracked.bool(False),

            CaloEnergy=cms.untracked.double(10)
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
            DrElectronMinPt=cms.double(10),
            RejectElectronsInGapAfterJetSelection=cms.bool(False),
            DrMuon=cms.double(0.4), #0.4
            DrMuonMinPt=cms.double(10)
        ),

        EventSelection=MakeEventSelection( "SC", 76, 106, False, False, False,25.0,35.0),

        LooseSelection=cms.bool(True),
        EnableEventControlPlots=cms.bool(True),
        EnableElectronControlPlots=cms.bool(True),
        EnableMuonControlPlots=cms.bool(True),
        EnableJetControlPlots=cms.bool(True),
        TriggerNames=cms.vstring("HLTNoTrigger")
    ),
    Analyzers=cms.VPSet(
        cms.PSet(
            Type=cms.string("PMM"),
            Name=cms.string("PMM The First"),
            UseGen = cms.bool(False),

            CTFChargePrintOrder = cms.vstring("CTFConvTightD0"), #"ConvGeo","OnlyConv","ConvTight","ConvHit1","ConvHit0","D0","D0Conv","CTFConvTightD0"),
            EleChargePrintOrder = cms.vstring("CTFConvTightD0"), #"OnlyCTF","ConvGeo","OnlyConv","ConvTight","ConvHit1","ConvHit0","CTFConv","CTFConvTight","D0CTFConv","D0ConvTight","CTFConvTightD0"),
            GSFChargePrintOrder = cms.vstring("OnlyCTF","ConvGeo","OnlyConv","ConvTight","ConvHit1","ConvHit0","CTFConv","CTFConvTight","D0CTFConv","D0ConvTight","CTFConvTightD0"),
            AllChargePrintOrder = cms.vstring("ConvGeo","OnlyConv","ConvTight","ConvHit1","ConvHit0","D0CTFConv","CTFConvTightD0","D0ConvTight"),
            HistogramsName   = cms.string("Events"),
            DirName = cms.string("")
            )
        )
    )

def MakeName (eleCharge):
    ret = eleCharge.Name.value()
    ret = ret + '_' + eleCharge.Inputs[0]
    if len(eleCharge.HLTs) != 0 :
        ret = ret + '_' + eleCharge.HLTs[0]
    ret = ret + '_' + GetStringOfElectronSelection(eleCharge.SelectorConfigs.ElectronSelection)
    ret = ret + '_' + GetStringOfEventSelection(eleCharge.SelectorConfigs.EventSelection)
    os.mkdir(ret)
    return ret

process.ElectronCharge.Analyzers[0].DirName = MakeName( process.ElectronCharge )
process.ElectronCharge.OutFileName = process.ElectronCharge.Analyzers[0].DirName.value() + "/" + process.ElectronCharge.OutFileName.value()

prepare_ds(process.vpsets[INPUT],20,False)
