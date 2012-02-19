import FWCore.ParameterSet.Config as cms

SampleEventSelection=cms.PSet(
    InvMassMethod=cms.int32(3), #_Momentum = 0,_GSFTrack = 1,_GSFTrack_OUT = 2,_SuperCluster = 3,_Seed = 4,_NearestTrack = 5,_BestMatch1 = 6,_BestMatch3 = 7
    InvMassMin=cms.double(10),
    InvMassMax=cms.double(3000),
    
    ElectronsShouldMatch=cms.bool(False),
    JetVeto=cms.bool(False),
    VetoMoreThan2Elecs=cms.bool(False)
    ),


def MakeEventSelection( invMassMethod, _from, _to, GenMatch, _JetVeto, _VetoMoreThan2, _MET, _MT) :
    method = -1
    if invMassMethod == "Momentum" :
        method = 0
    elif invMassMethod == "GSF" :
        method = 1
    elif invMassMethod == "GSFOut" :
        method = 2
    elif invMassMethod == "SC" :
        method = 3
    elif invMassMethod == "Seed" :
        method = 4
    elif invMassMethod == "CTF" :
        method = 5

    ret = cms.PSet(
        InvMassMethod=cms.int32(method),
        InvMassMin=cms.double(_from),
        InvMassMax=cms.double(_to),
        MET=cms.double(_MET),
        MT=cms.double(_MT),
        
        ElectronsShouldMatch=cms.bool(GenMatch),
        JetVeto=cms.bool(_JetVeto),
        VetoMoreThan2Elecs=cms.bool(_VetoMoreThan2)
        )

    return ret

def GetStringOfEventSelection (eveSel) :
    ret = ''
    invMassMethod = ''
    if eveSel.InvMassMethod.value() == 0 :
        invMassMethod = "Mom"     
    elif eveSel.InvMassMethod.value() == 1 :
        invMassMethod = "GSF"
    elif eveSel.InvMassMethod.value() == 2 :
        invMassMethod = "GSFOut"
    elif eveSel.InvMassMethod.value() == 3 :
        invMassMethod = "SC"
    elif eveSel.InvMassMethod.value() == 4:
        invMassMethod = "Seed"
    elif eveSel.InvMassMethod.value() == 5:
        invMassMethod = "CTF"

    ret = str(eveSel.InvMassMin.value()) + "<" + invMassMethod + "<" + str(eveSel.InvMassMax.value())

    met__ = '_met<' + str(eveSel.MET.value())
    mt__ = '_mt<' + str(eveSel.MT.value())
    
    
    jetV = ''
    if eveSel.JetVeto.value() :
        jetV = 'JV'
        
    genMatch = ''
    if eveSel.ElectronsShouldMatch.value() :
        genMatch = 'Gen'

    thirdVeto = ''
    if eveSel.VetoMoreThan2Elecs.value() :
        thirdVeto = 'V3Ele'

    ret += "," + jetV + genMatch + thirdVeto + met__ + mt__
    
    return ret
