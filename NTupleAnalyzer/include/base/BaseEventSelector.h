/* 
 * File:   BaseEventSelector.h
 * Author: hbakhshi
 *
 * Created on November 9, 2010, 8:39 PM
 */

#ifndef BASEEVENTSELECTOR_H
#define	BASEEVENTSELECTOR_H

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/PythonProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "base/TreeReader.hh"
#include "DataFormats/interface/DiElectronEvent.h"
#include "DataFormats/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "BaseAnalysis/interface/HistogramsManager.h"
#include "base/Configuration.h"

#include <algorithm>
#include <deque>

using namespace ElectronAnalysis;
using ElectronAnalysis::Muon;

class BaseSelectorConfig {
public:

    class MuonSelection {
    public:
        bool isSet;

        bool GlobalMuon;
        bool TrackerMuon;
        double PtCut;
        double Eta;
        int NTrkHits;
        int NMuHits;
        double NChi2;
        double EEm;
        double EHad;

        double isoCut;
        double d0Cut;

        MuonSelection() : isSet(false) {
        };

        void Set(const edm::ParameterSet& ps) {
            GlobalMuon = ps.getParameter<bool>("GlobalMuon");
            TrackerMuon = ps.getParameter<bool>("TrackerMuon");

            PtCut = ps.getParameter<double>("PtCut");
            Eta = ps.getParameter<double>("Eta");
            NChi2 = ps.getParameter<double>("NChi2");
            EEm = ps.getParameter<double>("EEm");
            EHad = ps.getParameter<double>("EHad");

            isoCut = ps.getParameter<double>("IsoCut");
            d0Cut = ps.getParameter<double>("d0Cut");

            NTrkHits = ps.getParameter<int>("NTrkHits");
            NMuHits = ps.getParameter<int>("NMuHits");

            isSet = true;
        };

    };

    class ElectronSelection {
    public:
        bool isSet;

        enum ElectronID {
            idNone = -1,
            idWP95 = 0,
            idWP90 = 1,
            idWP85 = 2,
            idWP80 = 3,
            idWP70 = 4,
            idWP60 = 5,
            idRobustTight = 6,
            idRobustLoose = 7,
            idTight = 8,
            idLoose = 9
        };

        enum ElectronIso {
            isoNone = -2,
            isoComb03 = 21,
            isoComb04 = 22,
            isoTrk03 = 23,
            isoTrk04 = 24,
            isoECal03 = 25,
            isoECal04 = 26,
            isoHCal03 = 27,
            isoHCal04 = 28,
            isoAllIndividuals = 29, //in this case, it uses the cut values from the selected WP for ID and accept the electrons which pass all of the individual isolation cuts
            isoHybrid = 30
        };

        enum ElectronConversion {
            convGeoCutsOnly = 41,
            convInnerHitsOnly = 42,
            convAllCuts = 43,
            convUseWPInfo = 44,
            convNone = 45,
            convORCutsHits2 = 46
        };

        enum ElectronD0 {
            D0PV = 51,
            D0PVSign = 52,
            D0BS = 53,
            D0BSSign = 54,
            D0None = 55
        };

        ElectronConversion conv;
        ElectronD0 d0;
        double d0Cut;
        ElectronID id;
        ElectronIso iso;
        double isoCut;
        double IsoCutBarrel;
        double IsoCutEndCap;
        double isoUseWPInfo;
        bool isoRelative;
        bool RejectCrack;

        bool RejectTrackerDrivensOnly;
        double ptCut;

        bool CTFGSFChargesAgree;
        bool allChargesAgree;
        double deltaRNearestMuon;
        bool specialMuonSelection;
        MuonSelection MuonSelection__;
        double CaloEnergy;

        void SetElectronProperties(bool rejectTrackerDrivens, double ptCut) {
            this->RejectTrackerDrivensOnly = rejectTrackerDrivens;
            this->ptCut = ptCut;
        }

        void SetIDInfo(ElectronID id) {
            this->id = id;
        };

        void SetIsoInfo(ElectronIso iso, double cutVal, bool relative) {
            this->iso = iso;
            this->isoRelative = relative;
            this->isoCut = cutVal;

            this->isoUseWPInfo = cutVal < 0;
            if (this->isoUseWPInfo)
                this->isoRelative = true;
        };

        void SetConvInfo(ElectronConversion conv) {
            this->conv = conv;
        };

        void SetD0Info(ElectronD0 d0, double cut) {
            this->d0 = d0;
            this->d0Cut = cut;
        };

        ElectronSelection() : isSet(false) {
            specialMuonSelection = false;
        };

        void Set(const edm::ParameterSet& ps) {
            id = (ElectronID) (ps.getParameter<int>("ID"));

            iso = (ElectronIso) (ps.getParameter<int>("IsoMethod"));
            isoCut = ps.getParameter<double>("IsoCutValue");

            IsoCutBarrel = ps.getUntrackedParameter<double>("EleIsoBarrel", -1.0);
            IsoCutEndCap = ps.getUntrackedParameter<double>("EleIsoEndCap", -1.0);

            isoUseWPInfo = (isoCut < 0);
            isoRelative = ps.getParameter<bool>("IsoRelative");
            if (isoUseWPInfo)
                isoRelative = true;

            conv = (ElectronConversion) (ps.getParameter<int>("ConvRejMethod"));

            d0 = (ElectronD0) (ps.getParameter<int>("D0Method"));
            d0Cut = ps.getParameter<double>("D0Value");

            RejectTrackerDrivensOnly = ps.getParameter<bool>("RejectTrackerDrivensOnly");
            ptCut = ps.getParameter<double>("PtCut");

            deltaRNearestMuon = ps.getUntrackedParameter<double>("deltaRNearestMuon", -1.0);
            if (specialMuonSelection = ps.exists("MuonSelection"))
                MuonSelection__.Set(ps.getParameterSet("MuonSelection"));

            CaloEnergy = ps.getUntrackedParameter<double>("CaloEnergy", 0.0);
            allChargesAgree = ps.getUntrackedParameter<bool>("allChargesAgree", false);
            CTFGSFChargesAgree = ps.getUntrackedParameter<bool>("CTFGSFChargesAgree", false);

            RejectCrack = ps.getParameter<bool>("RejectCrack");
            isSet = true;
        };

        void Print() const {
            cout << "ElectronSelection:" << endl;
            cout << "\tNTuple2Event::ElectronConversion : " << conv << endl;
            cout << "\tNTuple2Event::ElectronD0 : " << d0 << endl;
            cout << "\td0Cut : " << d0Cut << endl;
            cout << "\tNTuple2Event::ElectronID : " << id << endl;
            cout << "\tNTuple2Event::ElectronIso : " << iso << endl;
            cout << "\tisoCut : " << isoCut << endl;
            cout << "\tisoRelative : " << isoRelative << endl;

            cout << "\tRejectTrackerDrivensOnly : " << RejectTrackerDrivensOnly << endl;
            cout << "\tptCut : " << ptCut << endl;
            cout << "------------------------------------------------" << endl;
        };
    };

    class JetSelection {
    public:
        bool isSet;

        double PtCut;
        double Eta;
        double DrElectron;
        bool specialElectronSelection;
        ElectronSelection ElectronSelection__;
        double DrMuon;
        bool specialMuonSelection;
        MuonSelection MuonSelection__;

        bool checkID;
        bool RejectCrack;
        bool RejectElectronsInGapAfterJetSelection;

        JetSelection() : isSet(false) {

        }

        void Set(const edm::ParameterSet& ps) {
            checkID = ps.getParameter<bool>("checkID");
            RejectCrack = ps.getParameter<bool>("RejectCrack");
            RejectElectronsInGapAfterJetSelection = ps.getParameter<bool>("RejectElectronsInGapAfterJetSelection");

            PtCut = ps.getParameter<double>("PtCut");
            Eta = ps.getParameter<double>("Eta");

            DrElectron = ps.getParameter<double>("DrElectron");
            if (specialElectronSelection = ps.exists("ElectronSelection"))
                ElectronSelection__.Set(ps.getParameterSet("ElectronSelection"));

            DrMuon = ps.getParameter<double>("DrMuon");
            if (specialMuonSelection = ps.exists("MuonSelection"))
                MuonSelection__.Set(ps.getParameterSet("MuonSelection"));

            isSet = true;
        }
    };

    ElectronSelection ElectronCuts;
    MuonSelection MuonCuts;
    JetSelection JetCuts;

    vector<string> TriggerNames;

    multimap<int, int> RunEventIDS;

    bool CheckRunEventID(int run, int event) {
        if (RunEventIDS.size() == 0)
            return true;

        pair<multimap<int, int>::iterator, multimap<int, int>::iterator> range_runevent;
        range_runevent = RunEventIDS.equal_range(run);
        multimap<int, int>::iterator it_runevent = std::find(range_runevent.first, range_runevent.second, pair<const int, int>(run, event));
        if (it_runevent != range_runevent.second) {
            return true;
        } else
            return false;
    };

    bool LooseSelection;

    bool EnableElectronControlPlots;
    bool EnableMuonControlPlots;
    bool EnableJetControlPlots;
    bool EnableEventControlPlots;

    BaseSelectorConfig() {
        cout << "A BaseSelectorConfig With no parameter input is created" << endl;
    };

    BaseSelectorConfig(const edm::ParameterSet& ps) : LooseSelection(ps.getParameter<bool>("LooseSelection")),
    EnableElectronControlPlots(ps.getParameter<bool>("EnableElectronControlPlots")),
    EnableMuonControlPlots(ps.getParameter<bool>("EnableMuonControlPlots")),
    EnableJetControlPlots(ps.getParameter<bool>("EnableJetControlPlots")),
    EnableEventControlPlots(ps.getParameter<bool>("EnableEventControlPlots")),
    TriggerNames(ps.getParameter<vector<string> >("TriggerNames")) {
        if (ps.exists("ElectronSelection"))
            ElectronCuts.Set(ps.getParameter<edm::ParameterSet > ("ElectronSelection"));
        if (ps.exists("MuonSelection"))
            MuonCuts.Set(ps.getParameter<edm::ParameterSet > ("MuonSelection"));
        if (ps.exists("JetSelection"))
            JetCuts.Set(ps.getParameter<edm::ParameterSet > ("JetSelection"));
        if (ps.exists("EventsRunID")) {
            edm::VParameterSet ssss = (ps.getParameter< edm::VParameterSet > ("EventsRunID"));

            for (edm::VParameterSet::const_iterator itr = ssss.begin(); itr != ssss.end(); itr++) {
                int run = itr->getParameter<int>("Run");
                std::vector<int> events = itr->getParameter< std::vector<int> >("Events");

                for (std::vector<int>::const_iterator iii = events.begin(); iii != events.end(); iii++)
                    RunEventIDS.insert(std::pair<int, int>(run, *iii));
            }
        }
    };
};


#include <sstream>

class SelectionException : public exception {
public:

    string __WHAT;
    int lineNumber;
    string fileName;

    SelectionException(string WHAT, int line = __LINE__, string file = __FILE__) : __WHAT(WHAT), lineNumber(line), fileName(file) {
    }

    virtual ~SelectionException() throw () {
    };

    virtual const char* what() const throw () {
        std::stringstream ss;
        ss << __WHAT << "\nLine : " << lineNumber << "\nFile : " << fileName;
        return ss.str().c_str();
    }
};

template<class RetType>
class BaseEventSelector {
public:
    BaseEventSelector(TreeReader* tree, BaseSelectorConfig* cfg, map<string, bool>* triggerResults);

    virtual RetType* Read(int& stat) = 0;

    Electron* MakeElectron(int eleidx, ElectronAnalysis::BaseElectron*);
    ElectronAnalysis::Muon* MakeMuon(int muonidx, ElectronAnalysis::Muon*);

    RetType TheEvent;
    TreeReader *TheTree;
    BaseSelectorConfig* Config;
    map<string, bool>* TriggerResults;

    ~BaseEventSelector() {
        if (info::TheInfo->Verbosity > 100)
            cout << "EventSelector Ended" << endl;
    }

    virtual void SetTree(TreeReader* tree) {
        TheTree = tree;

        AddSelectionStepsElectron();
        this->AddSelectionPlotsElectron();

        AddSelectionStepsMuon();
        this->AddSelectionPlotsMuon();

        this->AddSelectionStepsJet();
        this->AddSelectionPlotsJet();

        this->AddSelectionStepsEvent();
        this->AddSelectionPlotsEvent();
    }
    double Weight;
    bool IsZee;

    void SetEventProperties(double weight, bool isZEE) {
        Weight = weight;
        IsZee = isZEE;
    }

    virtual void End(TDirectory* dir) {
        TDirectory* newDir = dir->mkdir("Selection");
        EleSelectionHistos.WriteAll(newDir);
        MuonSelectionHistos.WriteAll(newDir);
        JetSelectionHistos.WriteAll(newDir);
        EventSelectionHistos.WriteAll(newDir);
        EventSelectionHistosAfterObjectCreation.WriteAll(newDir);
    }

    virtual void AddTriggerHistos() {
    };

    virtual void OnChangeFile(FileInfo* fi, DataSet* ds) {
    };
protected:
    HistogramsManager<int> EleSelectionHistos;
    HistogramsManager<int> MuonSelectionHistos;
    HistogramsManager<int> JetSelectionHistos;
    HistogramsManager<int> EventSelectionHistos;
    HistogramsManager<RetType> EventSelectionHistosAfterObjectCreation;
    virtual void AddSelectionStepsElectron();
    virtual void AddSelectionPlotsElectron();
    virtual void AddSelectionStepsMuon();
    virtual void AddSelectionPlotsMuon();
    virtual void AddSelectionStepsJet();
    virtual void AddSelectionPlotsJet();
    virtual void AddSelectionStepsEvent();
    virtual void AddSelectionPlotsEvent();


    bool CheckEleID(int eleidx);
    bool CheckEleD0(int eleidx);
    bool CheckEleIso(int eleidx);
    bool CheckEleConv(int eleidx);
    virtual bool CheckElectron(int eleidx, bool fillHisto = true);

    Double_t BarrelMaxEta_D;
    Double_t EndCapMinEta_D;
    Double_t EndCapMaxEta_D;

    bool IsGoodJet(int index);

    template<class LIST>
    double GetMinDistance(float eta, float phi, float etas[], float phis[], LIST goodObjsToLoopOver, int& idOfNearest) {
        idOfNearest = -1;
        double ret = -10000.0;
        for (int i = 0; i < goodObjsToLoopOver.size(); i++) {
            double dr_i = reco::deltaR(eta, phi, etas[goodObjsToLoopOver.at(i)], phis[goodObjsToLoopOver.at(i)]);
            if (ret == -10000.0 || dr_i < ret) {
                ret = dr_i;
                idOfNearest = goodObjsToLoopOver.at(i);
            }
        }

        return ret;
    }

    template<class LIST>
    double GetMinDistance(double eta, double phi, double etas[], double phis[], LIST goodObjsToLoopOver, int& idOfNearest) {
        idOfNearest = -1;
        double ret = -10000.0;
        for (int i = 0; i < goodObjsToLoopOver.size(); i++) {
            double dr_i = reco::deltaR(eta, phi, etas[goodObjsToLoopOver.at(i)], phis[goodObjsToLoopOver.at(i)]);
            if (ret == -10000.0 || dr_i < ret) {
                ret = dr_i;
                idOfNearest = goodObjsToLoopOver.at(i);
            }
        }

        return ret;
    };

    bool InBarrel(double Eta) {
        return (fabs(Eta) <= BarrelMaxEta_D);
    }

    bool InEndcap(double Eta) {
        return (fabs(Eta) >= EndCapMinEta_D && fabs(Eta) <= EndCapMaxEta_D);
    }

    virtual bool CheckMuon(int muidx, bool fillhisto = true);

    virtual bool CheckJet(int jetIdx);
    std::deque<int> GoodElecs;
    std::deque<int> GoodMuons_ForElectronSelection;

    struct Sorter {
        double Pts[100];

        bool operator() (int i, int j) {
            return Pts[i] > Pts[j];
        }
    } TheSorter;

    uint FillGoodElecs() {
        GoodMuons_ForElectronSelection.clear();
        if (Config->ElectronCuts.specialMuonSelection) {
            BaseSelectorConfig::MuonSelection DefaultSelection(this->Config->MuonCuts);

            this->Config->MuonCuts = this->Config->ElectronCuts.MuonSelection__;
            for (int muid = 0; muid < TheTree->NMus; muid++) {
                TheSorter.Pts[muid] = TheTree->MuPt[muid];
                if (this->CheckMuon(muid, false))
                    GoodMuons_ForElectronSelection.push_back(muid);
            }
            sort(GoodMuons_ForElectronSelection.begin(), GoodMuons_ForElectronSelection.end(), TheSorter);

            this->Config->MuonCuts = DefaultSelection;
        } else
            GoodMuons_ForElectronSelection.assign(GoodMuons.begin(), GoodMuons.end());

        this->GoodElecs.clear();
        if (this->GoodElecs.size() != 0)
            throw new SelectionException("Number of good electrons after cleaning is more than zero");

        for (int eleid = 0; eleid < TheTree->NEles; eleid++) {
            TheSorter.Pts[eleid] = TheTree->ElPt[eleid];
            if (this->CheckElectron(eleid))
                GoodElecs.push_back(eleid);
        }
        sort(GoodElecs.begin(), GoodElecs.end(), TheSorter);
        return GoodElecs.size();
    };

    std::deque<int> GoodMuons;

    uint FillGoodMuons() {
        this->GoodMuons.clear();
        if (this->GoodMuons.size() != 0)
            throw new SelectionException("Number of good muons after cleaning is more than zero");


        for (int muid = 0; muid < TheTree->NMus; muid++) {
            TheSorter.Pts[muid] = TheTree->MuPt[muid];
            if (this->CheckMuon(muid))
                GoodMuons.push_back(muid);
        }
        sort(GoodMuons.begin(), GoodMuons.end(), TheSorter);

        return GoodMuons.size();
    };
    std::deque<int> GoodJets;
    std::deque<int> GoodMuons_ForJetSelection;
    std::deque<int> GoodElectrons_ForJetSelection;

    uint FillGoodJets() {
        GoodMuons_ForJetSelection.clear();
        if (Config->JetCuts.specialMuonSelection) {
            BaseSelectorConfig::MuonSelection DefaultSelection(this->Config->MuonCuts);

            this->Config->MuonCuts = this->Config->JetCuts.MuonSelection__;
            for (int muid = 0; muid < TheTree->NMus; muid++) {
                TheSorter.Pts[muid] = TheTree->MuPt[muid];
                if (this->CheckMuon(muid, false))
                    GoodMuons_ForJetSelection.push_back(muid);
            }
            sort(GoodMuons_ForJetSelection.begin(), GoodMuons_ForJetSelection.end(), TheSorter);

            this->Config->MuonCuts = DefaultSelection;
        } else
            GoodMuons_ForJetSelection.assign(GoodMuons.begin(), GoodMuons.end());

        GoodElectrons_ForJetSelection.clear();
        if (Config->JetCuts.specialElectronSelection) {
            BaseSelectorConfig::ElectronSelection DefaultSelection(this->Config->ElectronCuts);

            this->Config->ElectronCuts = this->Config->JetCuts.ElectronSelection__;

            for (int eleid = 0; eleid < TheTree->NEles; eleid++) {
                TheSorter.Pts[eleid] = TheTree->ElPt[eleid];
                if (this->CheckElectron(eleid, false))
                    GoodElectrons_ForJetSelection.push_back(eleid);
            }
            sort(GoodElectrons_ForJetSelection.begin(), GoodElectrons_ForJetSelection.end(), TheSorter);

            this->Config->ElectronCuts = DefaultSelection;
        } else
            GoodElectrons_ForJetSelection.assign(GoodElecs.begin(), GoodElecs.end());


        this->GoodJets.clear();
        if (this->GoodJets.size() != 0)
            throw new SelectionException("Number of good jets after cleaning is more than zero");

        for (int jid = 0; jid < TheTree->NJets; jid++) {
            TheSorter.Pts[jid] = TheTree->JPt[jid];
            if (this->CheckJet(jid))
                this->GoodJets.push_back(jid);
        }
        sort(GoodJets.begin(), GoodJets.end(), TheSorter);

        if (Config->JetCuts.RejectElectronsInGapAfterJetSelection)
            for (std::deque<int>::iterator ele = GoodElecs.begin(); ele != GoodElecs.end();) {
                double eta = TheTree->ElEta[*ele];
                bool inBarrel = InBarrel(eta);
                bool inEndcap = InEndcap(eta);

                bool inGap = !(inBarrel || inEndcap);
                if (inGap) {
                    ele = GoodElecs.erase(ele);

                    if (GoodElecs.size() == 0)
                        break;
                } else
                    ele++;
            }

        return GoodJets.size();
    };

        void FillAllValue(Histograms<int>* histos) {
        if (!(this->Config->EnableElectronControlPlots))
            return;

        for (uint i = 0; i < histos->GetSizeH1(); i++) {
            TH1Ext<int>* hhh = histos->GetHistogram1(i);
            int maxVal = -1;
            const ObjectPropertyFromIntArray* objInt = dynamic_cast<const ObjectPropertyFromIntArray*> (hhh->theProperty);
            const ObjectPropertyFromFltArray* objDbl = dynamic_cast<const ObjectPropertyFromFltArray*> (hhh->theProperty);
            if (objInt != 0) {
                if (objInt->MaxPossibleIndex != NULL)
                    maxVal = *(objInt->MaxPossibleIndex);
            } else if (objDbl != 0) {
                if (objDbl->MaxPossibleIndex != NULL)
                    maxVal = *(objDbl->MaxPossibleIndex);
            }

            if (maxVal == -1) {
                int objId = 0;
                hhh->Fill(&objId);
            } else {
                for (int objId = 0; objId < maxVal; objId++)
                    hhh->Fill(&objId);
            }
        }
    }

};

template<class T>
Electron* BaseEventSelector<T>::MakeElectron(int eleidx, ElectronAnalysis::BaseElectron* TheObj) {
    if (TheObj == NULL)
        TheObj = new ElectronAnalysis::Electron();

    TheObj->index_in_tree = eleidx;

    TheObj->ElGood = TheTree->ElGood[eleidx];
    TheObj->ElIsIso = TheTree->ElIsIso[eleidx]; //[NEles]
    TheObj->ElChargeMisIDProb = TheTree->ElChargeMisIDProb[eleidx]; //[NEles]
    TheObj->Px = TheTree->ElPx[eleidx]; //[NEles]
    TheObj->Py = TheTree->ElPy[eleidx]; //[NEles]
    TheObj->Pz = TheTree->ElPz[eleidx]; //[NEles]
    TheObj->Pt = TheTree->ElPt[eleidx]; //[NEles]
    TheObj->ElPtE = TheTree->ElPtE[eleidx]; //[NEles]
    TheObj->E = TheTree->ElE[eleidx]; //[NEles]
    TheObj->ElEt = TheTree->ElEt[eleidx]; //[NEles]
    TheObj->Eta = TheTree->ElEta[eleidx]; //[NEles]
    TheObj->Theta = TheTree->ElTheta[eleidx]; //[NEles]
    TheObj->ElSCEta = TheTree->ElSCEta[eleidx]; //[NEles]
    TheObj->Phi = TheTree->ElPhi[eleidx]; //[NEles]
    TheObj->ElGsfTkPt = TheTree->ElGsfTkPt[eleidx]; //[NEles]
    TheObj->ElGsfTkEta = TheTree->ElGsfTkEta[eleidx]; //[NEles]
    TheObj->ElGsfTkPhi = TheTree->ElGsfTkPhi[eleidx]; //[NEles]
    TheObj->ElTrkMomentumError = TheTree->ElTrkMomentumError[eleidx]; //[NEles]
    TheObj->ElEcalEnergyError = TheTree->ElEcalEnergyError[eleidx]; //[NEles]
    TheObj->ElEleMomentumError = TheTree->ElEleMomentumError[eleidx]; //[NEles]
    TheObj->ElNBrems = TheTree->ElNBrems[eleidx]; //[NEles]
    TheObj->D0BS = TheTree->ElD0BS[eleidx]; //[NEles]
    TheObj->D0PV = TheTree->ElD0PV[eleidx]; //[NEles]
    TheObj->D0E = TheTree->ElD0E[eleidx]; //[NEles]
    TheObj->DzBS = TheTree->ElDzBS[eleidx]; //[NEles]
    TheObj->DzPV = TheTree->ElDzPV[eleidx]; //[NEles]
    TheObj->DzE = TheTree->ElDzE[eleidx]; //[NEles]
    TheObj->ElRelIso03 = TheTree->ElRelIso03[eleidx]; //[NEles]
    TheObj->ElRelIso04 = TheTree->ElRelIso04[eleidx]; //[NEles]
    TheObj->ElDR03TkSumPt = TheTree->ElDR03TkSumPt[eleidx]; //[NEles]
    TheObj->ElDR04TkSumPt = TheTree->ElDR04TkSumPt[eleidx]; //[NEles]
    TheObj->ElDR03EcalRecHitSumEt = TheTree->ElDR03EcalRecHitSumEt[eleidx]; //[NEles]
    TheObj->ElDR04EcalRecHitSumEt = TheTree->ElDR04EcalRecHitSumEt[eleidx]; //[NEles]
    TheObj->ElDR03HcalTowerSumEt = TheTree->ElDR03HcalTowerSumEt[eleidx]; //[NEles]
    TheObj->ElDR04HcalTowerSumEt = TheTree->ElDR04HcalTowerSumEt[eleidx]; //[NEles]
    TheObj->ElNChi2 = TheTree->ElNChi2[eleidx]; //[NEles]
    TheObj->Charge = TheTree->ElCharge[eleidx]; //[NEles]
    TheObj->ElCInfoIsGsfCtfCons = TheTree->ElCInfoIsGsfCtfCons[eleidx]; //[NEles]
    TheObj->ElCInfoIsGsfCtfScPixCons = TheTree->ElCInfoIsGsfCtfScPixCons[eleidx]; //[NEles]
    TheObj->ElCInfoIsGsfScPixCons = TheTree->ElCInfoIsGsfScPixCons[eleidx]; //[NEles]
    TheObj->ElScPixCharge = TheTree->ElScPixCharge[eleidx]; //[NEles]
    TheObj->ElClosestCtfTrackPt = TheTree->ElClosestCtfTrackPt[eleidx]; //[NEles]
    TheObj->ElClosestCtfTrackEta = TheTree->ElClosestCtfTrackEta[eleidx]; //[NEles]
    TheObj->ElClosestCtfTrackPhi = TheTree->ElClosestCtfTrackPhi[eleidx]; //[NEles]
    TheObj->ElClosestCtfTrackCharge = TheTree->ElClosestCtfTrackCharge[eleidx]; //[NEles]
    TheObj->ElIDMva = TheTree->ElIDMva[eleidx]; //[NEles]
    TheObj->ElIDTight = TheTree->ElIDTight[eleidx]; //[NEles]
    TheObj->ElIDLoose = TheTree->ElIDLoose[eleidx]; //[NEles]
    TheObj->ElIDRobustTight = TheTree->ElIDRobustTight[eleidx]; //[NEles]
    TheObj->ElIDRobustLoose = TheTree->ElIDRobustLoose[eleidx]; //[NEles]
    TheObj->ElIDsimpleWPrelIso = TheTree->ElIDsimpleWPrelIso[eleidx]; //[NEles]
    TheObj->ElIDsimpleWP80relIso = TheTree->ElIDsimpleWP80relIso[eleidx]; //[NEles]
    TheObj->ElIDsimpleWP85relIso = TheTree->ElIDsimpleWP85relIso[eleidx]; //[NEles]
    TheObj->ElIDsimpleWP90relIso = TheTree->ElIDsimpleWP90relIso[eleidx]; //[NEles]
    TheObj->ElIDsimpleWP95relIso = TheTree->ElIDsimpleWP95relIso[eleidx]; //[NEles]
    TheObj->ElInGap = TheTree->ElInGap[eleidx]; //[NEles]
    TheObj->ElEcalDriven = TheTree->ElEcalDriven[eleidx]; //[NEles]
    TheObj->ElTrackerDriven = TheTree->ElTrackerDriven[eleidx]; //[NEles]
    TheObj->ElBasicClustersSize = TheTree->ElBasicClustersSize[eleidx]; //[NEles]
    TheObj->Elfbrem = TheTree->Elfbrem[eleidx]; //[NEles]
    TheObj->ElHcalOverEcal = TheTree->ElHcalOverEcal[eleidx]; //[NEles]
    TheObj->ElE1x5 = TheTree->ElE1x5[eleidx]; //[NEles]
    TheObj->ElE5x5 = TheTree->ElE5x5[eleidx]; //[NEles]
    TheObj->ElE2x5Max = TheTree->ElE2x5Max[eleidx]; //[NEles]
    TheObj->ElSigmaIetaIeta = TheTree->ElSigmaIetaIeta[eleidx]; //[NEles]
    TheObj->ElDeltaPhiSeedClusterAtCalo = TheTree->ElDeltaPhiSeedClusterAtCalo[eleidx]; //[NEles]
    TheObj->ElDeltaEtaSeedClusterAtCalo = TheTree->ElDeltaEtaSeedClusterAtCalo[eleidx]; //[NEles]
    TheObj->ElDeltaPhiSuperClusterAtVtx = TheTree->ElDeltaPhiSuperClusterAtVtx[eleidx]; //[NEles]
    TheObj->ElDeltaEtaSuperClusterAtVtx = TheTree->ElDeltaEtaSuperClusterAtVtx[eleidx]; //[NEles]
    TheObj->ElCaloEnergy = TheTree->ElCaloEnergy[eleidx]; //[NEles]
    TheObj->ElTrkMomAtVtx = TheTree->ElTrkMomAtVtx[eleidx]; //[NEles]
    TheObj->ElESuperClusterOverP = TheTree->ElESuperClusterOverP[eleidx]; //[NEles]
    TheObj->ElNumberOfMissingInnerHits = TheTree->ElNumberOfMissingInnerHits[eleidx]; //[NEles]
    TheObj->ElSCindex = TheTree->ElSCindex[eleidx]; //[NEles]
    TheObj->ElConvPartnerTrkDist = TheTree->ElConvPartnerTrkDist[eleidx]; //[NEles]
    TheObj->ElConvPartnerTrkDCot = TheTree->ElConvPartnerTrkDCot[eleidx]; //[NEles]
    TheObj->ElConvPartnerTrkPt = TheTree->ElConvPartnerTrkPt[eleidx]; //[NEles]
    TheObj->ElConvPartnerTrkEta = TheTree->ElConvPartnerTrkEta[eleidx]; //[NEles]
    TheObj->ElConvPartnerTrkPhi = TheTree->ElConvPartnerTrkPhi[eleidx]; //[NEles]
    TheObj->ElConvPartnerTrkCharge = TheTree->ElConvPartnerTrkCharge[eleidx]; //[NEles]
    TheObj->ElScSeedSeverity = TheTree->ElScSeedSeverity[eleidx]; //[NEles]
    TheObj->ElE1OverE9 = TheTree->ElE1OverE9[eleidx]; //[NEles]
    TheObj->ElS4OverS1 = TheTree->ElS4OverS1[eleidx]; //[NEles]
    TheObj->GenID = TheTree->ElGenID[eleidx]; //[NEles]
    TheObj->GenStatus = TheTree->ElGenStatus[eleidx]; //[NEles]
    TheObj->GenPt = TheTree->ElGenPt[eleidx]; //[NEles]
    TheObj->GenEta = TheTree->ElGenEta[eleidx]; //[NEles]
    TheObj->GenPhi = TheTree->ElGenPhi[eleidx]; //[NEles]
    TheObj->GenE = TheTree->ElGenE[eleidx]; //[NEles]
    TheObj->GenMID = TheTree->ElGenMID[eleidx]; //[NEles]
    TheObj->GenMStatus = TheTree->ElGenMStatus[eleidx]; //[NEles]
    TheObj->GenMPt = TheTree->ElGenMPt[eleidx]; //[NEles]
    TheObj->GenMEta = TheTree->ElGenMEta[eleidx]; //[NEles]
    TheObj->GenMPhi = TheTree->ElGenMPhi[eleidx]; //[NEles]
    TheObj->GenME = TheTree->ElGenME[eleidx]; //[NEles]
    TheObj->GenGMID = TheTree->ElGenGMID[eleidx]; //[NEles]
    TheObj->GenGMStatus = TheTree->ElGenGMStatus[eleidx]; //[NEles]
    TheObj->GenGMPt = TheTree->ElGenGMPt[eleidx]; //[NEles]
    TheObj->GenGMEta = TheTree->ElGenGMEta[eleidx]; //[NEles]
    TheObj->GenGMPhi = TheTree->ElGenGMPhi[eleidx]; //[NEles]
    TheObj->GenGME = TheTree->ElGenGME[eleidx]; //[NEles]

    return (Electron*) TheObj;
}

template<class T>
ElectronAnalysis::Muon* BaseEventSelector<T>::MakeMuon(int muonidx, ElectronAnalysis::Muon* TheObj) {
    if (TheObj == NULL)
        TheObj = new ElectronAnalysis::Muon();

    TheObj->index_in_tree = muonidx;

    TheObj->Px = TheTree->MuPx[muonidx];
    TheObj->Py = TheTree->MuPy[muonidx];
    TheObj->Pz = TheTree->MuPz[muonidx];
    TheObj->Pt = TheTree->MuPt[muonidx];
    TheObj->E = TheTree->MuE[muonidx];
    TheObj->Eta = TheTree->MuEta[muonidx];
    TheObj->Phi = TheTree->MuPhi[muonidx];
    TheObj->D0BS = TheTree->MuD0BS[muonidx];
    TheObj->D0PV = TheTree->MuD0PV[muonidx];
    TheObj->D0E = TheTree->MuD0E[muonidx];
    TheObj->DzBS = TheTree->MuDzBS[muonidx];
    TheObj->DzPV = TheTree->MuDzPV[muonidx];
    TheObj->DzE = TheTree->MuDzE[muonidx];
    TheObj->Charge = TheTree->MuCharge[muonidx];

    TheObj->MuGood = TheTree->MuGood[muonidx]; //[NMus]
    TheObj->MuIsIso = TheTree->MuIsIso[muonidx]; //[NMus]
    TheObj->MuIsGlobalMuon = TheTree->MuIsGlobalMuon[muonidx]; //[NMus]
    TheObj->MuIsTrackerMuon = TheTree->MuIsTrackerMuon[muonidx]; //[NMus]
    TheObj->MuInnerTkPt = TheTree->MuInnerTkPt[muonidx]; //[NMus]
    TheObj->MuPtE = TheTree->MuPtE[muonidx]; //[NMus]
    TheObj->MuEt = TheTree->MuEt[muonidx]; //[NMus]
    TheObj->MuRelIso03 = TheTree->MuRelIso03[muonidx]; //[NMus]
    TheObj->MuIso03SumPt = TheTree->MuIso03SumPt[muonidx]; //[NMus]
    TheObj->MuIso03EmEt = TheTree->MuIso03EmEt[muonidx]; //[NMus]
    TheObj->MuIso03HadEt = TheTree->MuIso03HadEt[muonidx]; //[NMus]
    TheObj->MuIso03EMVetoEt = TheTree->MuIso03EMVetoEt[muonidx]; //[NMus]
    TheObj->MuIso03HadVetoEt = TheTree->MuIso03HadVetoEt[muonidx]; //[NMus]
    TheObj->MuIso05SumPt = TheTree->MuIso05SumPt[muonidx]; //[NMus]
    TheObj->MuIso05EmEt = TheTree->MuIso05EmEt[muonidx]; //[NMus]
    TheObj->MuIso05HadEt = TheTree->MuIso05HadEt[muonidx]; //[NMus]
    TheObj->MuEem = TheTree->MuEem[muonidx]; //[NMus]
    TheObj->MuEhad = TheTree->MuEhad[muonidx]; //[NMus]
    TheObj->MuNChi2 = TheTree->MuNChi2[muonidx]; //[NMus]
    TheObj->MuNGlHits = TheTree->MuNGlHits[muonidx]; //[NMus]
    TheObj->MuNMuHits = TheTree->MuNMuHits[muonidx]; //[NMus]
    TheObj->MuNTkHits = TheTree->MuNTkHits[muonidx]; //[NMus]
    TheObj->MuNPxHits = TheTree->MuNPxHits[muonidx]; //[NMus]
    TheObj->MuInnerTkNChi2 = TheTree->MuInnerTkNChi2[muonidx]; //[NMus]
    TheObj->MuNMatches = TheTree->MuNMatches[muonidx]; //[NMus]
    TheObj->MuNChambers = TheTree->MuNChambers[muonidx]; //[NMus]
    TheObj->MuCaloComp = TheTree->MuCaloComp[muonidx]; //[NMus]
    TheObj->MuSegmComp = TheTree->MuSegmComp[muonidx]; //[NMus]
    TheObj->MuIsGMPT = TheTree->MuIsGMPT[muonidx]; //[NMus]
    TheObj->MuIsGMTkChiComp = TheTree->MuIsGMTkChiComp[muonidx]; //[NMus]
    TheObj->MuIsGMStaChiComp = TheTree->MuIsGMStaChiComp[muonidx]; //[NMus]
    TheObj->MuIsGMTkKinkTight = TheTree->MuIsGMTkKinkTight[muonidx]; //[NMus]
    TheObj->MuIsAllStaMuons = TheTree->MuIsAllStaMuons[muonidx]; //[NMus]
    TheObj->MuIsAllTrkMuons = TheTree->MuIsAllTrkMuons[muonidx]; //[NMus]
    TheObj->MuIsTrkMuonArbitrated = TheTree->MuIsTrkMuonArbitrated[muonidx]; //[NMus]
    TheObj->MuIsAllArbitrated = TheTree->MuIsAllArbitrated[muonidx]; //[NMus]
    TheObj->MuIsTMLSLoose = TheTree->MuIsTMLSLoose[muonidx]; //[NMus]
    TheObj->MuIsTMLSTight = TheTree->MuIsTMLSTight[muonidx]; //[NMus]
    TheObj->MuIsTM2DCompLoose = TheTree->MuIsTM2DCompLoose[muonidx]; //[NMus]
    TheObj->MuIsTM2DCompTight = TheTree->MuIsTM2DCompTight[muonidx]; //[NMus]
    TheObj->MuIsTMOneStationLoose = TheTree->MuIsTMOneStationLoose[muonidx]; //[NMus]
    TheObj->MuIsTMOneStationTight = TheTree->MuIsTMOneStationTight[muonidx]; //[NMus]
    TheObj->MuIsTMLSOptLowPtLoose = TheTree->MuIsTMLSOptLowPtLoose[muonidx]; //[NMus]
    TheObj->MuIsTMLSAngLoose = TheTree->MuIsTMLSAngLoose[muonidx]; //[NMus]
    TheObj->MuIsTMLastStationAngTight = TheTree->MuIsTMLastStationAngTight[muonidx]; //[NMus]
    TheObj->MuIsTMOneStationAngTight = TheTree->MuIsTMOneStationAngTight[muonidx]; //[NMus]
    TheObj->MuIsTMOneStationAngLoose = TheTree->MuIsTMOneStationAngLoose[muonidx]; //[NMus]

    TheObj->GenID = TheTree->MuGenID[muonidx];
    TheObj->GenStatus = TheTree->MuGenStatus[muonidx];
    TheObj->GenPt = TheTree->MuGenPt[muonidx];
    TheObj->GenEta = TheTree->MuGenEta[muonidx];
    TheObj->GenPhi = TheTree->MuGenPhi[muonidx];
    TheObj->GenE = TheTree->MuGenE[muonidx];
    TheObj->GenMID = TheTree->MuGenMID[muonidx];
    TheObj->GenMStatus = TheTree->MuGenMStatus[muonidx];
    TheObj->GenMPt = TheTree->MuGenMPt[muonidx];
    TheObj->GenMEta = TheTree->MuGenMEta[muonidx];
    TheObj->GenMPhi = TheTree->MuGenMPhi[muonidx];
    TheObj->GenME = TheTree->MuGenME[muonidx];
    TheObj->GenGMID = TheTree->MuGenGMID[muonidx];
    TheObj->GenGMStatus = TheTree->MuGenGMStatus[muonidx];
    TheObj->GenGMPt = TheTree->MuGenGMPt[muonidx];
    TheObj->GenGMEta = TheTree->MuGenGMEta[muonidx];
    TheObj->GenGMPhi = TheTree->MuGenGMPhi[muonidx];
    TheObj->GenGME = TheTree->MuGenGME[muonidx];

    return (ElectronAnalysis::Muon*) TheObj;
}

template<class T>
BaseEventSelector<T>::BaseEventSelector(TreeReader* tree, BaseSelectorConfig* cfg, map<string, bool>* triggerResults) : TheTree(tree),
BarrelMaxEta_D(1.4442),
EndCapMinEta_D(1.566),
EndCapMaxEta_D(2.5),
Config(cfg),
EleSelectionHistos("EleSelection", true, true, false, cfg->EnableElectronControlPlots),
MuonSelectionHistos("MuonSelection", true, true, false, cfg->EnableMuonControlPlots),
JetSelectionHistos("JetSelection", true, true, false, cfg->EnableJetControlPlots),
EventSelectionHistos("EventSelection", true, true, false, cfg->EnableEventControlPlots),
EventSelectionHistosAfterObjectCreation("EventSelectionHistosAfterObjectCreation", true, true, false, cfg->EnableEventControlPlots),
TriggerResults(triggerResults) {
}

template<class T>
void BaseEventSelector<T>::AddSelectionStepsElectron() {
    this->EleSelectionHistos.CreateHistos("All");
    this->EleSelectionHistos.CreateHistos("TrackerDivenRejection");
    this->EleSelectionHistos.CreateHistos("PtCut");
    this->EleSelectionHistos.CreateHistos("ID");
    this->EleSelectionHistos.CreateHistos("Iso");
    this->EleSelectionHistos.CreateHistos("D0");
    this->EleSelectionHistos.CreateHistos("Conv");
    this->EleSelectionHistos.CreateHistos("CaloEnergy");
    this->EleSelectionHistos.CreateHistos("Eta");
    this->EleSelectionHistos.CreateHistos("DrMuon");
    this->EleSelectionHistos.CreateHistos("Charges");
}

template<class T>
void BaseEventSelector<T>::AddSelectionPlotsElectron() {
    ObjectPropertyFromFltArray::ObjectInfo elPtInf("Pt", "P_{t}", 10, 200, 50, "Electron", 0);
    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    this->EleSelectionHistos.AddHisto1ToAll(ElePt);

    ObjectPropertyFromFltArray::ObjectInfo elEtaInf("Eta", "#eta", -3, 3, 60, "Electron", 1);
    ObjectPropertyFromFltArray* EleEta = new ObjectPropertyFromFltArray(this->TheTree->ElEta, &(this->TheTree->NEles), elEtaInf);
    this->EleSelectionHistos.AddHisto1ToAll(EleEta);

    ObjectPropertyFromFltArray::ObjectInfo elIsoInf("Iso03", "Iso_{03}", 0, 2, 100, "Electron", 2);
    ObjectPropertyFromFltArray* EleIso = new ObjectPropertyFromFltArray(this->TheTree->ElRelIso03, &(this->TheTree->NEles), elIsoInf);
    this->EleSelectionHistos.AddHisto1ToAll(EleIso);
}

template<class T>
bool BaseEventSelector<T>::CheckEleID(int eleidx) {
    double barrelIdSieie[6] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};
    double barrelIdDPhi[6] = {0.8, 0.8, 0.06, 0.06, 0.03, 0.025};
    double barrelIdDEta[6] = {0.007, 0.007, 0.006, 0.004, 0.004, 0.004};
    double barrelIdHOE[6] = {0.15, 0.12, 0.04, 0.04, 0.025, 0.025};

    double endcapIdSieie[6] = {0.03, 0.03, 0.03, 0.03, 0.03, 0.03};
    double endcapIdDPhi[6] = {0.7, 0.7, 0.04, 0.03, 0.02, 0.02};
    double endcapIdDEta[6] = {0.7, 0.7, 0.04, 0.03, 0.02, 0.02};
    double endcapIdHOE[6] = {0.7, 0.7, 0.04, 0.03, 0.02, 0.02};

    BaseSelectorConfig::ElectronSelection::ElectronID eleID = this->Config->ElectronCuts.id;

    int val = 100;

    switch (this->Config->ElectronCuts.id) {
        case BaseSelectorConfig::ElectronSelection::idNone:
            return true;
        case BaseSelectorConfig::ElectronSelection::idRobustTight:
            return TheTree->ElIDRobustTight[eleidx];
        case BaseSelectorConfig::ElectronSelection::idRobustLoose:
            return TheTree->ElIDRobustLoose[eleidx];
        case BaseSelectorConfig::ElectronSelection::idLoose:
            return TheTree->ElIDLoose[eleidx];
        case BaseSelectorConfig::ElectronSelection::idTight:
            return TheTree->ElIDTight[eleidx];
        case BaseSelectorConfig::ElectronSelection::idWP80:
            val = TheTree->ElIDsimpleWP80relIso[eleidx];
            return ( (val == 1) || (val == 3) || (val == 5) || (val == 7));
        case BaseSelectorConfig::ElectronSelection::idWP95:
            val = TheTree->ElIDsimpleWP95relIso[eleidx];
            return ( (val == 1) || (val == 3) || (val == 5) || (val == 7));
        default:
            bool inBarrel = InBarrel(TheTree->ElEta[eleidx]);
            bool inEndcap = InEndcap(TheTree->ElEta[eleidx]);

            double sigmaIeIe = TheTree->ElSigmaIetaIeta[eleidx];
            double dphi = TheTree->ElDeltaPhiSuperClusterAtVtx[eleidx];
            double deta = TheTree->ElDeltaEtaSuperClusterAtVtx[eleidx];
            double hoe = TheTree->ElHcalOverEcal[eleidx];

            if (inBarrel)
                return sigmaIeIe < barrelIdSieie[eleID] && hoe < barrelIdHOE[eleID] &&
                    deta < barrelIdDEta[eleID] && dphi < barrelIdDPhi[eleID];

            if (inEndcap)
                return sigmaIeIe < endcapIdSieie[eleID] && hoe < endcapIdHOE[eleID] &&
                    deta < endcapIdDEta[eleID] && dphi < endcapIdDPhi[eleID];
    }
}

template<class T>
bool BaseEventSelector<T>::CheckEleD0(int eleidx) {
    double val = 0;
    switch (this->Config->ElectronCuts.d0) {
        case BaseSelectorConfig::ElectronSelection::D0None:
            return true;
        case BaseSelectorConfig::ElectronSelection::D0BS:
            val = this->TheTree->ElD0BS[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::D0PV:
            val = this->TheTree->ElD0PV[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::D0BSSign:
            val = this->TheTree->ElD0BS[eleidx] / this->TheTree->ElD0E[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::D0PVSign:
            val = this->TheTree->ElD0PV[eleidx] / this->TheTree->ElD0E[eleidx];
            break;
    }

    return (fabs(val) < this->Config->ElectronCuts.d0Cut);
}

template<class T>
bool BaseEventSelector<T>::CheckEleIso(int eleidx) {
    double barrelIsoCombined[6] = {0.15, 0.10, 0.09, 0.07, 0.04, 0.03};
    double barrelIsotrackRel03[6] = {0.15, 0.12, 0.09, 0.09, 0.05, 0.04};
    double barrelIsoecalRel03[6] = {2.00, 0.09, 0.08, 0.07, 0.06, 0.04};
    double barrelIsohcalRel03[6] = {0.12, 0.10, 0.10, 0.10, 0.03, 0.03};

    double endcapIsoCombined[6] = {0.1, 0.07, 0.06, 0.06, 0.03, 0.02};
    double endcapIsotrackRel03[6] = {0.08, 0.05, 0.05, 0.04, 0.025, 0.025};
    double endcapIsoecalRel03[6] = {0.06, 0.06, 0.05, 0.05, 0.025, 0.02};
    double endcapIsohcalRel03[6] = {0.05, 0.03, 0.025, 0.025, 0.02, 0.02};

    bool inBarrel = InBarrel(TheTree->ElEta[eleidx]);
    bool inEndcap = InEndcap(TheTree->ElEta[eleidx]);

    BaseSelectorConfig::ElectronSelection::ElectronID eleID = this->Config->ElectronCuts.id;
    double val = 0;
    switch (this->Config->ElectronCuts.iso) {
        case BaseSelectorConfig::ElectronSelection::isoNone:
            return true;
        case BaseSelectorConfig::ElectronSelection::isoComb03:
            val = this->TheTree->ElRelIso03[eleidx] * this->TheTree->ElPt[eleidx];
            if (this->Config->ElectronCuts.isoUseWPInfo) {
                if (inBarrel)
                    this->Config->ElectronCuts.isoCut = barrelIsoCombined[eleID];
                if (inEndcap)
                    this->Config->ElectronCuts.isoCut = endcapIsoCombined[eleID];
            }
            break;
        case BaseSelectorConfig::ElectronSelection::isoComb04:
            val = this->TheTree->ElRelIso04[eleidx] * this->TheTree->ElPt[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::isoTrk03:
            val = this->TheTree->ElDR03TkSumPt[eleidx];
            if (this->Config->ElectronCuts.isoUseWPInfo) {
                if (inBarrel)
                    this->Config->ElectronCuts.isoCut = barrelIsotrackRel03[eleID];
                if (inEndcap)
                    this->Config->ElectronCuts.isoCut = endcapIsotrackRel03[eleID];
            }
            break;
        case BaseSelectorConfig::ElectronSelection::isoTrk04:
            val = this->TheTree->ElDR04TkSumPt[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::isoECal03:
            val = this->TheTree->ElDR03EcalRecHitSumEt[eleidx];
            if (this->Config->ElectronCuts.isoUseWPInfo) {
                if (inBarrel)
                    this->Config->ElectronCuts.isoCut = barrelIsoecalRel03[eleID];
                if (inEndcap)
                    this->Config->ElectronCuts.isoCut = endcapIsoecalRel03[eleID];
            }
            break;
        case BaseSelectorConfig::ElectronSelection::isoECal04:
            val = this->TheTree->ElDR04EcalRecHitSumEt[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::isoHCal03:
            val = this->TheTree->ElDR03HcalTowerSumEt[eleidx];
            if (this->Config->ElectronCuts.isoUseWPInfo) {
                if (inBarrel)
                    this->Config->ElectronCuts.isoCut = barrelIsohcalRel03[eleID];
                if (inEndcap)
                    this->Config->ElectronCuts.isoCut = endcapIsohcalRel03[eleID];
            }
            break;
        case BaseSelectorConfig::ElectronSelection::isoHCal04:
            val = this->TheTree->ElDR04HcalTowerSumEt[eleidx];
            break;
        case BaseSelectorConfig::ElectronSelection::isoHybrid:
            if (inBarrel)
                val = (TheTree->ElDR03TkSumPt[eleidx] + max(0., double(TheTree->ElDR03EcalRecHitSumEt[eleidx]) - 1.) + TheTree->ElDR03HcalTowerSumEt[eleidx]) / max(20., double( this->TheTree->ElEt[eleidx]));
            else if (inEndcap)
                val = (TheTree->ElDR03TkSumPt[eleidx] + TheTree->ElDR03EcalRecHitSumEt[eleidx] + TheTree->ElDR03HcalTowerSumEt[eleidx]) / max(20., double( this->TheTree->ElEt[eleidx]));

            this->Config->ElectronCuts.isoRelative = false;
            break;
        case BaseSelectorConfig::ElectronSelection::isoAllIndividuals:
            this->Config->ElectronCuts.isoUseWPInfo = true;

            this->Config->ElectronCuts.iso = BaseSelectorConfig::ElectronSelection::isoTrk03;
            bool ret = this->CheckEleIso(eleidx);

            this->Config->ElectronCuts.iso = BaseSelectorConfig::ElectronSelection::isoECal03;
            ret &= this->CheckEleIso(eleidx);

            this->Config->ElectronCuts.iso = BaseSelectorConfig::ElectronSelection::isoHCal03;
            ret &= this->CheckEleIso(eleidx);

            this->Config->ElectronCuts.iso = BaseSelectorConfig::ElectronSelection::isoAllIndividuals;
            return ret;
    }

    if (this->Config->ElectronCuts.isoUseWPInfo)
        this->Config->ElectronCuts.isoRelative = true;

    if (this->Config->ElectronCuts.isoRelative)
        val /= this->TheTree->ElPt[eleidx];

    if (this->Config->ElectronCuts.IsoCutBarrel > 0.0 && inBarrel)
        this->Config->ElectronCuts.isoCut = this->Config->ElectronCuts.IsoCutBarrel;

    if (this->Config->ElectronCuts.IsoCutEndCap > 0.0 && inEndcap)
        this->Config->ElectronCuts.isoCut = this->Config->ElectronCuts.IsoCutEndCap;


    return val < this->Config->ElectronCuts.isoCut;
}

template<class T>
bool BaseEventSelector<T>::CheckEleConv(int eleidx) {
    int missing[6] = {1, 1, 1, 0, 0, 0};
    double Dist[6] = {-1.0, 0.02, 0.02, 0.02, 0.02, 0.02};
    double DCot[6] = {-1.0, 0.02, 0.02, 0.02, 0.02, 0.02};

    double distVal = fabs(TheTree->ElConvPartnerTrkDist[eleidx]);
    double dcotVal = fabs(TheTree->ElConvPartnerTrkDCot[eleidx]);
    int nmissht = TheTree->ElNumberOfMissingInnerHits[eleidx];

    bool checkGeo = true;
    bool checkTrk = true;

    int cutOnMissingHits = 1;
    double cutOnDist = 0.02;
    double cutOnDCot = 0.02;

    BaseSelectorConfig::ElectronSelection::ElectronID eleID = this->Config->ElectronCuts.id;
    switch (this->Config->ElectronCuts.conv) {
        case BaseSelectorConfig::ElectronSelection::convGeoCutsOnly:
            checkTrk = false;
            break;
        case BaseSelectorConfig::ElectronSelection::convInnerHitsOnly:
            checkGeo = false;
            break;
        case BaseSelectorConfig::ElectronSelection::convUseWPInfo:

            switch (eleID) {
                case BaseSelectorConfig::ElectronSelection::idWP95:
                    return (TheTree->ElIDsimpleWP95relIso[eleidx] > 3);
                case BaseSelectorConfig::ElectronSelection::idWP80:
                    return (TheTree->ElIDsimpleWP80relIso[eleidx] > 3);
            }

            if (eleID == BaseSelectorConfig::ElectronSelection::idWP95)
                checkGeo = false;

            cutOnMissingHits = missing[eleID];
            break;
        case BaseSelectorConfig::ElectronSelection::convNone:
            return true;
        case BaseSelectorConfig::ElectronSelection::convORCutsHits2:
            checkGeo = false;
            checkTrk = true;
            cutOnMissingHits = 1;
            if ((distVal <= 0.02) && (dcotVal <= 0.02))
                return false;
            break;
    }

    bool ret = true;
    if (checkGeo)
        ret = (distVal > cutOnDist) && (dcotVal > cutOnDCot);

    if (checkTrk && ret)
        ret &= (nmissht <= cutOnMissingHits);

    return ret;
}

template<class T>
bool BaseEventSelector<T>::CheckElectron(int eleidx, bool fillHisto) {
    bool ret = true;

    if (fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 0);

    if (!(this->Config->ElectronCuts.isSet))
        return false;

    if (this->Config->ElectronCuts.RejectTrackerDrivensOnly && ret)
        ret = !(this->TheTree->ElEcalDriven[eleidx] == 0);

    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 1);

    ret &= (this->TheTree->ElPt[eleidx] > this->Config->ElectronCuts.ptCut);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 2);

    ret &= CheckEleID(eleidx);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 3);

    ret &= CheckEleIso(eleidx);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 4);

    ret &= CheckEleD0(eleidx);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 5);

    ret &= CheckEleConv(eleidx);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 6);

    ret &= (this->TheTree->ElCaloEnergy[eleidx] > this->Config->ElectronCuts.CaloEnergy);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 7);


    ret &= fabs(this->TheTree->ElEta[eleidx]) < 2.5;
    if (this->Config->ElectronCuts.RejectCrack) {
        bool inBarrel = InBarrel(TheTree->ElSCEta[eleidx]);
        bool inEndcap = InEndcap(TheTree->ElSCEta[eleidx]);

        bool inGap = !(inBarrel || inEndcap);
        ret &= (!inGap);
    }
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 8);

    if (ret && this->Config->ElectronCuts.deltaRNearestMuon > 0) {
        int idOfNearestMuon = -1;

        double dr_muons = GetMinDistance(TheTree->ElEta[eleidx], TheTree->ElPhi[eleidx], TheTree->MuEta, TheTree->MuPhi, this->GoodMuons_ForElectronSelection, idOfNearestMuon);
        if (idOfNearestMuon >= 0)
            ret &= (dr_muons > this->Config->ElectronCuts.deltaRNearestMuon);

        if (ret && fillHisto)
            EleSelectionHistos.FillAll(&eleidx, 9);
    }

    if (this->Config->ElectronCuts.allChargesAgree) {
        ret &= (this->TheTree->ElCInfoIsGsfCtfScPixCons[eleidx] != 0);

        if (ret && fillHisto)
            EleSelectionHistos.FillAll(&eleidx, 10);
    } else if (this->Config->ElectronCuts.CTFGSFChargesAgree) {

        if (this->TheTree->ElClosestCtfTrackPt[eleidx] >= 0.0)
            ret &= (this->TheTree->ElCInfoIsGsfCtfCons[eleidx] != 0);
        else
            ret = false;

        if (ret && fillHisto)
            EleSelectionHistos.FillAll(&eleidx, 10);
    }

    return ret;
}

template<class T>
void BaseEventSelector<T>::AddSelectionStepsMuon() {
    this->MuonSelectionHistos.CreateHistos("All");
    this->MuonSelectionHistos.CreateHistos("GlobalAndTracke");
    this->MuonSelectionHistos.CreateHistos("Pt");
    this->MuonSelectionHistos.CreateHistos("Eta");
    this->MuonSelectionHistos.CreateHistos("NTkAndMuHits");
    this->MuonSelectionHistos.CreateHistos("NChi2");
    this->MuonSelectionHistos.CreateHistos("CaloEnergy");

}

template<class T>
void BaseEventSelector<T>::AddSelectionPlotsMuon() {
    ObjectPropertyFromFltArray::ObjectInfo muPtInf("Pt", "P_{t}", 10, 200, 50, "Muon", 0);
    ObjectPropertyFromFltArray* MuoPt = new ObjectPropertyFromFltArray(this->TheTree->MuPt, &(this->TheTree->NMus), muPtInf);
    this->MuonSelectionHistos.AddHisto1ToAll(MuoPt);

    ObjectPropertyFromFltArray::ObjectInfo muEtaInf("Eta", "#eta", -3, 3, 60, "Muon", 1);
    ObjectPropertyFromFltArray* MuoEta = new ObjectPropertyFromFltArray(this->TheTree->MuEta, &(this->TheTree->NMus), muEtaInf);
    this->MuonSelectionHistos.AddHisto1ToAll(MuoEta);

    ObjectPropertyFromFltArray::ObjectInfo muIsoInf("Iso03", "Iso_{03)", 0, 1.0, 100, "Muon", 2);
    ObjectPropertyFromFltArray* MuoIso = new ObjectPropertyFromFltArray(this->TheTree->MuRelIso03, &(this->TheTree->NMus), muIsoInf);
    this->MuonSelectionHistos.AddHisto1ToAll(MuoIso);

    ObjectPropertyFromFltArray::ObjectInfo muHCEInf("HCalE", "HCalE", 0, 100, 100, "Muon", 3);
    ObjectPropertyFromFltArray* muHCE = new ObjectPropertyFromFltArray(this->TheTree->MuEhad, &(this->TheTree->NMus), muHCEInf);
    this->MuonSelectionHistos.AddHisto1ToAll(muHCE);

    ObjectPropertyFromFltArray::ObjectInfo muECEInf("ECalE", "HCalE", 0, 100, 100, "Muon", 4);
    ObjectPropertyFromFltArray* MuoECE = new ObjectPropertyFromFltArray(this->TheTree->MuEem, &(this->TheTree->NMus), muECEInf);
    this->MuonSelectionHistos.AddHisto1ToAll(MuoECE);
}

template<class T>
bool BaseEventSelector<T>::CheckMuon(int muidx, bool fillHisto) {
    bool ret = true;

    if (!(this->Config->MuonCuts.isSet))
        return false;

    if (fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 0);

    if (this->Config->MuonCuts.GlobalMuon)
        ret &= (TheTree->MuIsGlobalMuon[muidx] != 0);

    if (this->Config->MuonCuts.TrackerMuon)
        ret &= (TheTree->MuIsTrackerMuon[muidx] != 0);

    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 1);

    ret &= (TheTree->MuPt[muidx] > this->Config->MuonCuts.PtCut);
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 2);
    ret &= (fabs(TheTree->MuEta[muidx]) < this->Config->MuonCuts.Eta);
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 3);
    ret &= TheTree->MuNTkHits[muidx] > this->Config->MuonCuts.NTrkHits;
    ret &= TheTree->MuNMuHits[muidx] > this->Config->MuonCuts.NMuHits;
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 4);
    ret &= TheTree->MuNChi2[muidx] < this->Config->MuonCuts.NChi2;
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 5);
    ret &= TheTree->MuEem[muidx] <= this->Config->MuonCuts.EEm;
    ret &= TheTree->MuEhad[muidx] <= this->Config->MuonCuts.EHad;
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 6);

    return ret;
}

template<class T>
void BaseEventSelector<T>::AddSelectionStepsJet() {
    this->JetSelectionHistos.CreateHistos("All");
    this->JetSelectionHistos.CreateHistos("Pt");
    this->JetSelectionHistos.CreateHistos("Eta");
    this->JetSelectionHistos.CreateHistos("DrElectron");
    this->JetSelectionHistos.CreateHistos("DrMuon");
    this->JetSelectionHistos.CreateHistos("ID");
    this->JetSelectionHistos.CreateHistos("GoodJets");
}

template<class T>
void BaseEventSelector<T>::AddSelectionPlotsJet() {
    ObjectPropertyFromFltArray::ObjectInfo jetPtInf("Pt", "P_{t}", 10, 310, 60, "Jet", 0);
    ObjectPropertyFromFltArray* JetPt = new ObjectPropertyFromFltArray(this->TheTree->JPt, &(this->TheTree->NJets), jetPtInf);
    this->JetSelectionHistos.AddHisto1ToAll(JetPt);

    ObjectPropertyFromFltArray::ObjectInfo jetEtaInf("Eta", "#eta", -5.0, 5.0, 100, "Jet", 1);
    ObjectPropertyFromFltArray* JetEta = new ObjectPropertyFromFltArray(this->TheTree->JEta, &(this->TheTree->NJets), jetEtaInf);
    this->JetSelectionHistos.AddHisto1ToAll(JetEta);

    ObjectPropertyFromFltArray::ObjectInfo jetChFracHadInf("HadChFrac", "Hadronic Charge Fraction", -1.0, 2.0, 30, "Jet", 2);
    ObjectPropertyFromFltArray* JetHadChFrac = new ObjectPropertyFromFltArray(this->TheTree->JChargedHadFrac, &(this->TheTree->NJets), jetChFracHadInf);
    this->JetSelectionHistos.AddHisto1ToAll(JetHadChFrac);

    ObjectPropertyFromFltArray::ObjectInfo jetChFracEMInf("EMChFrac", "EM Charge Fraction", -1.0, 2.0, 30, "Jet", 3);
    ObjectPropertyFromFltArray* JetEMChFrac = new ObjectPropertyFromFltArray(this->TheTree->JChargedEmFrac, &(this->TheTree->NJets), jetChFracEMInf);
    this->JetSelectionHistos.AddHisto1ToAll(JetEMChFrac);

    ObjectPropertyFromFltArray::ObjectInfo jetBtag1Info("btag1", "TrackCountingHighEff", -1.0, 5.0, 60, "Jet", 5);
    ObjectPropertyFromFltArray* JetBTag1 = new ObjectPropertyFromFltArray(this->TheTree->JbTagProbTkCntHighEff, &(this->TheTree->NJets), jetBtag1Info);
    this->JetSelectionHistos.AddHisto1ToAll(JetBTag1);

    ObjectPropertyFromFltArray::ObjectInfo jetBtag2Info("btag2", "simpleSecondaryVertexHighEff", -1.0, 5.0, 60, "Jet", 6);
    ObjectPropertyFromFltArray* JetBTag2 = new ObjectPropertyFromFltArray(this->TheTree->JbTagProbSimpSVHighEff, &(this->TheTree->NJets), jetBtag2Info);
    this->JetSelectionHistos.AddHisto1ToAll(JetBTag2);

    ObjectProperty<int>* JetChFrac = (*JetEMChFrac) + JetHadChFrac;
    this->JetSelectionHistos.AddHisto1ToAll(JetChFrac);

    ObjectPropertyFromFltArray::ObjectInfo jetEMNeuFInf("EMNeuFrac", "EM Neutral Fraction", -1.0, 2.0, 30, "Jet", 4);
    ObjectPropertyFromFltArray* JetEMNeuFrac = new ObjectPropertyFromFltArray(this->TheTree->JNeutralEmFrac, &(this->TheTree->NJets), jetEMNeuFInf);
    this->JetSelectionHistos.AddHisto1ToAll(JetEMNeuFrac);

    ObjectProperty<int>* JetEmFrac = (*JetEMChFrac) + JetEMNeuFrac;
    this->JetSelectionHistos.AddHisto1ToAll(JetEmFrac);
}

template<class T>
bool BaseEventSelector<T>::CheckJet(int jetIdx) {
    bool ret = true;

    this->JetSelectionHistos.FillAll(&jetIdx, 0);

    if (!(this->Config->JetCuts.isSet))
        return false;

    ret &= (TheTree->JPt[jetIdx] > this->Config->JetCuts.PtCut);
    if (ret) this->JetSelectionHistos.FillAll(&jetIdx, 1);

    ret &= (fabs(TheTree->JEta[jetIdx]) < this->Config->JetCuts.Eta);
    if (Config->JetCuts.RejectCrack) {
        double eta = TheTree->JEta[jetIdx];
        bool inBarrel = InBarrel(eta);
        bool inEndcap = InEndcap(eta);

        bool inGap = !(inBarrel || inEndcap);
        ret &= !(inGap);
    }

    if (ret) this->JetSelectionHistos.FillAll(&jetIdx, 2);

    if (this->Config->JetCuts.DrElectron > 0.0) {
        int idOfNearestEle = -1;
        double distEle = this->GetMinDistance(TheTree->JEta[jetIdx], TheTree->JPhi[jetIdx], TheTree->ElEta, TheTree->ElPhi, this->GoodElectrons_ForJetSelection, idOfNearestEle);
        if (idOfNearestEle != -1)
            ret &= (distEle > this->Config->JetCuts.DrElectron);

        if (ret) this->JetSelectionHistos.FillAll(&jetIdx, 3);
    }

    if (this->Config->JetCuts.DrMuon > 0.0) {
        int idOfNearestMuo = -1;
        double distMuo = this->GetMinDistance(TheTree->JEta[jetIdx], TheTree->JPhi[jetIdx], TheTree->MuEta, TheTree->MuPhi, this->GoodMuons_ForJetSelection, idOfNearestMuo);
        if (idOfNearestMuo != -1)
            ret &= (distMuo > this->Config->JetCuts.DrMuon);

        if (ret) this->JetSelectionHistos.FillAll(&jetIdx, 4);
    }

    if (this->Config->JetCuts.checkID) {
        ret &= this->IsGoodJet(jetIdx);
        if (ret) this->JetSelectionHistos.FillAll(&jetIdx, 5);
    }

    return ret;
}

template<class T>
bool BaseEventSelector<T>::IsGoodJet(int index) {
    if (!(TheTree->JNConstituents[index] > 1)) return false;
    if (!(TheTree->JNeutralEmFrac[index] < 0.99)) return false;
    if (!(TheTree->JNeutralHadFrac[index] < 0.99)) return false;
    if (!(TheTree->JChargedEmFrac[index] < 0.99)) return false;
    if (fabs(TheTree->JEta[index]) < 2.4) { // Cuts for |eta|<2.4
        if (!(TheTree->JChargedHadFrac[index] > 0.00)) return false;
        if (!(TheTree->JNAssoTracks[index] > 0)) return false;
        // TODO : are you sure nassotracks is equal to chargedMultiplicity
    }
    return true;
};

template<class T>
void BaseEventSelector<T>::AddSelectionStepsEvent() {
}

template<class T>
void BaseEventSelector<T>::AddSelectionPlotsEvent() {
}
#endif	/* BASEEVENTSELECTOR_H */
