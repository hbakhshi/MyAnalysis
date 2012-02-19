#include "../interface/DiLeptonTTBarEvent.h"
#include "../interface/DiLeptonEventProperties.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"

void TopAnalysis::TTBarDileptonicEvent::SolverResults::AnalyzeSolutions() {
    SolEffMass.clear();
    SolTopDR.clear();
    SolTopBarDR.clear();
    SolTTBarDR.clear();
    SolNuChi2.clear();
    SolNuBarChi2.clear();
    SolNuNuBarChi2.clear();

    TopAnalysis::DiLeptonTTBarEventProperties::EffectiveMass<false> effMass;
    TopAnalysis::DiLeptonTTBarEventProperties::GenRecComp<1 , 1 , 1 > topDr;
    TopAnalysis::DiLeptonTTBarEventProperties::GenRecComp<2 , 1 , 1 > topBDr;
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* ttbarDr = topDr + &topBDr;
    TopAnalysis::DiLeptonTTBarEventProperties::GenRecComp<1 , 5 , 5 > nuchi2;
    TopAnalysis::DiLeptonTTBarEventProperties::GenRecComp<2 , 5 , 5 > nubchi2;
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* nunubarchi2 = sqrt( * (nuchi2*(&nuchi2)) + nubchi2* (&nubchi2) );
    

    for (int i = 0; i < NumberOfSolutions; i++)
        if (SelectASolution(i)) {
            SolEffMass[effMass.ReadValue(this)] = i;
            SolTopDR[topDr.ReadValue(this)] = i;
            SolTopBarDR[topBDr.ReadValue(this)] = i;
            SolTTBarDR[ttbarDr->ReadValue(this)] = i;
            SolNuChi2[nuchi2.ReadValue(this)] = i;
            SolNuBarChi2[nubchi2.ReadValue(this)] = i;
            SolNuNuBarChi2[nunubarchi2->ReadValue(this)] = i;
        }
}

TopAnalysis::TTBarDileptonicEvent::SolverResults::SolverResults(vector<double> pnux, vector<double> pnuy, vector<double> pnuz, vector<double> pnubx, vector<double> pnuby, vector<double> pnubz) {
    NumberOfSolutions = pnux.size();
    NeutrinoSolutions.clear();
    NeutrinoBarSolutions.clear();

    for (int i = 0; i < NumberOfSolutions; i++) {
        TLorentzVector lnu;
        lnu.SetXYZM(pnux[i], pnuy[i], pnuz[i], 0.0);
        NeutrinoSolutions.push_back(lnu);

        TLorentzVector lnub;
        lnub.SetXYZM(pnubx[i], pnuby[i], pnubz[i], 0.0);
        NeutrinoBarSolutions.push_back(lnub);
    }
}

bool TopAnalysis::TTBarDileptonicEvent::SelectASolution(string name, SolverResults::solutions solution) {
    if (NumberOfSolutions == 0)
        return false;

    if (solution >= NumberOfSolutions || solution < 0)
        return false;

    Top_Rec.W.neutrino = this->AllSolutions[name].NeutrinoSolutions[solution];
    TopBar_Rec.W.neutrino = NeutrinoBarSolutions[solution];
    return true;
}

TopAnalysis::TTBarDileptonicEvent::TTBarDileptonicEvent() : DiLepton(),
TOP_Gen(+1),
TOPBar_Gen(-1),
Top_Rec(+1),
TopBar_Rec(-1),
GenDecayMode(NOT_SET),
RecDecayMode(NOT_SET) {
    hasBeenSolved = false;
    hasGenInfo = false;
};

void TopAnalysis::TTBarDileptonicEvent::SetLeptons(Lepton* lep1, Lepton* lep2) {
    hasBeenSolved = false;
    //hasGenInfo = false;            

    this->FirstElectron = lep1;
    this->SecondElectron = lep2;

    int nele(0), nPmuons(0), nNmuons(0);
    this->FirstElectron->isElectron() ? nele++ : (this->FirstElectron->Charge > 0 ? nPmuons++ : nNmuons++);
    this->SecondElectron->isElectron() ? nele++ : (this->SecondElectron->Charge > 0 ? nPmuons++ : nNmuons++);
    if (nele == 2)
        this->RecDecayMode = DiEle;
    else if (nele == 1 && nPmuons == 1)
        this->RecDecayMode = ElM_MuP;
    else if (nele == 1 && nNmuons == 1)
        this->RecDecayMode = ElP_MuM;
    else
        this->RecDecayMode = DiMu;


    math::XYZTLorentzVector ll1 = lep1->get4Vector(0);
    math::XYZTLorentzVector ll2 = lep2->get4Vector(0);

    if (lep1->Charge > 0) //it is for T
    {
        Top_Rec.W.lepton.SetXYZT(ll1.x(), ll1.y(), ll1.z(), ll1.t());
        TopBar_Rec.W.lepton.SetXYZT(ll2.x(), ll2.y(), ll2.z(), ll2.t());
    } else {
        TopBar_Rec.W.lepton.SetXYZT(ll1.x(), ll1.y(), ll1.z(), ll1.t());
        Top_Rec.W.lepton.SetXYZT(ll2.x(), ll2.y(), ll2.z(), ll2.t());
    }
}

double TopAnalysis::TTBarDileptonicEvent::TopDecayChain::CosTheta() const {
    TLorentzVector w = getW();
    TLorentzVector lep = W.lepton;
    TLorentzVector top = getTop();

    lep.Boost(-w.BoostVector());
    w.Boost(-top.BoostVector());

    return ROOT::Math::VectorUtil::CosTheta(lep, w);
}

void TopAnalysis::TTBarDileptonicEvent::TopDecayChain::Clear() {
    b.SetXYZT(0, 0, 0, 0);
    W.lepton.SetXYZT(0, 0, 0, 0);
    W.neutrino.SetXYZT(0, 0, 0, 0);
}
