#include "../interface/DiLeptonTTBarEvent.h"
#include "../interface/DiLeptonEventProperties.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"


#include "WPolarization/interface/DileptonAnalyticalSolver.h"

TopAnalysis::TTBarDileptonicEvent::SolverResults::SolverResults() : Top_Rec(+1),
TopBar_Rec(-1),
Top_Gen(NULL),
TopBar_Gen(NULL),
NumberOfSolutions(0) {

}

TopAnalysis::TTBarDileptonicEvent::SolverResults::SolverResults(string name, math::XYZTLorentzVector the_b, math::XYZTLorentzVector the_bbar,
        math::XYZTLorentzVector the_lminus, math::XYZTLorentzVector the_lplus, bool isPlusEle, bool isMinusEle, double met_x, double met_y, TopDecayChain* tgen, TopDecayChain* tbargen) :
Top_Rec(+1),
TopBar_Rec(-1),
Top_Gen(tgen),
TopBar_Gen(tbargen),
NumberOfSolutions(0) {
    this->Name = name;

    Top_Rec.b.SetXYZT(the_b.X(), the_b.Y(), the_b.Z(), the_b.T());
    Top_Rec.W.lepton.SetXYZT(the_lplus.X(), the_lplus.Y(), the_lplus.Z(), the_lplus.T());
    Top_Rec.W.isElectron = isPlusEle;

    TopBar_Rec.b.SetXYZT(the_bbar.X(), the_bbar.Y(), the_bbar.Z(), the_bbar.T());
    TopBar_Rec.W.lepton.SetXYZT(the_lminus.X(), the_lminus.Y(), the_lminus.Z(), the_lminus.T());
    TopBar_Rec.W.isElectron = isMinusEle;

    this->MET.Set(met_x, met_y);

    ETMiss[0] = MET.X();
    ETMiss[1] = MET.Y();
    BPt[1] = Top_Rec.b.X();
    BPt[2] = Top_Rec.b.Y();
    BPt[3] = Top_Rec.b.Z();
    BPt[0] = Top_Rec.b.T();
    BBarPt[1] = TopBar_Rec.b.X();
    BBarPt[2] = TopBar_Rec.b.Y();
    BBarPt[3] = TopBar_Rec.b.Z();
    BBarPt[0] = TopBar_Rec.b.T();
    LPPt[1] = Top_Rec.W.lepton.X();
    LPPt[2] = Top_Rec.W.lepton.Y();
    LPPt[3] = Top_Rec.W.lepton.Z();
    LPPt[0] = Top_Rec.W.lepton.T();
    LMPt[1] = TopBar_Rec.W.lepton.X();
    LMPt[2] = TopBar_Rec.W.lepton.Y();
    LMPt[3] = TopBar_Rec.W.lepton.Z();
    LMPt[0] = TopBar_Rec.W.lepton.T();
}

void TopAnalysis::TTBarDileptonicEvent::SolverResults::SwapBs() {
    TLorentzVector tmp(Top_Rec.b);
    Top_Rec.b = TopBar_Rec.b;
    TopBar_Rec.b = tmp;
}

TopAnalysis::TTBarDileptonicEvent::SolverResults* TopAnalysis::TTBarDileptonicEvent::AddSolverResults(string name, int the_b_index, int the_bbar_index) {
    TopAnalysis::TTBarDileptonicEvent::SolverResults results(name, Jets[the_b_index], Jets[the_bbar_index],
            GetLepton(-1)->get4Vector(0), GetLepton(+1)->get4Vector(0), GetLepton(+1)->isElectron(), GetLepton(-1)->isElectron(), PFMET.X(), PFMET.Y(), &(TOP_Gen), &(TOPBar_Gen));

    results.Top_Rec.W.leptonIsolationValue = GetLepton(+1)->IsolationValue();
    results.TopBar_Rec.W.leptonIsolationValue = GetLepton(-1)->IsolationValue();

    return & (AllSolutions.insert(std::make_pair(name, results)).first->second);
}

void TopAnalysis::TTBarDileptonicEvent::SolverResults::SetSolverResultsAndAnalyze(vector<double>* pnux, vector<double>* pnuy, vector<double>* pnuz, vector<double>* pnubx, vector<double>* pnuby, vector<double>* pnubz) {

    NumberOfSolutions = pnux->size();
    NeutrinoSolutions.clear();
    NeutrinoBarSolutions.clear();

    for (int i = 0; i < NumberOfSolutions; i++) {
        TLorentzVector lnu;
        lnu.SetXYZM(pnux->at(i), pnuy->at(i), pnuz->at(i), 0.0);
        NeutrinoSolutions.push_back(lnu);

        TLorentzVector lnub;
        lnub.SetXYZM(pnubx->at(i), pnuby->at(i), pnubz->at(i), 0.0);
        NeutrinoBarSolutions.push_back(lnub);
    }

    SolEffMass.clear();
    SolTopDR.clear();
    SolTopBarDR.clear();
    SolTTBarDR.clear();
    SolNuChi2.clear();
    SolNuBarChi2.clear();
    SolNuNuBarChi2.clear();

    TLorentzVector top;
    TLorentzVector top_bar;

    TLorentzVector topG;
    TLorentzVector top_barG;

    TLorentzVector nu;
    TLorentzVector nu_bar;

    TLorentzVector nuG;
    TLorentzVector nu_barG;

    for (int i = 0; i < NumberOfSolutions; i++)
        if (GetTops((solutions) (i + 100))) {
            top = Top_Rec.getTop();
            top_bar = TopBar_Rec.getTop();

            topG = Top_Gen->getTop();
            top_barG = TopBar_Gen->getTop();

            double effMass = (top + top_bar).M();
            if (effMass > 340.0)
                SolEffMass[effMass] = i;
            else
                SolEffMass[1000.0 + i] = i;

            double topDr = ROOT::Math::VectorUtil::DeltaR<TLorentzVector, TLorentzVector > (top, topG);
            double topBDr = ROOT::Math::VectorUtil::DeltaR<TLorentzVector, TLorentzVector > (top_bar, top_barG);

            SolTopDR[topDr] = i;
            SolTopBarDR[topBDr] = i;
            SolTTBarDR[topDr + topBDr] = i;

            nu = Top_Rec.W.neutrino;
            nu_bar = TopBar_Rec.W.neutrino;

            nuG = Top_Gen->W.neutrino;
            nu_barG = TopBar_Gen->W.neutrino;


            double nuChi2 = (nu - nuG).Mag();
            double nubChi2 = (nu_bar - nu_barG).Mag();

            SolNuChi2[ nuChi2 ] = i;
            SolNuBarChi2[nubChi2] = i;
            SolNuNuBarChi2[ sqrt(nuChi2 * nuChi2 + nubChi2 * nubChi2) ] = i;
        }
}

bool TopAnalysis::TTBarDileptonicEvent::SolverResults::GetTops(solutions solution, TopDecayChain** top, TopDecayChain** topbar) {
    int solution_id(-1);
    if (NumberOfSolutions == 0)
        return false;

    if (NumberOfSolutions != NeutrinoSolutions.size())
        cout << NumberOfSolutions << "  " << NeutrinoSolutions.size() << endl;

    switch (solution) {
        case MinEffMass:
            solution_id = SolEffMass.begin()->second;
            break;
        case MinNuChi2:
            solution_id = SolNuChi2.begin()->second;
            break;
        case MinNuNubarChi2:
            solution_id = SolNuNuBarChi2.begin()->second;
            break;
        case MinNubarChi2:
            solution_id = SolNuBarChi2.begin()->second;
            break;
        case MinTTbarDR:
            solution_id = SolTTBarDR.begin()->second;
            break;
        case MinTopDR:
            solution_id = SolTopDR.begin()->second;
            break;
        case MinTopbarDR:
            solution_id = SolTopBarDR.begin()->second;
            break;
        default:
            solution_id = (int) solution - 100;
            break;
    }

    if (solution_id >= NumberOfSolutions)
        return false;

    this->Top_Rec.W.neutrino.SetXYZT(NeutrinoSolutions[solution_id].X(), NeutrinoSolutions[solution_id].Y(), NeutrinoSolutions[solution_id].Z(), NeutrinoSolutions[solution_id].T());
    this->TopBar_Rec.W.neutrino.SetXYZT(NeutrinoBarSolutions[solution_id].X(), NeutrinoBarSolutions[solution_id].Y(), NeutrinoBarSolutions[solution_id].Z(), NeutrinoBarSolutions[solution_id].T());

    if (top != NULL)
        (*top) = &Top_Rec;
    if (topbar != NULL)
        (*topbar) = &TopBar_Rec;

    return true;
}

bool TopAnalysis::TTBarDileptonicEvent::SelectASolution(string name, SolverResults::solutions solution) {
    if (this->AllSolutions[name].NumberOfSolutions == 0)
        return false;

    if (solution >= this->AllSolutions[name].NumberOfSolutions || solution < 0)
        return false;

    this->AllSolutions[name].GetTops(solution, & this->Top_Rec, & this->TopBar_Rec);

    return true;
}

TopAnalysis::TTBarDileptonicEvent::TTBarDileptonicEvent() : DiLepton(),
TOP_Gen(+1),
TOPBar_Gen(-1),
Top_Rec(NULL),
TopBar_Rec(NULL) {
    GenDecayMode = NOT_SET;
    RecDecayMode = NOT_SET;
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
    if (nele == 2) {
        if (this->TRG_DiEle)
            this->RecDecayMode = DiEle;
        else
            this->RecDecayMode = EE_Inconsistent_With_Trigger;
    } else if (nele == 1 && nPmuons == 1) {
        if (this->TRG_EleMuon)
            this->RecDecayMode = ElM_MuP;
        else
            this->RecDecayMode = EM_Inconsistent_With_Trigger;
    } else if (nele == 1 && nNmuons == 1) {
        if (this->TRG_EleMuon)
            this->RecDecayMode = ElP_MuM;
        else
            this->RecDecayMode = EM_Inconsistent_With_Trigger;
    } else {
        if (this->TRG_DiMuon)
            this->RecDecayMode = DiMu;
        else
            this->RecDecayMode = MM_Inconsistent_With_Trigger;
    }


    //    math::XYZTLorentzVector ll1 = lep1->get4Vector(0);
    //    math::XYZTLorentzVector ll2 = lep2->get4Vector(0);
    //
    //    if (lep1->Charge > 0) //it is for T
    //    {
    //        Top_Rec.W.lepton.SetXYZT(ll1.x(), ll1.y(), ll1.z(), ll1.t());
    //        TopBar_Rec.W.lepton.SetXYZT(ll2.x(), ll2.y(), ll2.z(), ll2.t());
    //    } else {
    //        TopBar_Rec.W.lepton.SetXYZT(ll1.x(), ll1.y(), ll1.z(), ll1.t());
    //        Top_Rec.W.lepton.SetXYZT(ll2.x(), ll2.y(), ll2.z(), ll2.t());
    //    }
}

double TopAnalysis::TTBarDileptonicEvent::TopDecayChain::CosThetaTopW() const {
    TLorentzVector w = getW();
    TLorentzVector top = getTop();
    TLorentzVector b = b;

    top.Boost(-w.BoostVector());
    b.Boost(-w.BoostVector());

    return ROOT::Math::VectorUtil::CosTheta(b.Vect(), top.Vect());

    TLorentzVector w2 = getW();
    TLorentzVector top2 = getTop();

    w2.Boost(-top2.BoostVector());

    double ret= ROOT::Math::VectorUtil::CosTheta(w2.Vect(), top.Vect());
    
//    if(ret > -0.9){
//        cout << "----------------" << endl;
//        w.Print();
//        top2.Print();
//        cout << endl;
//        w2.Print();
//        top.Print();
//        cout << "angle :" << ret << endl;
//        cout << "----------------" << endl;
//    }
    
    return ret;
}

double TopAnalysis::TTBarDileptonicEvent::TopDecayChain::CosThetaTopSpin() const {
    TLorentzVector w = getW();
    TLorentzVector lep = W.lepton;
    TLorentzVector top = getTop();

    lep.Boost(-top.BoostVector());
    //w.Boost(-top.BoostVector());
        
    //lep.Boost( -w.BoostVector() );

    return ROOT::Math::VectorUtil::CosTheta(lep.Vect(), top.Vect());
}

double TopAnalysis::TTBarDileptonicEvent::TopDecayChain::CosTheta(int method) const {
    TLorentzVector w = getW();
    TLorentzVector lep = W.lepton;
    TLorentzVector top = getTop();

    if (method == 1) {
        lep.Boost(-w.BoostVector());
        top.Boost(-w.BoostVector());

        double val = ROOT::Math::VectorUtil::CosTheta(lep.Vect(), -(top.Vect()));
        //cout << val << endl;
        return val;
    } else if (method == 2) {
        lep.Boost(-top.BoostVector());
        w.Boost(-top.BoostVector());
        
        lep.Boost( -w.BoostVector() );

        return ROOT::Math::VectorUtil::CosTheta(lep.Vect(), w.Vect());
    } else if (method == 3) {
        lep.Boost(-w.BoostVector());
        w.Boost(-top.BoostVector());

        return ROOT::Math::VectorUtil::CosTheta(lep.Vect(), w.Vect());
    } else 
        throw -10;
}

void TopAnalysis::TTBarDileptonicEvent::TopDecayChain::Clear() {
    b.SetXYZT(0, 0, 0, 0);
    W.lepton.SetXYZT(0, 0, 0, 0);
    W.neutrino.SetXYZT(0, 0, 0, 0);
}
