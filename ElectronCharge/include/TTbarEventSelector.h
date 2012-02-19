/* 
 * File:   TTbarEventSelector.h
 * Author: hbakhshi
 *
 * Created on November 4, 2010, 9:49 PM
 */

#ifndef TTBAREVENTSELECTOR_H
#define	TTBAREVENTSELECTOR_H

#include "base/TreeReader.hh"
#include "DataFormats/interface/DiElectronEvent.h"

using namespace std;
using namespace ElectronAnalysis;
using ElectronAnalysis::Muon;
using ElectronAnalysis::Electron;

class TTbarEventSelector : public BaseEventSelector<DiLepton> {
public:
    TTbarEventSelector(TreeReader* tree);
    ~TTbarEventSelector();

    static string GetStatusString(int stat) {
        switch (stat) {
            case 1:
                return "OK";
            case 2:
                return "OK, But more than 2 eles";
            case 3:
                return "!OK, more than 2 eles"; //it depends to configurations. sometimes it accepts events with >2 eles
            case 4:
                return "!OK, just 1 eles";
            case 5:
                return "!OK, no eles";
            case 11:
                return "!OK, don't match with gen";
            case 12:
                return "!OK, Jet Veto";
            case 13:
                return "!OK, inv mass";
            case 100:
                return "!OK, user analyzer rejected";
        }

        return "No such status is defined";
    }
    DiLepton* Read(int& stat);

    ElectronAnalysis::Electron* MakeElectron(int eleidx, ElectronAnalysis::BaseElectron*);
    ElectronAnalysis::Muon* MakeMuon(int muonidx, ElectronAnalysis::Muon*);

    DiLepton TheEvent;
    TreeReader *TheTree;

    void SetTree(TreeReader* tree) {
        TheTree = tree;
    }

    double Weight;

    bool CheckElectron(int eleidx,bool fillHisto = true);

    std::vector<int> GoodElecs;
    std::vector<int> GoodMuons;
    std::vector<int> GoodJets;

    bool CheckMuon(int muidx,bool fillhisto = true);


    bool CheckJet(int jetIdx);
};

TTbarEventSelector::TTbarEventSelector(TreeReader* tree) : TheTree(tree) {

}

DiLepton* TTbarEventSelector::Read(int& stat) {
    this->GoodElecs.clear();
    this->GoodMuons.clear();
    this->GoodJets.clear();

    for (int eleid = 0; eleid < TheTree->NEles; eleid++)
        if(this->CheckElectron(eleid))
            this->GoodElecs.push_back(eleid);

    for(int muid = 0 ; muid < TheTree->NMus; muid++)
        if(this->CheckMuon(muid))
            this->GoodMuons.push_back(muid);

    for(int jid = 0; jid < TheTree->NJets; jid++)
        if(this->CheckJet(jid))
            this->GoodJets.push_back(jid);

    if(TheTree->PrimVtxIsFake == 0 && TheTree->PrimVtxNdof > 4 && fabs(TheTree->PrimVtxz) < 24 && TheTree->PrimVtxRho < 2.0){
        if((this->GoodElecs.size() + this->GoodMuons.size()) > 1){
            std::map<double, int, greater_equal<double> > lepton_pts;
            for(int ii = 0 ; ii< GoodElecs.size(); ii++)
                lepton_pts[TheTree->ElPt[GoodElecs.at(ii)]] == ii;

            for(int ij = 0 ; ij< GoodMuons.size(); ij++)
                lepton_pts[TheTree->MuPt[GoodMuons.at(ij)]] == ij + GoodElecs.size();

            map<double, int, greater_equal<double> >::const_iterator itr = lepton_pts.begin();

            ElectronAnalysis::Lepton* firstLepton;
            ElectronAnalysis::Lepton* secondLepton;

            if(itr->second < GoodElecs.size()) //it's an electron
            {
                firstLepton = MakeElectron(itr->second, NULL);
            }else{
                firstLepton = MakeMuon(itr->second - GoodElecs.size() , NULL);
            }

            itr++;
            if(itr->second < GoodElecs.size()) //it's an electron
            {
                secondLepton = MakeElectron(itr->second, NULL);
            }else{
                secondLepton = MakeMuon(itr->second - GoodElecs.size() , NULL);
            }

            
        }
    }

    return NULL;
}

bool TTbarEventSelector::CheckElectron(int eleidx,bool fillHisto) {
    bool ret = true;


    ret = !(this->TheTree->ElEcalDriven[eleidx] == 0);
    ret &= (this->TheTree->ElCaloEnergy[eleidx] > 15.0);
    ret &= (this->TheTree->ElPt[eleidx] > 20.0);

    double dr_muons = GetMinDistance(TheTree->ElEta[eleidx], TheTree->ElPhi[eleidx], TheTree->MuEta, TheTree->MuPhi, TheTree->NMus);
    ret &= (dr_muons > 0.1);

    ret &= CheckEleID(eleidx);
    ret &= CheckEleIso(eleidx);
    ret &= CheckEleD0(eleidx);
    ret &= CheckEleConv(eleidx);

    return ret;
}

ElectronAnalysis::Muon* TTbarEventSelector::MakeMuon(int muonidx, ElectronAnalysis::Muon* TheObj) {
    if(TheObj == NULL)
        TheObj = new ElectronAnalysis::Muon();

    TheObj->Px = TheTree->MuPx[muonidx];
    TheObj->Py = TheTree->MuPy[muonidx];
    TheObj->Pz = TheTree->MuPz[muonidx];
    TheObj->Pt = TheTree->MuPt[muonidx];
    TheObj->E = TheTree->MuE[muonidx];
    TheObj->Eta = TheTree->MuEta[muonidx];
    TheObj->Phi = TheTree->MuPhi[muonidx];
    TheObj->D0BS = TheTree->MuD0BS[muonidx];
    TheObj->D0PV = TheTree->MuD0PV[muonidx];
    TheObj->D0E = TheTree->MuD0E[muonidx];
    TheObj->DzBS = TheTree->MuDzBS[muonidx];
    TheObj->DzPV = TheTree->MuDzPV[muonidx];
    TheObj->DzE = TheTree->MuDzE[muonidx];
    TheObj->Charge = TheTree->MuCharge[muonidx];

    TheObj->MuCaloComp = TheTree->MuCaloComp[muonidx];
    TheObj->MuEem = TheTree->MuEem[muonidx];
    TheObj->MuEhad = TheTree->MuEhad[muonidx];
    TheObj->MuEt = TheTree->MuEt[muonidx];
    TheObj->MuGood = TheTree->MuGood[muonidx];
    TheObj->MuInnerTkNChi2 = TheTree->MuEhad[muonidx];
    TheObj->MuIsAllArbitrated = TheTree->MuIsAllArbitrated[muonidx];
    TheObj->MuIsAllStaMuons = TheTree->MuIsAllStaMuons[muonidx];
    TheObj->MuIsAllTrkMuons = TheTree->MuIsAllTrkMuons[muonidx];
    TheObj->MuIsGMPT = TheTree->MuIsGMPT[muonidx];
    TheObj->MuIsGMStaChiComp = TheTree->MuIsGMStaChiComp[muonidx];
    TheObj->MuIsGMTkChiComp = TheTree->MuIsGMTkChiComp[muonidx];
    TheObj->MuIsGMTkKinkTight = TheTree->MuIsGMTkKinkTight[muonidx];
    TheObj->MuIsGlobalMuon = TheTree->MuIsGlobalMuon[muonidx];
    TheObj->MuIsIso = TheTree->MuIsIso[muonidx];
    TheObj->MuIsTM2DCompLoose = TheTree->MuIsTM2DCompLoose[muonidx];
    TheObj->MuIsTM2DCompTight = TheTree->MuIsTM2DCompTight[muonidx];
    TheObj->MuIsTMLSAngLoose = TheTree->MuIsTMLSAngLoose[muonidx];
    TheObj->MuIsTMLSLoose = TheTree->MuIsTMLSLoose[muonidx];
    TheObj->MuIsTMLSOptLowPtLoose = TheTree->MuIsTMLSOptLowPtLoose[muonidx];
    TheObj->MuIsTMLSTight = TheTree->MuIsTMLSTight[muonidx];
    TheObj->MuIsTMLastStationAngTight = TheTree->MuIsTMLastStationAngTight[muonidx];
    TheObj->MuIsTMOneStationAngLoose = TheTree->MuIsTMOneStationAngLoose[muonidx];
    TheObj->MuIsTMOneStationAngTight = TheTree->MuIsTMOneStationAngTight[muonidx];
    TheObj->MuIsTMOneStationLoose = TheTree->MuIsTMOneStationLoose[muonidx];
    TheObj->MuIsTMOneStationTight = TheTree->MuIsTMOneStationTight[muonidx];
    TheObj->MuIsTrackerMuon = TheTree->MuIsTrackerMuon[muonidx];
    TheObj->MuIsTrkMuonArbitrated = TheTree->MuIsTrkMuonArbitrated[muonidx];
    TheObj->MuIso03EMVetoEt = TheTree->MuIso03EMVetoEt[muonidx];
    TheObj->MuIso03EmEt = TheTree->MuIso03EmEt[muonidx];
    TheObj->MuIso03HadEt = TheTree->MuIso03HadEt[muonidx];
    TheObj->MuIso03HadVetoEt = TheTree->MuIso03HadVetoEt[muonidx];
    TheObj->MuIso03SumPt = TheTree->MuIso03SumPt[muonidx];
    TheObj->MuIso05EmEt = TheTree->MuIso05EmEt[muonidx];
    TheObj->MuIso05HadEt = TheTree->MuIso05HadEt[muonidx];
    TheObj->MuIso05SumPt = TheTree->MuIso05SumPt[muonidx];
    TheObj->MuNChambers = TheTree->MuNChambers[muonidx];
    TheObj->MuNChi2 = TheTree->MuNChi2[muonidx];
    TheObj->MuNGlHits = TheTree->MuNGlHits[muonidx];
    TheObj->MuNMatches = TheTree->MuNMatches[muonidx];
    TheObj->MuNMuHits = TheTree->MuNMuHits[muonidx];
    TheObj->MuNPxHits = TheTree->MuNPxHits[muonidx];
    TheObj->MuNTkHits = TheTree->MuNTkHits[muonidx];
    TheObj->MuOutMomEta = TheTree->MuOutMomEta[muonidx];
    TheObj->MuOutMomPhi = TheTree->MuOutMomPhi[muonidx];
    TheObj->MuOutMomTheta = TheTree->MuOutMomTheta[muonidx];
    TheObj->MuOutMomx = TheTree->MuOutMomx[muonidx];
    TheObj->MuOutMomy = TheTree->MuOutMomy[muonidx];
    TheObj->MuOutMomz = TheTree->MuOutMomz[muonidx];
    TheObj->MuOutPosRadius = TheTree->MuOutPosRadius[muonidx];
    TheObj->MuOutPosX = TheTree->MuOutPosX[muonidx];
    TheObj->MuOutPosY = TheTree->MuOutPosY[muonidx];
    TheObj->MuOutPosZ = TheTree->MuOutPosZ[muonidx];
    TheObj->MuPtE = TheTree->MuPtE[muonidx];
    TheObj->MuRelIso03 = TheTree->MuRelIso03[muonidx];
    TheObj->MuSegmComp = TheTree->MuSegmComp[muonidx];

    TheObj->GenID = TheTree->MuGenID[muonidx];
    TheObj->GenStatus = TheTree->MuGenStatus[muonidx];
    TheObj->GenCharge = TheTree->MuGenCharge[muonidx];
    TheObj->GenPt = TheTree->MuGenPt[muonidx];
    TheObj->GenEta = TheTree->MuGenEta[muonidx];
    TheObj->GenPhi = TheTree->MuGenPhi[muonidx];
    TheObj->GenE = TheTree->MuGenE[muonidx];
    TheObj->GenMID = TheTree->MuGenMID[muonidx];
    TheObj->GenMStatus = TheTree->MuGenMStatus[muonidx];
    TheObj->GenMCharge = TheTree->MuGenMCharge[muonidx];
    TheObj->GenMPt = TheTree->MuGenMPt[muonidx];
    TheObj->GenMEta = TheTree->MuGenMEta[muonidx];
    TheObj->GenMPhi = TheTree->MuGenMPhi[muonidx];
    TheObj->GenME = TheTree->MuGenME[muonidx];
    TheObj->GenGMID = TheTree->MuGenGMID[muonidx];
    TheObj->GenGMStatus = TheTree->MuGenGMStatus[muonidx];
    TheObj->GenGMCharge = TheTree->MuGenGMCharge[muonidx];
    TheObj->GenGMPt = TheTree->MuGenGMPt[muonidx];
    TheObj->GenGMEta = TheTree->MuGenGMEta[muonidx];
    TheObj->GenGMPhi = TheTree->MuGenGMPhi[muonidx];
    TheObj->GenGME = TheTree->MuGenGME[muonidx];

    return (Muon*) TheObj;
}

bool TTbarEventSelector::CheckMuon(int muidx,bool fillhisto) {
    bool ret = true;

    ret &= (TheTree->MuIsGlobalMuon[muidx] != 0);
    ret &= (TheTree->MuIsTrackerMuon[muidx] != 0);
    ret &= (TheTree->MuPt[muidx] > 20);
    ret &= (fabs(TheTree->MuEta[muidx]) < 2.5);
    ret &= TheTree->MuNTkHits[muidx] > 10;
    ret &= TheTree->MuNMuHits[muidx]) > 0;
    ret &= TheTree->MuNChi2[muidx] < 10;
    ret &= fabs(TheTree->MuD0PV[muidx]) < 0.02;
    ret &= TheTree->MuRelIso03[muidx] < 0.15;

    return ret;
}

bool TTbarEventSelector::CheckJet(int jetIdx) {
    bool ret = true;

    ret &= TheTree->PFJPt[jetIdx] > 30;
    ret &= fabs(TheTree->PFJEta[jetIdx]) < 2.5;

    double eleEta[100];
    double elePhi[100];
    for (uint elei = 0; elei < this->GoodElecs.size(); elei++) {
        eleEta[elei] = TheTree->ElEta[this->GoodElecs.at(elei)];
        elePhi[elei] = TheTree->ElPhi[this->GoodElecs.at(elei)];
    }
    double distEle = this->GetMinDistance(TheTree->PFJEta[jetIdx], TheTree->PFJPhi[jetIdx], eleEta, elePhi, this->GoodElecs.size());
    ret &= (distEle > 0.4);

    double muoEta[100];
    double muoPhi[100];
    for (uint muoi = 0; muoi < this->GoodMuons.size(); muoi++) {
        muoEta[muoi] = TheTree->MuEta[this->GoodMuons.at(muoi)];
        muoPhi[muoi] = TheTree->MuPhi[this->GoodMuons.at(muoi)];
    }
    double distMuo = this->GetMinDistance(TheTree->PFJEta[jetIdx], TheTree->PFJPhi[jetIdx], muoEta, muoPhi, this->GoodMuons.size());
    ret &= (distMuo > 0.4);

    ret &= this->IsGoodPFJet(jetIdx);

    return ret;
}

bool TTbarEventSelector::IsGoodPFJet(int index) {
    if (!(TheTree->PFJNConstituents[index] > 1)) return false;
    if (TheTree->PFJNeuEmfrac[index] >= 1.0) return false;
    if (TheTree->PFJNeuHadfrac[index] >= 1.0) return false;
    if (fabs(TheTree->PFJEta[index]) < 2.4) { // Cuts for |eta|<2.4
        if (!(TheTree->PFJChEmfrac[index] < 0.99)) return false;
        if (!(TheTree->PFJChHadfrac[index] > 0.00)) return false;
        if (!(TheTree->PFJChMult[index] > 0)) return false;
    }
    return true;
};
#endif	/* TTBAREVENTSELECTOR_H */

