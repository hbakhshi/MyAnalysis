#ifndef TTbarEVENTSELECTOR_H
#define	TTbarEVENTSELECTOR_H

#include "base/TreeReader.hh"
#include "DataFormats/interface/DiLeptonTTBarEvent.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "base/BaseEventSelector.h"
#include <map>

#include "FWCore/ParameterSet/interface/ParameterSet.h"

using namespace std;
using namespace ElectronAnalysis;
using namespace TopAnalysis;
using ElectronAnalysis::Muon;
using ElectronAnalysis::Electron;

class TTbarSelectorConfig : public BaseSelectorConfig {
public:
    bool RejectInvMassLessThan12;
    bool RejectZRegion;
    bool RejectLeptonsFromZ;
    double METCutSF;
    double METCutOF;    
    int NJets;

    double DRJetsLeptons;
    
    bool IsTTBarSample;
    
    string btag_algo;
    int getBTagAlgo(){
        if( btag_algo == "TrackCountingHighEff" )
            return 1;
        
        if( btag_algo == "simpleSecondaryVertexHighEff" )
            return 2;
        
        return 100;
    }
    double BTag1,BTag2;
    
    edm::ParameterSet SelectedEventTypesByDSName;

    TTbarSelectorConfig(const edm::ParameterSet& ps) : BaseSelectorConfig(ps),
    RejectInvMassLessThan12(ps.getParameter<bool>("RejectInvMassLessThan12")),
    RejectZRegion(ps.getParameter<bool>("RejectZRegion")),
    RejectLeptonsFromZ(ps.getParameter<bool>("RejectLeptonsFromZ")),
    METCutOF(ps.getParameter<double>("METCutOF")),
    METCutSF(ps.getParameter<double>("METCutSF")),    
    NJets(ps.getParameter<int>("NJets")),    
    SelectedEventTypesByDSName(ps.getParameter<edm::ParameterSet>("SelectedEventTypesByDSName")),
    btag_algo(ps.getParameter<string>("btag_algo")),
    BTag1(ps.getParameter<double>("BTag1")),    
    BTag2(ps.getParameter<double>("BTag2")),
    DRJetsLeptons(ps.getParameter<double>("DRJetsLeptons")),
    IsTTBarSample(ps.getUntrackedParameter<bool>("TTBarSample" , false)){
        //        cout << RejectZRegion << endl;
        //        cout << RejectLeptonsFromZ << endl;
    }
};

class TTbarEventSelector : public BaseEventSelector<TTBarDileptonicEvent> {
public:
    typedef TTBarDileptonicEvent EVENTTYPE;
    typedef BaseEventSelector<TTBarDileptonicEvent> BASE;
    TTbarEventSelector(TreeReader* tree, TTbarSelectorConfig* config, map<string, bool>* triggerResults);
    ~TTbarEventSelector();
    multimap<int, int> RunEventNumbers;
    std::vector<double> CurrentFileAcceptedEventTypes;
    TopAnalysis::DiLeptonTTBarEventProperties::EventType EventTypeReader;    
    ObjectProperty<TTBarDileptonicEvent>* btag_1;
    ObjectProperty<TTBarDileptonicEvent>* btag_2;

    virtual TTBarDileptonicEvent* Read(int& stat);

    virtual bool CheckElectron(int eleidx, bool fillHisto = true);
    virtual bool CheckMuon(int muidx, bool fillHisto = true);
    virtual bool CheckJet(int jetIdx);

    virtual void AddSelectionStepsElectron();
    virtual void AddSelectionPlotsElectron();
    virtual void AddSelectionStepsMuon();

    void SetGenTops();
    
    enum TTbarEventSelectionSteps {
        TTbarEventSelectionSteps_All = 0,
        TTbarEventSelectionSteps_Trigger = 1,
        TTbarEventSelectionSteps_Cleaning = 2,
        TTbarEventSelectionSteps_PairChoose = 3,
        TTbarEventSelectionSteps_SameFlavours = 4,
        TTbarEventSelectionSteps_SameFlavours_InvMass12 = 5,
        TTbarEventSelectionSteps_SameFlavours_InvMassZ = 6,
        TTbarEventSelectionSteps_SameFlavours_NJets = 7,
        TTbarEventSelectionSteps_SameFlavours_MET = 8,
        TTbarEventSelectionSteps_SameFlavours_bTag1 = 9,
        TTbarEventSelectionSteps_SameFlavours_bTag2 = 10,
        TTbarEventSelectionSteps_OppositeFlavours = 11,
        TTbarEventSelectionSteps_OppositeFlavours_NJets = 12,
        TTbarEventSelectionSteps_OppositeFlavours_MET = 13,
        TTbarEventSelectionSteps_OppositeFlavours_bTag1 = 14,
        TTbarEventSelectionSteps_OppositeFlavours_bTag2 = 15,
        TTbarEventSelectionSteps_AllSelectedEvents = 16
    };
    virtual void AddSelectionStepsEvent();
    virtual void AddSelectionPlotsEvent();

    virtual void AddTriggerHistos();

    virtual void End(TDirectory* dir);

    virtual void OnChangeFile(FileInfo* fi, DataSet* ds);
    virtual void AddSelectionStepsJet();
private:
};

typedef TTbarEventSelector EventSelector;
typedef TTbarSelectorConfig SelectorConfig;
#endif	/* SSSusyEVENTSELECTOR_H */
