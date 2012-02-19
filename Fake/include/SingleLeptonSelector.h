/* 
 * File:   SingleLeptonSelector.h
 * Author: hbakhshi
 *
 * Created on January 28, 2011, 7:39 PM
 */

#ifndef SINGLELEPTONSELECTOR_H
#define	SINGLELEPTONSELECTOR_H

#include "DataFormats/interface/TreeReaderEvent.h"
#include "base/BaseEventSelector.h"


using namespace std;

class SingleLeptonConfig : public BaseSelectorConfig {
public:
    edm::VParameterSet ElectronCategories;
    edm::VParameterSet MuonCategories;

    bool IsElectron;
    bool IsMuon;

    bool rejectCategoriesWithJustOneLepton;

    bool ArrangeJets;
    SingleLeptonConfig(const edm::ParameterSet& ps) : BaseSelectorConfig(ps),
    rejectCategoriesWithJustOneLepton(ps.getParameter<bool>("RejectCategoriesWithJustOneLepton")) {

        if (IsElectron = ps.exists("ElectronCategories"))
            ElectronCategories = ps.getParameter<edm::VParameterSet > ("ElectronCategories");

        if (IsMuon = ps.exists("MuonCategories"))
            MuonCategories = ps.getParameter<edm::VParameterSet > ("MuonCategories");
    };
};

class SingleLeptonSelector : public BaseEventSelector<TreeReaderEvent> {
public:
    typedef TreeReaderEvent EVENTTYPE;
    typedef BaseEventSelector<EVENTTYPE> BASE;
    SingleLeptonSelector(TreeReaderEvent* tree, SingleLeptonConfig* cfg, map<string, bool>* triggerResults);

    virtual bool CheckMuon(int muidx);
    virtual void AddSelectionStepsElectron();
    virtual void AddSelectionStepsMuon();
    virtual void AddSelectionStepsEvent();
    virtual void AddSelectionPlotsEvent();

    virtual void AddTriggerHistos();
    EVENTTYPE* Read(int& stat);

    virtual void End(TDirectory* dir);
private:
    SingleLeptonSelector::EVENTTYPE* LastRead;

    bool CategorizeElectrons(map< string, vector<int> >& CategorizedMuons, int& stat_value);
    bool CategorizeMuons(map< string, vector<int> >& CategorizedElectrons, int& stat_value);
};

typedef SingleLeptonSelector EventSelector;
typedef SingleLeptonConfig SelectorConfig;
#endif	/* SINGLELEPTONSELECTOR_H */

