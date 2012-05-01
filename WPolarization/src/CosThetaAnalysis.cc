/* 
 * File:   CosThetaAnalysis.cc
 * Author: hbakhshi
 * 
 * Created on February 1, 2012, 3:31 PM
 */

#include "WPolarization/interface/CosThetaAnalysis.h"
#include "TLorentzVector.h"

CosThetaAnalysis::CosThetaAnalysis(const edm::ParameterSet& ps) : BASE(ps),
AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")),
solverSolution((TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions)(ps.getParameter<int >("SolverSolution"))),
solverName(ps.getParameter<string >("SolverName")),
FillGen(ps.getParameter<bool >("FillGen")),
FillRec(ps.getParameter<bool >("FillRec")) {


    if (FillGen) {
        vector<int> GenDecayModesINT = ps.getParameter< vector<int> >("GenDecayModes");
        for (vector<int>::const_iterator int_mode = GenDecayModesINT.begin(); int_mode != GenDecayModesINT.end(); int_mode++)
            GenDecayModes.push_back((TopAnalysis::TTBarDileptonicEvent::TopDecays)(*int_mode));
        
        hCosThetaPosLepton_Gen = new TH1D("hCosThetaPosLepton_Gen", "cos(#theta) for positive Lepton (Gen)", 100, -1., 1.);
        hCosThetaNegLepton_Gen = new TH1D("hCosThetaNegLepton_Gen", "cos(#theta) for negative Lepton (Gen)", 100, -1., 1.);

        hCosThetaPtLep_Gen = new TH2D("hCosThetaPtLep_Gen", "cos(#theta) vs. p_t of lepton", 100, -1, 1, 200, 0, 200);
    }
    if (FillRec) {
        hCosThetaPosLepton = new TH1D("hCosThetaPosLepton", "cos(#theta) for positive Lepton", 100, -1., 1.);
        hCosThetaNegLepton = new TH1D("hCosThetaNegLepton", "cos(#theta) for negative Lepton", 100, -1., 1.);

        hCosTheta1stLepton = new TH1D("hCosTheta1stLepton", "cos(#theta) for first Lepton", 100, -1., 1.);
        hCosTheta2ndLepton = new TH1D("hCosTheta2ndLepton", "cos(#theta) for second Lepton", 100, -1., 1.);

        hCosThetaAllLeptonsUnWeighted = new TH1D("hCosThetaAllLeptonsUnWeighted", "cos(#theta) for all Lepton, No weight", 100, -1., 1.);
    }
}

CosThetaAnalysis::~CosThetaAnalysis() {
}

bool CosThetaAnalysis::Run(TopAnalysis::TTBarDileptonicEvent* ev) {

    if (FillGen) {
        if (std::find(this->GenDecayModes.begin(), this->GenDecayModes.end(), ev->GenDecayMode) != this->GenDecayModes.end())
        {
            hCosThetaPosLepton_Gen->Fill(ev->TOP_Gen.CosTheta(), ev->Weight);
            hCosThetaNegLepton_Gen->Fill(ev->TOPBar_Gen.CosTheta(), ev->Weight);

            hCosThetaPtLep_Gen->Fill(ev->TOP_Gen.CosTheta(), ev->TOP_Gen.W.lepton.Pt(), ev->Weight);
            hCosThetaPtLep_Gen->Fill(ev->TOPBar_Gen.CosTheta(), ev->TOPBar_Gen.W.lepton.Pt(), ev->Weight);
        }
    }
    if (!FillRec)
        return true;

    double evtType = EventTypeReader.ReadValue(ev);
    if (std::find(AcceptedEventTypes.begin(), AcceptedEventTypes.end(), evtType) == AcceptedEventTypes.end()) {
        return false;
    }

    if (!ev->SelectASolution(solverName, solverSolution))
        return false;

    //for first lepton :

    double costheta_top = ev->Top_Rec->CosTheta();
    hCosThetaPosLepton->Fill(costheta_top, ev->Weight);

    double costheta_tbar = ev->TopBar_Rec->CosTheta();
    hCosThetaNegLepton->Fill(costheta_tbar, ev->Weight);

    hCosThetaAllLeptonsUnWeighted->Fill(costheta_top);
    hCosThetaAllLeptonsUnWeighted->Fill(costheta_tbar);

    if (ev->Top_Rec->W.lepton.Pt() > ev->TopBar_Rec->W.lepton.Pt()) {
        hCosTheta1stLepton->Fill(costheta_top, ev->Weight);
        hCosTheta2ndLepton->Fill(costheta_tbar, ev->Weight);
    } else {
        hCosTheta1stLepton->Fill(costheta_tbar, ev->Weight);
        hCosTheta2ndLepton->Fill(costheta_top, ev->Weight);
    }
    return true;
}

void CosThetaAnalysis::End() {
    TFile* file = info::TheInfo->OutFile;
    file->cd();
    file->mkdir(this->Name.c_str())->cd();

    if (FillRec) {
        hCosThetaPosLepton->Write();
        dGammaFunctin::Fit(hCosThetaPosLepton, 10)->Write();
        dGammaFunctin::Fit(hCosThetaPosLepton, 10, true, true)->Write();

        hCosThetaNegLepton->Write();
        dGammaFunctin::Fit(hCosThetaNegLepton, 10)->Write();
        dGammaFunctin::Fit(hCosThetaNegLepton, 10, true, true)->Write();

        TH1D hAddRec("hCosThetaAllLepton", "cos(#theta) for all Leptons", 100, -1., 1.);
        hAddRec.Add(hCosThetaPosLepton, hCosThetaNegLepton);
        hAddRec.Write();
        dGammaFunctin::Fit(&hAddRec, 10)->Write();
        dGammaFunctin::Fit(&hAddRec, 10, true, true)->Write();

        hCosTheta1stLepton->Write();
        dGammaFunctin::Fit(hCosTheta1stLepton, 10)->Write();
        dGammaFunctin::Fit(hCosTheta1stLepton, 10, true, true)->Write();

        hCosTheta2ndLepton->Write();
        dGammaFunctin::Fit(hCosTheta2ndLepton, 10)->Write();
        dGammaFunctin::Fit(hCosTheta2ndLepton, 10, true, true)->Write();

        hCosThetaAllLeptonsUnWeighted->Write();
        dGammaFunctin::Fit(hCosThetaAllLeptonsUnWeighted, 10)->Write();
        dGammaFunctin::Fit(hCosThetaAllLeptonsUnWeighted, 10, true, true)->Write();
    }

    if (FillGen) {
        hCosThetaPosLepton_Gen->Write();
        dGammaFunctin::Fit(hCosThetaPosLepton_Gen, 10)->Write();
        dGammaFunctin::Fit(hCosThetaPosLepton_Gen, 10, true, true)->Write();

        hCosThetaNegLepton_Gen->Write();
        dGammaFunctin::Fit(hCosThetaNegLepton_Gen, 10)->Write();
        dGammaFunctin::Fit(hCosThetaNegLepton_Gen, 10, true, true)->Write();


        TH1D hAddGen("hCosThetaAllLepton_Gen", "cos(#theta) for all Leptons (Gen)", 100, -1., 1.);
        hAddGen.Add(hCosThetaPosLepton_Gen, hCosThetaNegLepton_Gen);
        hAddGen.Write();
        dGammaFunctin::Fit(&hAddGen, 10)->Write();

        hCosThetaPtLep_Gen->Write();
    }
    //cout << this->Name << " ended" << endl;
    file->cd();
}
