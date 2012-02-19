/* 
 * File:   SingleLeptonSelector.cc
 * Author: hbakhshi
 * 
 * Created on January 28, 2011, 7:39 PM
 */

#include "../include/SingleLeptonSelector.h"
#include "boost/detail/container_fwd.hpp"

SingleLeptonSelector::SingleLeptonSelector(TreeReaderEvent* tree, SingleLeptonConfig* cfg, map<string, bool>* triggerResults) : SingleLeptonSelector::BASE(tree, cfg, triggerResults),
LastRead(NULL) {
    for (edm::VParameterSet::const_iterator cat = cfg->ElectronCategories.begin(); cat != cfg->ElectronCategories.end(); cat++)
        std::cout << cat->getParameter<std::string > ("Name") << endl;
}

void SingleLeptonSelector::End(TDirectory* dir) {
    BASE::End(dir);


    std::map<string, int> tmp_map_muon;
    for (edm::VParameterSet::const_iterator cat = ((SingleLeptonConfig*) Config)->MuonCategories.begin(); cat != ((SingleLeptonConfig*) Config)->MuonCategories.end(); cat++)
        tmp_map_muon[cat->getParameter<std::string > ("Name")] = 0;

    std::map<string, int> tmp_map;
    for (edm::VParameterSet::const_iterator cat = ((SingleLeptonConfig*) Config)->ElectronCategories.begin(); cat != ((SingleLeptonConfig*) Config)->ElectronCategories.end(); cat++)
        tmp_map[cat->getParameter<std::string > ("Name")] = 0;

    cout << endl << "Order of digits in status of : ";

    for (std::map<string, int>::const_reverse_iterator map_itr = tmp_map_muon.rbegin(); map_itr != tmp_map_muon.rend(); map_itr++)
        cout << "(muon_" << map_itr->first << ")";

    for (std::map<string, int>::const_reverse_iterator map_itr = tmp_map.rbegin(); map_itr != tmp_map.rend(); map_itr++)
        cout << "(electron_" << map_itr->first << ")";

    cout << endl;
}

bool SingleLeptonSelector::CheckMuon(int muidx) {
    bool ret = BASE::CheckMuon(muidx);

    ret &= fabs(TheTree->MuD0BS[muidx]) < this->Config->MuonCuts.d0Cut;
    if (ret)
        this->MuonSelectionHistos.FillAll(&muidx, 7);

    double iso = (TheTree->MuIso03SumPt[muidx] + TheTree->MuIso03EmEt[muidx] + TheTree->MuIso03HadEt[muidx]) / std::max(TheTree->MuPt[muidx], 20.0f);
    ret &= (iso < this->Config->MuonCuts.isoCut);
    if (ret)
        this->MuonSelectionHistos.FillAll(&muidx, 8);

    return ret;
}

void SingleLeptonSelector::AddSelectionStepsElectron() {
    BASE::AddSelectionStepsElectron();

    this->EleSelectionHistos.CreateHistos("GoodElectrons");
}

void SingleLeptonSelector::AddSelectionStepsMuon() {
    BASE::AddSelectionStepsMuon();

    this->MuonSelectionHistos.CreateHistos("D0");
    this->MuonSelectionHistos.CreateHistos("Iso");
    this->MuonSelectionHistos.CreateHistos("GoodMuons");
}

void SingleLeptonSelector::AddSelectionStepsEvent() {
    BASE::EventSelectionHistos.CreateHistos("All");
}

void SingleLeptonSelector::AddSelectionPlotsEvent() {
    ObjectPropertyFromFltArray::ObjectInfo elPtInf("ElectronPt", "P_{t} of electrons", 5, 205, 50, "Electron", 0);
    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(ElePt);

    ObjectPropertyFromFltArray::ObjectInfo elEtaInf("ElectronEta", "#eta of electrons", -3, 3, 60, "Electron", 1);
    ObjectPropertyFromFltArray* EleEta = new ObjectPropertyFromFltArray(this->TheTree->ElEta, &(this->TheTree->NEles), elEtaInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(EleEta);

    ObjectPropertyFromIntArray::ObjectInfo eleN("NEles", "number of electrons", 0, 10, 10, "Electron", 2);
    ObjectPropertyFromIntArray* EleN = new ObjectPropertyFromIntArray(&(this->TheTree->NEles), eleN);
    BASE::EventSelectionHistos.AddHisto1ToAll(EleN);

    ObjectPropertyFromFltArray::ObjectInfo muPtInf("MuonPt", "P_{t} of muons", 5, 205, 50, "Muons", 0);
    ObjectPropertyFromFltArray* MuoPt = new ObjectPropertyFromFltArray(this->TheTree->MuPt, &(this->TheTree->NMus), muPtInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(MuoPt);

    ObjectPropertyFromFltArray::ObjectInfo muEtaInf("MuonEta", "#eta of muons", -3, 3, 60, "Muons", 1);
    ObjectPropertyFromFltArray* MuoEta = new ObjectPropertyFromFltArray(this->TheTree->MuEta, &(this->TheTree->NMus), muEtaInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(MuoEta);

    ObjectPropertyFromIntArray::ObjectInfo muN("NMuons", "number of muons", 0, 10, 10, "Muons", 2);
    ObjectPropertyFromIntArray* MuoN = new ObjectPropertyFromIntArray(&(this->TheTree->NMus), muN);
    BASE::EventSelectionHistos.AddHisto1ToAll(MuoN);

    ObjectPropertyFromFltArray::ObjectInfo jPtInf("JPt", "P_{t} of jets", 5, 205, 50, "Jets", 0);
    ObjectPropertyFromFltArray* JPt = new ObjectPropertyFromFltArray(this->TheTree->JPt, &(this->TheTree->NJets), jPtInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(JPt);

    ObjectPropertyFromFltArray::ObjectInfo jEtaInf("JEta", "#eta of jets", -3, 3, 60, "Jets", 1);
    ObjectPropertyFromFltArray* JEta = new ObjectPropertyFromFltArray(this->TheTree->JEta, &(this->TheTree->NJets), jEtaInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(JEta);

    ObjectPropertyFromIntArray::ObjectInfo jN("NJets", "number of jets", 0, 10, 10, "Jets", 2);
    ObjectPropertyFromIntArray* JN = new ObjectPropertyFromIntArray(&(this->TheTree->NJets), jN);
    BASE::EventSelectionHistos.AddHisto1ToAll(JN);

    ObjectPropertyFromFltArray::ObjectInfo met("PFMET", "PFMET", 20, 220, 40, "MET", 1);
    ObjectPropertyFromFltArray* MET = new ObjectPropertyFromFltArray(&(this->TheTree->PFMET), met);
    BASE::EventSelectionHistos.AddHisto1ToAll(MET);

    ObjectPropertyFromFltArray::ObjectInfo metSig("PFMETSig", "PFMET Significance", 0, 50, 50, "MET", 2);
    ObjectPropertyFromFltArray* METSig = new ObjectPropertyFromFltArray(&(this->TheTree->PFMETSignificance), metSig);
    BASE::EventSelectionHistos.AddHisto1ToAll(METSig);
}

void SingleLeptonSelector::AddTriggerHistos() {
    ObjectPropertyFromFltArray::ObjectInfo elPtInf("Pt", "P_{t}", 10, 200, 50, "Electron", 0);
    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    info::TheInfo->triggerCuts.AddHistogram(ElePt);

    ObjectPropertyFromIntArray::ObjectInfo elN("NElecs", "Number of electrons", 0, 20, 20, "Event", 0);
    ObjectPropertyFromIntArray* EleN = new ObjectPropertyFromIntArray(&(this->TheTree->NEles), elN);
    info::TheInfo->triggerCuts.AddHistogram(EleN);
}

SingleLeptonSelector::EVENTTYPE* SingleLeptonSelector::Read(int& stat) {

    if (LastRead != NULL) {
        LastRead->fChain = 0;
        delete LastRead;
        LastRead = NULL;
    }

    bool Trigger = false;
    for (vector<string>::iterator trg = this->Config->TriggerNames.begin(); trg != Config->TriggerNames.end(); trg++) {
        map<string, bool>::const_iterator res = TriggerResults->find(*trg);
        if (res == TriggerResults->end())
            throw SelectionException("SingleLeptonSelector::Read ==> Trigger " + *trg + " not found");

        Trigger = (Trigger || res->second);
    }
    if (!Trigger) {
        stat = 5;
        return NULL;
    }


    if (!this->Config->CheckRunEventID(TheTree->Run, TheTree->Event)) {
        stat = 6;
        return NULL;
    }

    BASE::FillAllValue(BASE::EventSelectionHistos.at(0));


    map< string, vector<int> >* CategorizedLeptons;

    bool HasAtleastOneLepton = false;


    map< string, vector<int> > CategorizedMuons;
    bool isMuon = ((SingleLeptonConfig*) Config)->IsMuon;
    if (isMuon) {
        FillGoodMuons();
        int k = 0;
        for (int i = 0; i < GoodMuons.size(); i++) {
            k = GoodMuons[i];
            this->MuonSelectionHistos.FillAll(&k, this->MuonSelectionHistos.size() - 1);
        }

        HasAtleastOneLepton = this->CategorizeMuons(CategorizedMuons, stat);
    }

    map< string, vector<int> > CategorizedElectrons;
    bool IsElectron = ((SingleLeptonConfig*) Config)->IsElectron;
    if (IsElectron) {
        FillGoodElecs();
        int k = 0;
        for (int i = 0; i < GoodElecs.size(); i++) {
            k = GoodElecs[i];
            this->EleSelectionHistos.FillAll(&k, this->EleSelectionHistos.size() - 1);
        }

        HasAtleastOneLepton |= this->CategorizeElectrons(CategorizedElectrons, stat);
    }

    if (!HasAtleastOneLepton)
        return NULL;

    SingleLeptonSelector::EVENTTYPE* RET = new SingleLeptonSelector::EVENTTYPE(this->TheTree);

    if (((SingleLeptonConfig*) Config)->ArrangeJets) {
        FillGoodJets();
        int k = 0;
        for (int i = 0; i < GoodJets.size(); i++) {
            k = GoodJets[i];
            this->JetSelectionHistos.FillAll(&k, this->JetSelectionHistos.size() - 1);
            RET->GoodJets.push_back(k);
        }
        double ht = 0;
        for (int i = 0; i < GoodJets.size(); i++)
            ht += TheTree->JPt[GoodJets.at(i)];

        RET->HTJets = ht;
    }

    int digit_number = 1;
    stat = 0;

    bool Pass;
    for (uint emu = 0; emu < 2; emu++) { // emu = 0, electron -------- 1, muon
        bool isElectron = true;
        if (emu == 0)
            CategorizedLeptons = &CategorizedElectrons;
        else if (emu == 1) {
            CategorizedLeptons = &CategorizedMuons;
            isElectron = false;
        }

        for (map< string, vector<int> >::const_iterator AnalysisMode = CategorizedLeptons->begin();
                AnalysisMode != CategorizedLeptons->end(); AnalysisMode++) {

            Pass = true;

            stat += AnalysisMode->second.size() * digit_number;

            for (vector<int>::const_iterator lepton_in_cat = AnalysisMode->second.begin(); lepton_in_cat != AnalysisMode->second.end(); lepton_in_cat++) {
                int lepIdx = *(lepton_in_cat);

                if (isElectron)
                    RET->ElectronsToAnalyze[AnalysisMode->first].push_back(lepIdx);
                else
                    RET->MuonsToAnalyze[AnalysisMode->first].push_back(lepIdx);
            }
        }
        //cout << digit_number << ":" << AnalysisMode->first << "__";

        digit_number *= 10;
    }

    if (Pass) {
        stat += 3 * digit_number;
    } else {
        stat += 2 * digit_number;
        RET->fChain = 0;
        delete RET;
        return NULL;
    }

    LastRead = RET;
    return RET;
}

bool SingleLeptonSelector::CategorizeMuons(map< string, vector<int> >& CategorizedMuons, int& stat_value) {
    edm::VParameterSet* psCatMuons = &((SingleLeptonConfig*) Config)->MuonCategories;

    bool ret = false;

    int firstDigitsHolder = 1;
    int numberDisplay = 0;

    for (edm::VParameterSet::const_iterator category = psCatMuons->begin(); category != psCatMuons->end(); category++) {
        double isoCone = category->getParameter<double>("ISOCone");
        double isoCUT = category->getParameter<double>("ISOCut");
        string cat_name = category->getParameter<string > ("Name");
        int minNumber = category->getUntrackedParameter<int > ("MinNumber", -1);
        int maxNumber = category->getUntrackedParameter<int > ("MaxNumber", 1000);

        for (deque<int>::const_iterator muo_ = GoodMuons.begin(); muo_ != GoodMuons.end(); muo_++) {
            double muon_eta = TheTree->MuEta[*muo_];

            double iso_val = 0;
            if (isoCone == 0.3) {
                iso_val = this->TheTree->MuRelIso03[*muo_];
            } else if (isoCone == 1.0) {
                iso_val = (TheTree->MuIso03SumPt[*muo_] + TheTree->MuIso03EmEt[*muo_] + TheTree->MuIso03HadEt[*muo_]) / std::max(TheTree->MuPt[*muo_], 20.0f);
            }

            if (iso_val > isoCUT)
                continue;
            else
                CategorizedMuons[cat_name].push_back(*muo_);
        }

        int temp_holder = CategorizedMuons[cat_name].size();
        if (temp_holder > minNumber && temp_holder < maxNumber)
            ret = true;

        numberDisplay += firstDigitsHolder * temp_holder;
        firstDigitsHolder *= 10;
    }

    stat_value = firstDigitsHolder + numberDisplay;
    return ret;
}

bool SingleLeptonSelector::CategorizeElectrons(map< string, vector<int> >& CategorizedElectrons, int& stat_value) {
    edm::VParameterSet* psCatElcs = &((SingleLeptonConfig*) Config)->ElectronCategories;

    bool ret = false;

    int firstDigitsHolder = 1;
    int numberDisplay = 0;

    for (edm::VParameterSet::const_iterator category = psCatElcs->begin(); category != psCatElcs->end(); category++) {
        int isoMethod = category->getParameter<int>("ISOMethod");
        double isoValEC, isoValBRL;
        if (isoMethod > -1) {
            isoValEC = category->getParameter<double>("ISOValEC");
            isoValBRL = category->getParameter<double>("ISOValBRL");
        }
        int minNumber = category->getUntrackedParameter<int > ("MinNumber", -1);
        int maxNumber = category->getUntrackedParameter<int > ("MaxNumber", 1000);

        int WP = category->getParameter<int>("WP");
        vector<int> Values = category->getParameter< vector<int> >("AcceptedValues");

        string cat_name = category->getParameter<string > ("Name");

        for (deque<int>::const_iterator ele_ = GoodElecs.begin(); ele_ != GoodElecs.end(); ele_++) {

            double ele_eta = TheTree->ElEta[*ele_];
            bool inbarrel = InBarrel(ele_eta);
            bool inendcap = InEndcap(ele_eta);

            if (!inendcap && !inbarrel)
                continue;

            if (isoMethod > -1) {
                double iso_val = 0;
                switch (isoMethod) {
                    case BaseSelectorConfig::ElectronSelection::isoComb03:
                        iso_val = this->TheTree->ElRelIso03[*ele_];
                        break;
                    case BaseSelectorConfig::ElectronSelection::isoComb04:
                        iso_val = this->TheTree->ElRelIso04[*ele_];
                        break;
                    case BaseSelectorConfig::ElectronSelection::isoHybrid:
                        if (inbarrel)
                            iso_val = (TheTree->ElDR03TkSumPt[*ele_] + max(0., TheTree->ElDR03EcalRecHitSumEt[*ele_] - 1.) + TheTree->ElDR03HcalTowerSumEt[*ele_]) / max(20.f, this->TheTree->ElEt[*ele_]);
                        else if (inendcap)
                            iso_val = (TheTree->ElDR03TkSumPt[*ele_] + TheTree->ElDR03EcalRecHitSumEt[*ele_] + TheTree->ElDR03HcalTowerSumEt[*ele_]) / max(20.f, this->TheTree->ElEt[*ele_]);
                        break;
                }

                if (inbarrel && iso_val > isoValBRL)
                    continue;

                if (inendcap && iso_val > isoValEC)
                    continue;
            }

            int value_ = -1;
            if (WP == 95)
                value_ = this->TheTree->ElIDsimpleWP95relIso[*ele_];
            else if (WP == 80)
                value_ = this->TheTree->ElIDsimpleWP80relIso[*ele_];

            if (std::find(Values.begin(), Values.end(), value_) != Values.end())
                CategorizedElectrons[cat_name].push_back(*ele_);
        }

        //to book the names :
        int temp_holder = CategorizedElectrons[cat_name].size();
        if (temp_holder > minNumber && temp_holder < maxNumber)
            ret = true;

        numberDisplay += firstDigitsHolder * temp_holder;
        firstDigitsHolder *= 10;
    }

    stat_value = firstDigitsHolder + numberDisplay;
    return ret;
}

std::string GetStatusString(int i) {
    switch (i) {
        case 1:
            return "OK";
        case 2:
            return "!OK, MET";
        case 3:
            return "!OK, #Jets";
        case 4:
            return "!OK, HT";
        case 5:
            return "!OK, Trigger";
        case 6:
            return "!OK, this event is not in the list of events given in config file";
    }


    int firstDigit = i;

    do {
        firstDigit /= 10;
    } while (firstDigit > 9);

    if (firstDigit == 3)
        return "OK, 3 - 2:>1 - 0:0 - 1:OK - 9:BadMT";
    else if (firstDigit == 2)
        return "!OK, M_T - 2:>1 - 0:0 - 1:OK - 9:BadMT";
    else if (firstDigit == 1)
        return "!OK, 1 ";

    return "";
}
