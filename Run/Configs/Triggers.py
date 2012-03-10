import FWCore.ParameterSet.Config as cms

HLTDiElectronMCTTBar2011 = cms.VPSet (
    cms.PSet(
        MinRun=cms.int32(0),
        MaxRun=cms.int32(2),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_Ele17_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_Ele8_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_v." , "HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v."),
        Name=cms.string("AllMC"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    )
)

HLTDiMuonMCTTBar2011 = cms.VPSet (
    cms.PSet(
        MinRun=cms.int32(0),
        MaxRun=cms.int32(2),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_DoubleMu3_v.", "HLT_DoubleMu6_v." , "HLT_DoubleMu7_v."),
        Name=cms.string("AllMC"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    )
)

HLTMuElMCTTBar2011 = cms.VPSet (
    cms.PSet(
        MinRun=cms.int32(0),
        MaxRun=cms.int32(2),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_Mu10_Ele10_CaloIdL_v." , "HLT_Mu17_Ele8_CaloIdL_v." , "HLT_Mu8_Ele17_CaloIdL_v."),
        Name=cms.string("AllMC"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    )
)

HLTEETTBar2011Data = cms.VPSet (
    cms.PSet(
        MinRun=cms.int32(160329),
        MaxRun=cms.int32(175770),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_Ele17_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_Ele8_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_v.","HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v.","HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v."),
        Name=cms.string("RunA2011"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    ),
    cms.PSet(
        MinRun=cms.int32(175832),
        MaxRun=cms.int32(180296),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_Ele17_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_Ele8_CaloIdT_TrkIdVL_CaloIsoVL_TrkIsoVL_v.","HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v.","HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v."),
        Name=cms.string("RunB2011"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    )
)

HLTEMuTTBar2011Data = cms.VPSet (
    cms.PSet(
        MinRun=cms.int32(160329),
        MaxRun=cms.int32(175770),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_Mu10_Ele10_CaloIdL_v.","HLT_Mu17_Ele8_CaloIdL_v.","HLT_Mu8_Ele17_CaloIdL_v.","HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v.","HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v."),
        Name=cms.string("RunA2011"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    ),
    cms.PSet(
        MinRun=cms.int32(175832),
        MaxRun=cms.int32(180296),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_Mu10_Ele10_CaloIdL_v.","HLT_Mu17_Ele8_CaloIdL_v.","HLT_Mu8_Ele17_CaloIdL_v.","HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v.","HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v."),
        Name=cms.string("RunB2011"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    )
)

HLTMuMuTTBar2011Data = cms.VPSet (
    cms.PSet(
        MinRun=cms.int32(160329),
        MaxRun=cms.int32(175770),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_DoubleMu7_v.","HLT_Mu13_Mu8_v.","HLT_Mu17_Mu8_v."),
        Name=cms.string("RunA2011"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    ),
    cms.PSet(
        MinRun=cms.int32(175832),
        MaxRun=cms.int32(180296),
        HLTNames=cms.vstring(),
        HTLRegExpNames=cms.vstring("HLT_DoubleMu7_v.","HLT_Mu13_Mu8_v.","HLT_Mu17_Mu8_v."),
        Name=cms.string("RunB2011"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
    )
)

HLTNoTrigger = cms.VPSet(
    cms.PSet(
        MinRun=cms.int32(0),
        MaxRun=cms.int32(100),
        HLTNames=cms.vstring(),
        Name=cms.string("AllMC"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        ),
    cms.PSet(
        MinRun=cms.int32(132440),
        MaxRun=cms.int32(149442),
        HLTNames=cms.vstring(),
        Name=cms.string("AllRun2010"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        )
    )

HLTMuon = cms.VPSet(
    cms.PSet(
        MinRun=cms.int32(132440),
        MaxRun=cms.int32(144114),
        HLTNames=cms.vstring("HLT_Mu9"),
        Name=cms.string("RunA")
        ),
    cms.PSet(
        MinRun=cms.int32(146428),
        MaxRun=cms.int32(147196),
        HLTNames=cms.vstring("HLT_Mu9","HLT_IsoMu9","HLT_Mu11"),
        Name=cms.string("RunB-6E31")
        ),
    cms.PSet(
        MinRun=cms.int32(147206),
        MaxRun=cms.int32(148058),
        HLTNames=cms.vstring("HLT_IsoMu9","HLT_Mu11","HLT_Mu13_v1","HLT_Mu15_v1"),
        Name=cms.string("6E31")
        ),
    cms.PSet(
        MinRun=cms.int32(148819),
        MaxRun=cms.int32(149442),
        HLTNames=cms.vstring("HLT_Mu15_v1"),
        Name=cms.string("148819-EndRunB")
        )
    )

HLTElectronTTBar = cms.VPSet(
    cms.PSet(
        MinRun=cms.int32(132440),
        MaxRun=cms.int32(137028),
        HLTNames=cms.vstring("HLT_Ele10_LW_L1R"), #They shall OR together
        Name=cms.string("EndOf2E29")
        ),
    cms.PSet(
        MinRun=cms.int32(138564),
        MaxRun=cms.int32(141881),
        HLTNames=cms.vstring("HLT_Ele15_LW_L1R"), #They shall OR together
        Name=cms.string("4E29ToEndOf1.6E30_pre3")
        ),
    cms.PSet(
        MinRun=cms.int32(141956),
        MaxRun=cms.int32(143962),
        HLTNames=cms.vstring("HLT_Ele15_SW_L1R"), #They shall OR together
        Name=cms.string("4.0-v5.3")
        ),
    cms.PSet(
        MinRun=cms.int32(144010),
        MaxRun=cms.int32(149442),
        HLTNames=cms.vstring("HLT_Ele15_SW_CaloEleId_L1R", "HLT_Ele20_SW_L1R", "HLT_DoubleEle10_SW_L1R"), #They shall OR together
        Name=cms.string("v5.4-TillTheEndOfRunB"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        #Notes : 
        # DS = ElDSRunBPRec_DCap ; Err = 147929 - Bit with name HLT_Ele15_SW_CaloEleId_L1R not found!
        # DS = MuDSRunBPRec_DCap ; Err = 147222 - Bit with name HLT_Ele15_SW_CaloEleId_L1R not found!
        )
    )

HLTElectronZandW = cms.VPSet(
    cms.PSet(
        MinRun=cms.int32(132440),
        MaxRun=cms.int32(137028),
        HLTNames=cms.vstring("HLT_Photon10_L1R"), #They shall OR together
        Name=cms.string("EndOf2E29")
        ),
    cms.PSet(
        MinRun=cms.int32(138564),
        MaxRun=cms.int32(140401),
        HLTNames=cms.vstring("HLT_Photon15_Cleaned_L1R"), #They shall OR together
        Name=cms.string("4E29ToEndOf1.6E30_pre7")
        ),
    cms.PSet(
        MinRun=cms.int32(141865),
        MaxRun=cms.int32(141882),
        HLTNames=cms.vstring(), 
        Name=cms.string("1.6E30") #these runs are missing : 874,876,880,881 They don't have a regular trigger menu
        ),
    cms.PSet(
        MinRun=cms.int32(141956),
        MaxRun=cms.int32(144114),
        HLTNames=cms.vstring("HLT_Ele15_SW_CaloEleId_L1R"), 
        Name=cms.string("4.0-v5.4")
        ),
    cms.PSet(
        MinRun=cms.int32(146428),
        MaxRun=cms.int32(147196),
        HLTNames=cms.vstring("HLT_Ele15_SW_CaloEleId_L1R","HLT_Ele17_SW_EleId_L1R"),
        Name=cms.string("RunB-6E31"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        # Notes :
            # DS = ElDSRunBPRec_DCap  ; Err = 147043 - Bit with name HLT_Ele15_SW_CaloEleId_L1R not found!
        ),
    cms.PSet(
        MinRun=cms.int32(147206),
        MaxRun=cms.int32(147284),
        HLTNames=cms.vstring("HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1"),
        Name=cms.string("6E31")
        ),
    cms.PSet(
        MinRun=cms.int32(147390),
        MaxRun=cms.int32(148058),
        HLTNames=cms.vstring("HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1","HLT_Ele17_SW_TightCaloEleId_Ele8HE_L1R_v1"),
        Name=cms.string("147390-148058"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        # Notes :
            ),
    cms.PSet(
        MinRun=cms.int32(148819),
        MaxRun=cms.int32(149442),
        HLTNames=cms.vstring("HLT_Ele15_SW_CaloEleId_L1R","HLT_Ele17_SW_TightCaloEleId_Ele8HE_L1R_v1"),
        Name=cms.string("148819-EndRunB"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        # Notes :
            # DS = MuDSRunBPRec_DCapp ; Err = 149011 - Bit with name HLT_Ele15_SW_CaloEleId_L1R not found!
        # DS = ElDSRunBPRec_DCap  ; Err = 147043 - Bit with name HLT_Ele15_SW_CaloEleId_L1R not found!
        )
    )

HLTSusySS = cms.VPSet(
    cms.PSet(
        MinRun=cms.int32(132440),
        MaxRun=cms.int32(137028),
        HLTNames=cms.vstring("HLT_Ele10_LW_L1R", "HLT_Ele15_LW_L1R", "HLT_DoubleEle5_SW_L1R"),
        Name=cms.string("EndOf2E29")
        ),
    cms.PSet(
        MinRun=cms.int32(138564),
        MaxRun=cms.int32(141881),
        HLTNames=cms.vstring("HLT_Ele10_LW_EleId_L1R", "HLT_DoubleEle5_SW_L1R", "HLT_Ele10_LW_EleId_L1R"),
        Name=cms.string("4E29ToEndOf1.6E30_pre3") #these runs are missing : 874,876,880,881 They don't have a regular trigger menu
        ),
    cms.PSet(
        MinRun=cms.int32(141956),
        MaxRun=cms.int32(144144),
        HLTNames=cms.vstring("HLT_Ele15_SW_L1R", "HLT_Ele10_SW_EleId_L1R", "HLT_Ele15_SW_CaloEleId_L1R", "HLT_Ele15_SW_EleId_L1R"
                             ,"HLT_DoubleEle10_SW_L1R"
                             , "HLT_Ele10_SW_EleId_L1R", "HLT_Ele15_SW_CaloEleId_L1R", "HLT_Ele15_SW_EleId_L1R"),
        Name=cms.string("Till end of runB"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        ),
    cms.PSet(
        MinRun=cms.int32(146428),
        MaxRun=cms.int32(149442),
        HLTNames=cms.vstring("HLT_Ele15_SW_L1R", "HLT_Ele10_SW_EleId_L1R", "HLT_Ele15_SW_CaloEleId_L1R", "HLT_Ele15_SW_EleId_L1R"
                             ,"HLT_DoubleEle10_SW_L1R"
                             , "HLT_Ele10_SW_EleId_L1R", "HLT_Ele15_SW_CaloEleId_L1R", "HLT_Ele15_SW_EleId_L1R"),
        Name=cms.string("Till end of runB"),
        SkipRunsWithInconsistentMenu=cms.untracked.bool(True)
        # Notes :
            # DS = ElDSRunBPRec_DCap  ; Err = 147929 - Bit with name HLT_Ele17_SW_CaloEleId_L1R not found!
        # DS = ElDSRunBPRec_DCap  ; Err = 147929 - Bit with name HLT_Ele17_SW_EleId_L1R not found!
        # DS = ElDSRunBPRec_DCap  ; Err = 147284 - Bit with name HLT_Ele17_SW_LooseEleId_L1R not found!
        # DS = ElDSRunBPRec_DCap  ; Err = 147284 - Bit with name HLT_Ele17_SW_CaloEleId_L1R not found!
        # DS = ElDSRunBPRec_DCap  ; Err = 147284 - Bit with name HLT_Ele17_SW_EleId_L1R not found!
        )
    )
for i in HLTSusySS:
    i.HLTNames.append('HLT_Mu9')
    i.HLTNames.append('HLT_DoubleMu3')
    #i.HLTNames.append('HLT_DoubleMu5_v1')
    #i.HLTNames.append('HLT_Mu11')
    #i.HLTNames.append('HLT_Mu15_v1')

HLTSusySS[3].HLTNames.append('HLT_Ele17_SW_LooseEleId_L1R')
HLTSusySS[3].HLTNames.append("HLT_Ele17_SW_CaloEleId_L1R")
HLTSusySS[3].HLTNames.append("HLT_Ele17_SW_EleId_L1R")
HLTSusySS[3].HLTNames.append("HLT_Ele17_SW_TightCaloEleId_SC8HE_L1R_v1")
HLTSusySS[3].HLTNames.append("HLT_Ele17_SW_TightEleIdIsol_L1R_v1")
HLTSusySS[3].HLTNames.append("HLT_DoubleEle15_SW_L1R_v1")
HLTSusySS[3].HLTNames.append("HLT_Ele17_SW_TightCaloEleId_Ele8HE_L1R_v1")
#HLTSusySS[2].HLTNames.append("HLT_Mu5_Ele9_v1")
#HLTSusySS[2].HLTNames.append("HLT_Mu8_Ele8_v1")
#HLTSusySS[2].HLTNames.append("HLT_Mu5_Ele15_v1")
