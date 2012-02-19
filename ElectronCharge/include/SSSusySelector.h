/* 
 * File:   SSSusyEventSelector.h
 * Author: hbakhshi
 *
 * Created on November 4, 2010, 9:49 PM
 */

#ifndef SSSusyEVENTSELECTOR_H
#define	SSSusyEVENTSELECTOR_H

#include "base/TreeReader.hh"
#include "DataFormats/interface/DiElectronEvent.h"
#include "DataFormats/interface/DiLeptonEventProperties.h"
#include "base/BaseEventSelector.h"
#include <map>

#include "FWCore/ParameterSet/interface/ParameterSet.h"

using namespace std;
using namespace ElectronAnalysis;
using ElectronAnalysis::Muon;
using ElectronAnalysis::Electron;

class SSSusySelectorConfig : public BaseSelectorConfig {
public:
    bool RejectInvMassLessThan12;
    bool RejectZRegion;
    bool RejectLeptonsFromZ;
    double METCutSF;
    double METCutOF;
    double HighLeptonPt;
    int NJets;

    double HTCut;

    edm::ParameterSet SelectedEventTypesByDSName;

    SSSusySelectorConfig(const edm::ParameterSet& ps) : BaseSelectorConfig(ps),
    RejectInvMassLessThan12(ps.getParameter<bool>("RejectInvMassLessThan12")),
    RejectZRegion(ps.getParameter<bool>("RejectZRegion")),
    RejectLeptonsFromZ(ps.getParameter<bool>("RejectLeptonsFromZ")),
    METCutOF(ps.getParameter<double>("METCutOF")),
    METCutSF(ps.getParameter<double>("METCutSF")),
    HTCut(ps.getParameter<double>("HTCut")),
    NJets(ps.getParameter<int>("NJets")),
    HighLeptonPt(ps.getParameter<double>("HighLeptonPt")),
    SelectedEventTypesByDSName(ps.getParameter<edm::ParameterSet>("SelectedEventTypesByDSName"))
    {
//        cout << RejectZRegion << endl;
//        cout << RejectLeptonsFromZ << endl;
    }
};

class SSSusyEventSelector : public BaseEventSelector<DiLepton> {
public:
    typedef DiLepton EVENTTYPE;
    typedef BaseEventSelector<DiLepton> BASE;
    SSSusyEventSelector(TreeReader* tree, SSSusySelectorConfig* config,map<string, bool>* triggerResults);
    ~SSSusyEventSelector();
    multimap<int,int> RunEventNumbers;
    std::vector<double> CurrentFileAcceptedEventTypes;
    ElectronAnalysis::DiLeptonEventProperties::EventType<EVENTTYPE> EventTypeReader;
    ElectronAnalysis::DiLeptonEventProperties::HT<0> HTReader;

    virtual DiLepton* Read(int& stat);

    virtual bool CheckElectron(int eleidx,bool fillHisto = true);
    virtual bool CheckMuon(int muidx,bool fillHisto = true);
    virtual bool CheckJet(int jetIdx);

    virtual void AddSelectionStepsElectron();
    virtual void AddSelectionPlotsElectron();
    virtual void AddSelectionStepsMuon();
    virtual void AddSelectionStepsEvent();
    virtual void AddSelectionPlotsEvent();

    virtual void AddTriggerHistos();
    virtual void End(TDirectory* dir) {
        BASE::End(dir);
    }

    virtual void OnChangeFile(FileInfo* fi, DataSet* ds);

private:
};

typedef SSSusyEventSelector EventSelector;
typedef SSSusySelectorConfig SelectorConfig;
#endif	/* SSSusyEVENTSELECTOR_H */
