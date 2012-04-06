#ifndef TTbarEVENTSELECTOR_H
#define	TTbarEVENTSELECTOR_H

#include "base/TreeReader.hh"
#include "DataFormats/interface/DiLeptonTTBarEvent.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "base/BaseEventSelector.h"
#include <map>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "WPolarization/interface/BTagWeight.h"

using namespace std;
using namespace ElectronAnalysis;
using namespace TopAnalysis;
using ElectronAnalysis::Muon;
using ElectronAnalysis::Electron;

class TTbarSelectorConfig : public BaseSelectorConfig {
public:
    string PUWFiles;

    bool RejectInvMassLessThan12;
    bool RejectZRegion;
    bool RejectLeptonsFromZ;
    double METCutSF;
    double METCutOF;
    int NJets;

    double DRJetsLeptons;

    bool Data;
    bool IsTTBarSample;
    bool MCStudy;

    string btag_algo;

    int getBTagAlgo() const{
        if (btag_algo == "TrackCountingHighEff")
            return 1;

        if (btag_algo == "simpleSecondaryVertexHighEff")
            return 2;

        return 100;
    }

    TF1* BTagScaleFactor;

    const TF1* getBTagScaleFactor(){
        if (BTagScaleFactor == NULL) {
            if (getBTagAlgo() == 1)
                BTagScaleFactor = new TF1("fSFB", "0.603913*((1.+(0.286361*x))/(1.+(0.170474*x)))" , 30 , 1000);
            else if (getBTagAlgo() == 2)
                BTagScaleFactor = new TF1("fSFB", "0.896462*((1.+(0.00957275*x))/(1.+(0.00837582*x)))" , 30 , 1000);
            }
        return BTagScaleFactor;
    }
    double BJetSelectionBTag;
    int NBJets;
    double BTag1, BTag2;

    vector<string> DiElectronTrigger;
    vector<string> DiElectronTrigger_Veto;
    vector<string> DiMuonTrigger;
    vector<string> DiMuonTrigger_Veto;
    vector<string> ElectronMuonTrigger;
    vector<string> ElectronMuonTrigger_Veto;

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
    NBJets(ps.getParameter<int>("NBJets")),
    BJetSelectionBTag(ps.getParameter<double>("BJetSelectionBTag")),
    IsTTBarSample(ps.getUntrackedParameter<bool>("TTBarSample", false)),
    MCStudy(ps.getUntrackedParameter<bool>("MCStudy", false)),

    DiElectronTrigger(ps.getParameter<vector<string> >("DiElectronTrigger")),
    DiElectronTrigger_Veto(ps.getParameter<vector<string> >("DiElectronTrigger_Veto")),
    DiMuonTrigger(ps.getParameter<vector<string> >("DiMuonTrigger")),
    DiMuonTrigger_Veto(ps.getParameter<vector<string> >("DiMuonTrigger_Veto")),
    ElectronMuonTrigger(ps.getParameter<vector<string> >("ElectronMuonTrigger")),
    ElectronMuonTrigger_Veto(ps.getParameter<vector<string> >("ElectronMuonTrigger_Veto")),

    PUWFiles(ps.getParameter<string>("PUWFiles")),
    Data(ps.getParameter<bool>("Data")),
    BTagScaleFactor(NULL) {

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

    TH1F hPileUpWeights;
    BTagWeight BTagWeighter;
    
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
        TTbarEventSelectionSteps_SameFlavours_NumberOfBJets = 11,
        TTbarEventSelectionSteps_SameFlavours_Triggers = 12,
        TTbarEventSelectionSteps_OppositeFlavours = 13,
        TTbarEventSelectionSteps_OppositeFlavours_NJets = 14,
        TTbarEventSelectionSteps_OppositeFlavours_MET = 15,
        TTbarEventSelectionSteps_OppositeFlavours_bTag1 = 16,
        TTbarEventSelectionSteps_OppositeFlavours_bTag2 = 17,
        TTbarEventSelectionSteps_OppositeFlavours_NumberOfBJets = 18,
        TTbarEventSelectionSteps_OppositeFlavours_Triggers = 19,
        TTbarEventSelectionSteps_AllSelectedEvents = 20
    };
    virtual void AddSelectionStepsEvent();
    virtual void AddSelectionPlotsEvent();

    virtual void AddTriggerHistos();

    virtual void End(TDirectory* dir);

    virtual void OnChangeFile(FileInfo* fi, DataSet* ds);
    virtual void AddSelectionStepsJet();
private:
    
    TH1D hEventSelectionMuonMuon;
    TH1D hEventSelectionElectronElectron;
    TH1D hEventSelectionElectronMuon;
    TH1D hEventSelectionMuonMuonW;
    TH1D hEventSelectionElectronElectronW;
    TH1D hEventSelectionElectronMuonW;
};

typedef TTbarEventSelector EventSelector;
typedef TTbarSelectorConfig SelectorConfig;
#endif	/* SSSusyEVENTSELECTOR_H */
