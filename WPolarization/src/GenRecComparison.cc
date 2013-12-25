/* 
 * File:   GenRecComparison.cc
 * Author: hbakhshi
 * 
 * Created on February 8, 2012, 3:45 PM
 */

#include "WPolarization/interface/GenRecComparison.h"

using namespace TopAnalysis::DiLeptonTTBarEventProperties;

GenRecComparison::GenRecComparison(const edm::ParameterSet& ps) : BASE(ps),
top_histos("Top"),
antitop_histos("AntiTop"),
event_histos("Statistics"),
gen_info("GenInfo"),
AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")),
solverSolution((TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions)(ps.getParameter<int >("SolverSolution"))),
solverName(ps.getParameter<string >("SolverName")) {
    event_histos.AddHisto1(new TopAnalysis::DiLeptonTTBarEventProperties::TTBarDecayMode < false > ());
    event_histos.AddHisto1(new TopAnalysis::DiLeptonTTBarEventProperties::TTBarDecayMode < true > ());
    event_histos.AddHisto1(&EventTypeReader);
    event_histos.AddHisto2<TopAnalysis::DiLeptonTTBarEventProperties::TTBarDecayMode < false >, TopAnalysis::DiLeptonTTBarEventProperties::TTBarDecayMode < true > >();
    event_histos.AddHisto2<TopAnalysis::DiLeptonTTBarEventProperties::TTBarDecayMode < false >, TopAnalysis::DiLeptonTTBarEventProperties::EventType > ();

    antitop_histos.AddHisto1<GenRecComp < 2, 1, 1 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 1, 2 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 1, 3 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 1, 4 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 1, 5 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 1, 6 > >();
    antitop_histos.AddHisto2<GenRecComp < 2, 1, 2 >, GenRecComp < 2, 1, 4 > >();

    antitop_histos.AddHisto1<GenRecComp < 2, 2, 1 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 2, 2 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 2, 3 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 2, 4 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 2, 5 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 2, 6 > >();
    antitop_histos.AddHisto2<GenRecComp < 2, 2, 2 >, GenRecComp < 2, 2, 4 > >();

    antitop_histos.AddHisto1<GenRecComp < 2, 3, 1 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 3, 2 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 3, 3 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 3, 4 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 3, 5 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 3, 6 > >();
    antitop_histos.AddHisto2<GenRecComp < 2, 3, 2 >, GenRecComp < 2, 3, 4 > >();

    antitop_histos.AddHisto1<GenRecComp < 2, 4, 1 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 4, 2 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 4, 3 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 4, 4 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 4, 5 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 4, 6 > >();
    antitop_histos.AddHisto2<GenRecComp < 2, 4, 2 >, GenRecComp < 2, 4, 4 > >();

    antitop_histos.AddHisto1<GenRecComp < 2, 5, 1 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 5, 2 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 5, 3 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 5, 4 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 5, 5 > >();
    antitop_histos.AddHisto1<GenRecComp < 2, 5, 6 > >();
    antitop_histos.AddHisto2<GenRecComp < 2, 5, 2 >, GenRecComp < 2, 5, 4 > >();

    top_histos.AddHisto1<GenRecComp < 1, 1, 1 > >();
    top_histos.AddHisto1<GenRecComp < 1, 1, 2 > >();
    top_histos.AddHisto1<GenRecComp < 1, 1, 3 > >();
    top_histos.AddHisto1<GenRecComp < 1, 1, 4 > >();
    top_histos.AddHisto1<GenRecComp < 1, 1, 5 > >();
    top_histos.AddHisto1<GenRecComp < 1, 1, 6 > >();
    top_histos.AddHisto2<GenRecComp < 1, 1, 2 >, GenRecComp < 1, 1, 4 > >();

    top_histos.AddHisto1<GenRecComp < 1, 2, 1 > >();
    top_histos.AddHisto1<GenRecComp < 1, 2, 2 > >();
    top_histos.AddHisto1<GenRecComp < 1, 2, 3 > >();
    top_histos.AddHisto1<GenRecComp < 1, 2, 4 > >();
    top_histos.AddHisto1<GenRecComp < 1, 2, 5 > >();
    top_histos.AddHisto1<GenRecComp < 1, 2, 6 > >();
    top_histos.AddHisto2<GenRecComp < 1, 2, 2 >, GenRecComp < 1, 2, 4 > >();

    top_histos.AddHisto1<GenRecComp < 1, 3, 1 > >();
    top_histos.AddHisto1<GenRecComp < 1, 3, 2 > >();
    top_histos.AddHisto1<GenRecComp < 1, 3, 3 > >();
    top_histos.AddHisto1<GenRecComp < 1, 3, 4 > >();
    top_histos.AddHisto1<GenRecComp < 1, 3, 5 > >();
    top_histos.AddHisto1<GenRecComp < 1, 3, 6 > >();
    top_histos.AddHisto2<GenRecComp < 1, 3, 2 >, GenRecComp < 1, 3, 4 > >();

    top_histos.AddHisto1<GenRecComp < 1, 4, 1 > >();
    top_histos.AddHisto1<GenRecComp < 1, 4, 2 > >();
    top_histos.AddHisto1<GenRecComp < 1, 4, 3 > >();
    top_histos.AddHisto1<GenRecComp < 1, 4, 4 > >();
    top_histos.AddHisto1<GenRecComp < 1, 4, 5 > >();
    top_histos.AddHisto1<GenRecComp < 1, 4, 6 > >();
    top_histos.AddHisto2<GenRecComp < 1, 4, 2 >, GenRecComp < 1, 4, 4 > >();

    top_histos.AddHisto1<GenRecComp < 1, 5, 1 > >();
    top_histos.AddHisto1<GenRecComp < 1, 5, 2 > >();
    top_histos.AddHisto1<GenRecComp < 1, 5, 3 > >();
    top_histos.AddHisto1<GenRecComp < 1, 5, 4 > >();
    top_histos.AddHisto1<GenRecComp < 1, 5, 5 > >();
    top_histos.AddHisto1<GenRecComp < 1, 5, 6 > >();
    top_histos.AddHisto2<GenRecComp < 1, 5, 2 >, GenRecComp < 1, 5, 4 > >();
    
    
    gen_info.AddHisto1<GenRecComp < 1, 1, 7 > >();
    gen_info.AddHisto1<GenRecComp < 1, 1, 8 > >();
    gen_info.AddHisto1<GenRecComp < 1, 2, 7 > >();
    gen_info.AddHisto1<GenRecComp < 1, 2, 8 > >();
    gen_info.AddHisto1<GenRecComp < 1, 5, 7 > >();
    gen_info.AddHisto1<GenRecComp < 1, 5, 8 > >();

    gen_info.AddHisto1<GenRecComp < 2, 1, 7 > >();
    gen_info.AddHisto1<GenRecComp < 2, 1, 8 > >();
    gen_info.AddHisto1<GenRecComp < 2, 2, 7 > >();
    gen_info.AddHisto1<GenRecComp < 2, 2, 8 > >();
    gen_info.AddHisto1<GenRecComp < 2, 5, 7 > >();
    gen_info.AddHisto1<GenRecComp < 2, 5, 8 > >();
}

GenRecComparison::~GenRecComparison() {
}

bool GenRecComparison::Run(TopAnalysis::TTBarDileptonicEvent* ev) {

    double evtType = EventTypeReader.ReadValue(ev);
    if (std::find(AcceptedEventTypes.begin(), AcceptedEventTypes.end(), evtType) == AcceptedEventTypes.end()) {
        return false;
    }

    //cout << "   " << ev->GenDecayMode << endl;
    if (!ev->hasGenInfo)
        return false;

    if(!ev->SelectASolution(solverName, solverSolution))
        return false;
    
    this->event_histos.Fill(ev);

    //    if (ev->GenDecayMode == TopAnalysis::TTBarDileptonicEvent::DiEle ||
    //            ev->GenDecayMode == TopAnalysis::TTBarDileptonicEvent::DiMu ||
    //            ev->GenDecayMode == TopAnalysis::TTBarDileptonicEvent::ElM_MuP ||
    //            ev->GenDecayMode == TopAnalysis::TTBarDileptonicEvent::ElP_MuM) {

    if (ev->GenDecayMode == ev->RecDecayMode) {
        this->top_histos.Fill(ev);
        this->antitop_histos.Fill(ev);

        this->gen_info.Fill(ev);
        
        return true;
    } else
        return false;
}

void GenRecComparison::End() {
    TFile* file = info::TheInfo->OutFile;
    file->cd();
    TDirectory* dir = file->mkdir(this->Name.c_str());
    dir->cd();
    this->top_histos.Write(dir);
    this->antitop_histos.Write(dir);
    this->event_histos.Write(dir);
    this->gen_info.Write(dir);

    file->cd();
}
