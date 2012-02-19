/* 
 * File:   CosThetaAnalysis.cc
 * Author: hbakhshi
 * 
 * Created on February 1, 2012, 3:31 PM
 */

#include "WPolarization/interface/CosThetaAnalysis.h"
#include "TLorentzVector.h"

CosThetaAnalysis::CosThetaAnalysis(const edm::ParameterSet& ps) : BASE(ps),
AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")) {

    hCosThetaPosLepton = new TH1D("hCosThetaPosLepton", "cos(#theta) for positive Lepton", 200, -1., 1.);
    hCosThetaNegLepton = new TH1D("hCosThetaNegLepton", "cos(#theta) for negative Lepton", 200, -1., 1.);
    hCosTheta1stLepton = new TH1D("hCosTheta1stLepton", "cos(#theta) for first Lepton", 200, -1., 1.);
    hCosTheta2ndLepton = new TH1D("hCosTheta2ndLepton", "cos(#theta) for second Lepton", 200, -1., 1.);
}

CosThetaAnalysis::~CosThetaAnalysis() {
}

bool CosThetaAnalysis::Run(TopAnalysis::TTBarDileptonicEvent* ev) {
    if (!ev->hasBeenSolved)
        return false;

    double evtType = EventTypeReader.ReadValue(ev);
    if (std::find(AcceptedEventTypes.begin(), AcceptedEventTypes.end(), evtType) == AcceptedEventTypes.end()) {
        return false;
    }

    //for first lepton :
    double costheta_top = ev->Top_Rec.CosTheta();
    hCosThetaPosLepton->Fill(costheta_top);

    double costheta_tbar = ev->TopBar_Rec.CosTheta();
    hCosThetaNegLepton->Fill(costheta_tbar);

    if (ev->Top_Rec.W.lepton.Pt() > ev->TopBar_Rec.W.lepton.Pt()) {
        hCosTheta1stLepton->Fill(costheta_top);
        hCosTheta2ndLepton->Fill(costheta_tbar);
    }else{
        hCosTheta1stLepton->Fill(costheta_tbar);
        hCosTheta2ndLepton->Fill(costheta_top);
    }
    return true;
}

void CosThetaAnalysis::End() {
    TFile* file = info::TheInfo->OutFile;
    file->cd();
    file->mkdir(this->Name.c_str())->cd();

    hCosThetaPosLepton->Write();
    hCosThetaNegLepton->Write();
    hCosTheta1stLepton->Write();
    hCosTheta2ndLepton->Write();

    cout << this->Name << " ended" << endl;

    file->cd();
}
