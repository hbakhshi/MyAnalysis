import FWCore.ParameterSet.Config as cms

SampleElectronSelection = cms.PSet(
    ID=cms.int32(0), # idWP95 = 0,idWP90 = 1,idWP85 = 2,idWP80 = 3,idWP70 = 4,idWP60 = 5,idRobustTight = 6,idRobustLoose = 7,idTight = 8,idLoose = 9
    
    IsoMethod=cms.int32(29), # isoComb03 = 21,isoComb04 =22,isoTrk03 =23,isoTrk04 =24,isoECal03 = 25,isoECal04 = 26,isoHCal03 = 27,isoHCal04 = 28,isoAllIndividuals = 29
    IsoCutValue=cms.double(-1), #negative values mean WP cut value should be used
    IsoRelative=cms.bool(True),

    ConvRejMethod=cms.int32(45), #convGeoCutsOnly = 41,convInnerHitsOnly = 42,convAllCuts = 43,convUseWPInfo = 44,convNone = 45
    
    D0Method=cms.int32(55), #D0PV = 51,D0PVSign = 52,D0BS = 53,D0BSSign = 54,D0None = 55
    D0Value=cms.double(0.),
    
    RejectTrackerDrivensOnly=cms.bool(True),
    PtCut=cms.double(15)
    )

def MakeWPElectronSelection (name, combinedIso, ptCut):
    Id = 0
    
    if name == 95:
        Id = 0
    elif name == 90:
        Id = 1
    elif name == 85:
        Id = 2
    elif name == 80:
        Id = 3
    elif name == 70:
        Id = 4
    elif name == 60:
        Id = 5
    
    iso = 0
    if combinedIso :
        iso = 21
    else :
        iso = 29

    ret = cms.PSet(
        ID = cms.int32(Id),

        IsoMethod=cms.int32(iso),
        IsoCutValue=cms.double(-1),
        IsoRelative=cms.bool(True),

        ConvRejMethod=cms.int32(44),

        D0Method=cms.int32(55),
        D0Value=cms.double(0.),

        RejectTrackerDrivensOnly=cms.bool(True),
        PtCut=cms.double(ptCut)
        )

    return ret
   
def GetStringOfElectronSelection (eleSel) :
    ret = 'id:' + str(eleSel.ID.value())
    ret += ',iso:' + str(eleSel.IsoMethod.value())
    ret += ',con:' + str(eleSel.ConvRejMethod.value())
    ret += ',d0:' + str(eleSel.D0Method.value())
    ret += ',pt>' + str(eleSel.PtCut.value())
    return ret
    
