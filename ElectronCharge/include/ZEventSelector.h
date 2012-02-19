/* 
 * File:   ZEventSelector.h
 * Author: hbakhshi
 *
 * Created on September 13, 2010, 6:58 PM
 */

#ifndef ZEventSelector_H
#define	ZEventSelector_H


#include "base/TreeReader.hh"
#include "DataFormats/interface/DiElectronEvent.h"
#include "base/BaseEventSelector.h"


using namespace std;
using namespace ElectronAnalysis;

class ZSelectorConfig : public BaseSelectorConfig {
public:

    struct EventSelection {
    public:
        ElectronAnalysis::Electron::Objects InvMassMethod;
        double InvMassMax;
        double InvMassMin;

        bool MatchWithGen;
        bool JetVeto;
        bool VetoMoreThan2Eles;
        
        double MET;
        double MT;

        void Set(const edm::ParameterSet & ps) {
            InvMassMethod = (ElectronAnalysis::Electron::Objects) (ps.getParameter<int>("InvMassMethod"));
            InvMassMin = ps.getParameter<double>("InvMassMin");
            InvMassMax = ps.getParameter<double>("InvMassMax");

            MET = ps.getParameter<double>("MET");
            MT = ps.getParameter<double>("MT");

            MatchWithGen = ps.getParameter<bool>("ElectronsShouldMatch");
            JetVeto = ps.getParameter<bool>("JetVeto");
            VetoMoreThan2Eles = ps.getParameter<bool>("VetoMoreThan2Elecs");
        }

        void SetInvMassCuts(ElectronAnalysis::Electron::Objects invMassObj, double invMassMin, double invMassMax) {
            this->InvMassMethod = invMassObj;
            this->InvMassMin = invMassMin;
            this->InvMassMax = invMassMax;
        };

        void SetEventSelection(bool checkMC, bool jetveto, bool VetoMoreThan2Elecs) {
            this->MatchWithGen = checkMC;
            this->JetVeto = jetveto;
            this->VetoMoreThan2Eles = VetoMoreThan2Elecs;
        };

        void Print() const {
            cout << "EventSelection: " << endl;
            cout << "\tElectronAnalysis::Electron::Objects : " << InvMassMethod << endl;
            cout << "\tInvMassMax : " << InvMassMax << endl;
            cout << "\tInvMassMin : " << InvMassMin << endl;

            cout << "\tMatchWithGen : " << MatchWithGen << endl;
            cout << "\tJetVeto : " << JetVeto << endl;
            cout << "\tVetoMoreThan2Eles : " << VetoMoreThan2Eles << endl;
            cout << "------------------------------------" << endl;

        }
    };

    EventSelection EventCuts;

    ZSelectorConfig(const edm::ParameterSet& ps) : BaseSelectorConfig(ps) {
        EventCuts.Set(ps.getParameter<edm::ParameterSet > ("EventSelection"));
    };
};

class ZEventSelector : public BaseEventSelector<DiElectronEvent> {
public:
    typedef DiElectronEvent EVENTTYPE;
    typedef BaseEventSelector<EVENTTYPE> BASE;
    ZEventSelector(TreeReader* tree, ZSelectorConfig* cfg, map<string, bool>* triggerResults);

    virtual void AddSelectionStepsElectron();
    virtual void AddSelectionStepsMuon();
    virtual void AddSelectionStepsEvent();
    virtual void AddSelectionPlotsEvent();

    virtual void AddTriggerHistos();
    DiElectronEvent* Read(int& stat);
private:
    bool CheckEvent(int& status);
};
typedef ZEventSelector EventSelector;
typedef ZSelectorConfig SelectorConfig;
#endif	/* ZEventSelector_H */
