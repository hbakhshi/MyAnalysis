#include "ElectronCharge/include/ZEventSelector.h"
#include "DataFormats/interface/DiElectronEventProperties.h"

using namespace ElectronAnalysis;

ZEventSelector::ZEventSelector(TreeReader* tree, ZSelectorConfig* cfg, map<string, bool>* triggerResults) : BaseEventSelector<DiElectronEvent>(tree, cfg, triggerResults) {
    //cfg->EventCuts.Print();
}

DiElectronEvent* ZEventSelector::Read(int& status) {
    this->TheEvent.Weight = this->Weight;
    this->TheEvent.IsZ2ee = this->IsZee;
    this->TheEvent.EventNumber = this->TheTree->Event;
    this->TheEvent.RunID = this->TheTree->Run;

    bool Trigger = false;
    for (vector<string>::iterator trg = this->Config->TriggerNames.begin(); trg != Config->TriggerNames.end(); trg++) {
        map<string, bool>::const_iterator res = TriggerResults->find(*trg);
        if (res == TriggerResults->end())
            throw SelectionException("ZEventSelector::Read ==> Trigger " + *trg + " not found");

        Trigger = (Trigger || res->second);
    }
    if (!Trigger) {
        status = 250;
        return NULL;
    }

    FillGoodMuons();
    int k = 0;
    for (int i = 0; i < GoodMuons.size(); i++) {
        k = GoodMuons[i];
        this->MuonSelectionHistos.FillAll(&k, this->MuonSelectionHistos.size() - 1);
    }

    FillGoodElecs();
    k = 0;
    for (int i = 0; i < GoodElecs.size(); i++) {
        k = GoodElecs[i];
        this->EleSelectionHistos.FillAll(&k, this->EleSelectionHistos.size() - 1);
    }

    FillGoodJets();
    k = 0;
    double jetHTX = 0;
    double jetHTY = 0;
    double jetHTZ = 0;
    for (int i = 0; i < GoodJets.size(); i++) {
        k = GoodJets[i];
        this->JetSelectionHistos.FillAll(&k, this->JetSelectionHistos.size() - 1);

        jetHTX += this->TheTree->JPx[k];
        jetHTY += this->TheTree->JPy[k];
        jetHTZ += this->TheTree->JPz[k];
    }

    BASE::FillAllValue(BASE::EventSelectionHistos.at(0));

    ZSelectorConfig* cfg = (ZSelectorConfig*) (Config);
    status = 1;
    if ((cfg->EventCuts.VetoMoreThan2Eles && this->GoodElecs.size() == 2) || (!(cfg->EventCuts.VetoMoreThan2Eles) && this->GoodElecs.size() > 1)) {
        std::deque<int>::const_iterator itr = this->GoodElecs.begin();

        MakeElectron(*itr, &(TheEvent.FirstElectron_));

        TheEvent.FirstElectron = &(TheEvent.FirstElectron_);

        itr++;

        MakeElectron(*itr, &(TheEvent.SecondElectron_));

        TheEvent.SecondElectron = &(TheEvent.SecondElectron_);

        this->TheEvent.NMuons = GoodMuons.size();
        this->TheEvent.NJets = GoodJets.size();
        this->TheEvent.PFMET.SetCoordinates(TheTree->PFMETpx, TheTree->PFMETpy, 0);
        this->TheEvent.JetsSumPt.SetCoordinates(jetHTX, jetHTY, jetHTZ);

        BASE::FillAllValue(BASE::EventSelectionHistos.at(1));
        if (!(cfg->EventCuts.VetoMoreThan2Eles) && this->GoodElecs.size() > 2) {
            status = 2;
            BASE::FillAllValue(BASE::EventSelectionHistos.at(2));
        }

        ZSelectorConfig* cfg = (ZSelectorConfig*) (Config);
        if (CheckEvent(status)) {

            if (TheEvent.PFMET.Rho() < cfg->EventCuts.MET) {
                BASE::FillAllValue(BASE::EventSelectionHistos.at(7));
                EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 1);

                if (TheEvent.M_T() < cfg->EventCuts.MT) {
                    BASE::FillAllValue(BASE::EventSelectionHistos.at(8));
                    EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 2);
                    return &(this->TheEvent);
                } else
                    status = 102;
            } else
                status = 101;
        }
    } else {
        if (this->GoodElecs.size() > 2)
            status = 3;
        else if (this->GoodElecs.size() == 1)
            status = 4;
        else if (this->GoodElecs.size() == 0)
            status = 5;
    }
    return NULL;
}

bool ZEventSelector::CheckEvent(int& status) {
    ZSelectorConfig* cfg = (ZSelectorConfig*) (Config);

    bool ret = true;
    if (cfg->EventCuts.MatchWithGen)
        ret &= TheEvent.AreTheZElectronsReconstructedWell();

    if (!ret) {
        status = 11;
        return ret;
    }
    BASE::FillAllValue(BASE::EventSelectionHistos.at(3));

    if (cfg->EventCuts.JetVeto && ret)
        if (this->GoodJets.size() > 0)
            ret = false;

    if (!ret) {
        status = 12;
        return ret;
    }
    BASE::FillAllValue(BASE::EventSelectionHistos.at(4));

    if (ret) {
        double invMass = TheEvent.GetInvMass(cfg->EventCuts.InvMassMethod);

        ret &= (invMass < cfg->EventCuts.InvMassMax);

        if (ret)
            BASE::FillAllValue(BASE::EventSelectionHistos.at(5));

        ret &= (invMass > cfg->EventCuts.InvMassMin);

        //cout << cfg->EventCuts.InvMassMethod << " - " << invMass << " - " << ret << endl;
    }
    if (!ret)
        status = 13;
    else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(6));
        EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 0);
    }
    return ret;
}

void ZEventSelector::AddSelectionStepsElectron() {
    BASE::AddSelectionStepsElectron();

    this->EleSelectionHistos.CreateHistos("GoodElectrons");
}

void ZEventSelector::AddSelectionStepsMuon() {
    BASE::AddSelectionStepsMuon();

    this->MuonSelectionHistos.CreateHistos("GoodMuons");
}

void ZEventSelector::AddSelectionStepsEvent() {
    BASE::EventSelectionHistos.CreateHistos("All");
    BASE::EventSelectionHistos.CreateHistos("AtLeast2Electrons");
    BASE::EventSelectionHistos.CreateHistos("MoreThan2Electrons");
    BASE::EventSelectionHistos.CreateHistos("Gen");
    BASE::EventSelectionHistos.CreateHistos("JetVeto");
    BASE::EventSelectionHistos.CreateHistos("InvMassMax");
    BASE::EventSelectionHistos.CreateHistos("InvMassMin");
    BASE::EventSelectionHistos.CreateHistos("MET");
    BASE::EventSelectionHistos.CreateHistos("MT");

    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("InvMass");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("MET");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("MT");
}

void ZEventSelector::AddSelectionPlotsEvent() {
    ObjectPropertyFromFltArray::ObjectInfo elPtInf("ElectronPt", "P_{t} of electrons", 5, 205, 50, "Electron", 0);
    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(ElePt);

    ObjectPropertyFromFltArray::ObjectInfo elEtaInf("ElectronEta", "#eta of electrons", -3, 3, 60, "Electron", 1);
    ObjectPropertyFromFltArray* EleEta = new ObjectPropertyFromFltArray(this->TheTree->ElEta, &(this->TheTree->NEles), elEtaInf);
    BASE::EventSelectionHistos.AddHisto1ToAll(EleEta);

    ObjectPropertyFromIntArray::ObjectInfo eleN("NEles", "number of electrons", 0, 10, 10, "Electron", 2);
    ObjectPropertyFromIntArray* EleN = new ObjectPropertyFromIntArray(&(this->TheTree->NEles), eleN);
    BASE::EventSelectionHistos.AddHisto1ToAll(EleN);

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

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::HT < 0 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::HT < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::HT < 2 > ());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::MHT < 0 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::MHT < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::MHT < 2 > ());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::InvariantMass<ElectronAnalysis::Electron::_Momentum > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::InvariantMass<ElectronAnalysis::Electron::_SuperCluster > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::NumberOfJets());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::NumberOfMuons());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::M_T());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::PFMet());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::EtaOFElectron < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::EtaOFElectron<-1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::PtOFElectron < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiElectronEventProperties::PtOFElectron<-1 > ());
}

void ZEventSelector::AddTriggerHistos() {
    ObjectPropertyFromFltArray::ObjectInfo elPtInf("Pt", "P_{t}", 10, 200, 50, "Electron", 0);
    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    info::TheInfo->triggerCuts.AddHistogram(ElePt);

    ObjectPropertyFromIntArray::ObjectInfo elN("NElecs", "Number of electrons", 0, 20, 20, "Event", 0);
    ObjectPropertyFromIntArray* EleN = new ObjectPropertyFromIntArray(&(this->TheTree->NEles), elN);
    info::TheInfo->triggerCuts.AddHistogram(EleN);
}

string GetStatusString(int stat) {
    switch (stat) {
        case 1:
            return "OK";
        case 2:
            return "OK, But more than 2 eles";
        case 3:
            return "!OK, more than 2 eles"; //it depends to configurations. sometimes it accepts events with >2 eles
        case 4:
            return "!OK, just 1 eles";
        case 5:
            return "!OK, no eles";
        case 11:
            return "!OK, don't match with gen";
        case 12:
            return "!OK, Jet Veto";
        case 13:
            return "!OK, inv mass";
        case 100:
            return "!OK, user analyzer rejected";
        case 101:
            return "!OK, MET";
        case 102:
            return "!OK, M_T";
        case 250:
            return "!OK, Trigger";
    }

    return "No such status is defined";
}