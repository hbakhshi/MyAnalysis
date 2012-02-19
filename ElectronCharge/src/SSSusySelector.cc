#include <deque>

#include "DataFormats/interface/DiLeptonEventProperties.h"
#include "ElectronCharge/include/SSSusySelector.h"
#include "base/Configuration.h"
#include "boost/detail/container_fwd.hpp"

SSSusyEventSelector::SSSusyEventSelector(TreeReader* tree, SSSusySelectorConfig* config, map<string, bool>* triggerResults) : BASE(tree, config, triggerResults) {

    //    if (Config->LooseSelection)
    //        Config->ElectronCuts.SetIDInfo(BaseSelectorConfig::ElectronSelection::idWP95);
    //    else
    //        Config->ElectronCuts.SetIDInfo(BaseSelectorConfig::ElectronSelection::idWP80);
    //
    //    Config->ElectronCuts.SetIsoInfo(BaseSelectorConfig::ElectronSelection::isoHybrid, 0.1, true);
    //    Config->ElectronCuts.SetConvInfo(BaseSelectorConfig::ElectronSelection::convUseWPInfo);
    //    Config->ElectronCuts.SetD0Info(BaseSelectorConfig::ElectronSelection::D0BS, 0.02);

    TheEvent.FirstElectron = NULL;
    TheEvent.SecondElectron = NULL;
}

DiLepton* SSSusyEventSelector::Read(int& stat) {

    if (!this->Config->CheckRunEventID(TheTree->Run, TheTree->Event)) {
        stat = 101;
        return NULL;
    }

    bool Trigger = false;
    for (vector<string>::iterator trg = this->Config->TriggerNames.begin(); trg != Config->TriggerNames.end(); trg++) {
        map<string, bool>::const_iterator res = TriggerResults->find(*trg);
        if (res == TriggerResults->end())
            throw SelectionException("SSSusyEventSelector::Read ==> Trigger " + *trg + " not found");

        Trigger = (Trigger || res->second);
    }
    if (!Trigger) {
        stat = 25;
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
    for (int i = 0; i < GoodJets.size(); i++) {
        k = GoodJets[i];
        this->JetSelectionHistos.FillAll(&k, this->JetSelectionHistos.size() - 1);
    }

    //check if another event with this run-event id is already accepted,don't analyze it again.
    //It's here to prevent double countings due to mixing Muon and Electron samples.
    //    pair<multimap<int, int>::iterator, multimap<int, int>::iterator> range_runevent;
    //    range_runevent = RunEventNumbers.equal_range(TheTree->Run);
    //    multimap<int, int>::iterator it_runevent = std::find(range_runevent.first, range_runevent.second, pair<const int, int>(TheTree->Run, TheTree->LumiSection));
    //    if (it_runevent != range_runevent.second) {
    //        stat = 101;
    //        return NULL;
    //    }

    BASE::FillAllValue(BASE::EventSelectionHistos.at(0));

    if (true) { //TheTree->PrimVtxIsFake == 0 && TheTree->PrimVtxNdof > 4 && fabs(TheTree->PrimVtxz) < 24 && TheTree->PrimVtxRho < 2.0) {
        if ((this->GoodElecs.size() + this->GoodMuons.size()) > 1) {

            BASE::FillAllValue(BASE::EventSelectionHistos.at(1));


            int nNeg = 0;
            int nPos = 0;

            std::multimap<double, int, greater_equal<double> > lepton_pts;
            std::map<int, int> lepton_charges;
            for (int ii = 0; ii < GoodElecs.size(); ii++) {
                lepton_pts.insert(std::pair<double, int>(TheTree->ElPt[GoodElecs.at(ii)], ii));
                if ((lepton_charges[ii] = TheTree->ElCharge[GoodElecs.at(ii)]) > 0)
                    nPos++;
                else
                    nNeg++;
            }
            for (int ij = 0; ij < GoodMuons.size(); ij++) {
                lepton_pts.insert(pair<double, int>(TheTree->MuPt[GoodMuons.at(ij)], ij + GoodElecs.size()));
                if ((lepton_charges[ij + GoodElecs.size()] = TheTree->MuCharge[GoodMuons.at(ij)]) > 0)
                    nPos++;
                else
                    nNeg++;
            }


            Lepton* firstLepton;
            Lepton* secondLepton;

            multimap<double, int, greater_equal<double> >::const_iterator itr;

            multimap<double, int, greater_equal<double> >::const_iterator itr_1;
            multimap<double, int, greater_equal<double> >::const_iterator itr_2;

            if (lepton_pts.size() == 2) {
                itr_1 = lepton_pts.begin();
                itr_2 = lepton_pts.begin();
                itr_2++;
            } else if (lepton_pts.size() > 2) {
                itr = lepton_pts.begin();
                int chargeOfHieghstPtLep = lepton_charges[itr->second];

                int eventCharge = chargeOfHieghstPtLep;

                if ((chargeOfHieghstPtLep > 0 && nPos < 2) || (chargeOfHieghstPtLep < 0 && nNeg < 2)) {
                    itr++;
                    eventCharge = lepton_charges[ itr->second ];
                }

                bool isTheFirstSet = false;

                for (; itr != lepton_pts.end(); itr++)
                    if (lepton_charges[itr->second] == eventCharge) {
                        if (!isTheFirstSet) {
                            isTheFirstSet = true;
                            itr_1 = itr;
                        } else {
                            itr_2 = itr;
                            break;
                        }
                    }
            }

            try {
                if (itr_1->second < GoodElecs.size()) //it's an electron
                {
                    firstLepton = MakeElectron(GoodElecs.at(itr_1->second), NULL);
                } else {
                    firstLepton = MakeMuon(GoodMuons.at(itr_1->second - GoodElecs.size()), NULL);
                }

                if (itr_2->second < GoodElecs.size()) //it's an electron
                {
                    secondLepton = MakeElectron(GoodElecs.at(itr_2->second), NULL);
                } else {
                    secondLepton = MakeMuon(GoodMuons.at(itr_2->second - GoodElecs.size()), NULL);
                }
            } catch (...) {
                cout << "exception" << endl;
            }
            //
            //
            //            cout << endl << "Lepton " << itr_1->second << "(pt" << itr_1->first;
            //            cout << "-" << firstLepton->Pt << ")" << firstLepton->Charge << ";";
            //            cout << "Lepton " << itr_2->second << "(pt" << itr_2->first;
            //            cout << "-" << secondLepton->Pt << ")" << secondLepton->Charge << ";";
            //
            //            for (itr = lepton_pts.begin(); itr != lepton_pts.end(); itr++)
            //                cout << itr->first << "---" << itr->second << ";";


            if (firstLepton->Pt > ((SSSusySelectorConfig*) Config)->HighLeptonPt) {


                BASE::FillAllValue(BASE::EventSelectionHistos.at(2));

                if (this->TheEvent.FirstElectron != NULL) {
                    delete this->TheEvent.FirstElectron;
                    this->TheEvent.FirstElectron = NULL;
                }
                if (this->TheEvent.SecondElectron != NULL) {
                    delete this->TheEvent.SecondElectron;
                    this->TheEvent.SecondElectron = NULL;
                }

                this->TheEvent.FirstElectron = firstLepton;
                this->TheEvent.SecondElectron = secondLepton;

                this->TheEvent.Jets.clear();
                for (deque<int>::const_iterator iJet = GoodJets.begin(); iJet < GoodJets.end(); iJet++) {
                    math::XYZTLorentzVector jet(this->TheTree->JPx[*iJet], this->TheTree->JPy[*iJet], this->TheTree->JPz[*iJet], this->TheTree->JE[*iJet]);
                    this->TheEvent.Jets.push_back(jet);
                }
                TheEvent.NJets = GoodJets.size();

                this->TheEvent.CaloMET.SetCoordinates(TheTree->MuJESCorrMETpx, TheTree->MuJESCorrMETpy, 0);
                this->TheEvent.PFMET.SetCoordinates(TheTree->PFMETpx, TheTree->PFMETpy, 0);

                this->TheEvent.RunID = TheTree->Run;
                this->TheEvent.EventNumber = TheTree->Event;

                double invMass = this->TheEvent.GetInvMass(Electron::_Momentum);

                //                cout << "invMass:" << invMass << std::endl;
                if (TheEvent.IsSameFlavour()) {
                    BASE::FillAllValue(BASE::EventSelectionHistos.at(3));
                    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 0);

                    double invMassCut = 12.0;
                    if (this->Config->LooseSelection)
                        invMassCut = 5.0;
                    if ((((SSSusySelectorConfig*) Config)->RejectInvMassLessThan12) ? invMass > invMassCut : true) {
                        BASE::FillAllValue(BASE::EventSelectionHistos.at(4));
                        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 1);

                        if (((SSSusySelectorConfig*) Config)->RejectZRegion ? (invMass < 76.0 || invMass > 104) : true) {
                            BASE::FillAllValue(BASE::EventSelectionHistos.at(5));
                            BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 2);

                            if (GoodJets.size() >= ((SSSusySelectorConfig*) Config)->NJets) {
                                BASE::FillAllValue(BASE::EventSelectionHistos.at(6));
                                BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 3);

                                if (TheTree->PFMET > ((SSSusySelectorConfig*) Config)->METCutSF) {
                                    BASE::FillAllValue(BASE::EventSelectionHistos.at(7));
                                    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 4);


                                    if (HTReader.ReadValue(&TheEvent) > ((SSSusySelectorConfig*) Config)->HTCut) {
                                        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 8);

                                        double evtType = EventTypeReader.ReadValue(&TheEvent);
                                        if (std::find(CurrentFileAcceptedEventTypes.begin(), CurrentFileAcceptedEventTypes.end(), evtType) != CurrentFileAcceptedEventTypes.end()) {
                                            BASE::FillAllValue(BASE::EventSelectionHistos.at(11));
                                            BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 9);

                                            stat = 1;
                                            RunEventNumbers.insert(std::pair<int, int>(TheTree->Run, TheTree->LumiSection));
                                            return &(TheEvent);
                                        } else {
                                            stat = 1000;
                                        }
                                    } else {
                                        stat = 13;
                                    }
                                } else {
                                    stat = 12;
                                }
                            } else
                                stat = 10 + GoodJets.size(); //10 and 11
                        } else
                            stat = 9;
                    } else
                        stat = 8;
                } else if (!TheEvent.IsSameFlavour()) {
                    BASE::FillAllValue(BASE::EventSelectionHistos.at(8));
                    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 5);

                    stat = 1111;
                    if (GoodJets.size() >= ((SSSusySelectorConfig*) Config)->NJets) {
                        BASE::FillAllValue(BASE::EventSelectionHistos.at(9));
                        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 6);

                        if (TheTree->PFMET > ((SSSusySelectorConfig*) Config)->METCutOF) {
                            BASE::FillAllValue(BASE::EventSelectionHistos.at(10));
                            BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 7);

                            if (HTReader.ReadValue(&TheEvent) > ((SSSusySelectorConfig*) Config)->HTCut) {
                                BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 8);

                                double evtType = EventTypeReader.ReadValue(&TheEvent);
                                if (std::find(CurrentFileAcceptedEventTypes.begin(), CurrentFileAcceptedEventTypes.end(), evtType) != CurrentFileAcceptedEventTypes.end()) {
                                    BASE::FillAllValue(BASE::EventSelectionHistos.at(11));
                                    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, 9);

                                    stat = 1;
                                    RunEventNumbers.insert(std::pair<int, int>(TheTree->Run, TheTree->LumiSection));
                                    return &(TheEvent);
                                } else {
                                    stat = 1000;
                                }
                            } else {
                                stat = 13;
                            }
                        } else {
                            stat = 12;
                        }
                    } else
                        stat = 10 + GoodJets.size(); //10 and 11
                }
            } else {
                if (this->GoodElecs.size() + this->GoodMuons.size() == 2)
                    stat = 6;
                else
                    stat = 7;


                if (firstLepton)
                    delete firstLepton;
                if (secondLepton)
                    delete secondLepton;
            }
        } else {
            if (GoodElecs.size() == 1)
                stat = 3;
            else if (GoodMuons.size() == 1)
                stat = 4;
            else
                stat = 5;

        }
    } else
        stat = 2;

    return NULL;
}

bool SSSusyEventSelector::CheckElectron(int eleidx, bool fillHisto) {
    bool ret = BASE::CheckElectron(eleidx, fillHisto);

    if (ret && ((SSSusySelectorConfig*)this->Config)->RejectLeptonsFromZ) {
        math::RhoEtaPhiVector tmp;

        tmp.SetCoordinates(TheTree->ElPt[eleidx]
                , TheTree->ElEta[eleidx]
                , TheTree->ElPhi[eleidx]);

        math::XYZTLorentzVector my_4vector;
        my_4vector.SetXYZT(tmp.X(), tmp.Y(), tmp.Z(), tmp.R());

        for (std::deque<int>::iterator eleit = GoodElecs.begin(); eleit != this->GoodElecs.end();) {
            int eleid = *eleit;
            tmp.SetCoordinates(TheTree->ElPt[eleid],
                    TheTree->ElEta[eleid],
                    TheTree->ElPhi[eleid]);

            math::XYZTLorentzVector ele_4vector;
            ele_4vector.SetXYZT(tmp.X(), tmp.Y(), tmp.Z(), tmp.R());


            double invMass = (ele_4vector + my_4vector).mass();

            if (invMass > 76 && invMass < 106) {
                ret = false;
                eleit = this->GoodElecs.erase(eleit);

                if (fillHisto)
                    EleSelectionHistos.FillAll(&eleid, 11);

                if (GoodElecs.size() == 0)
                    break;
            } else
                eleit++;
        }

        if (!ret && fillHisto)
            EleSelectionHistos.FillAll(&eleidx, 11);
    }

    return ret;
}

bool SSSusyEventSelector::CheckMuon(int muidx, bool fillHisto) {
    bool ret = BASE::CheckMuon(muidx, fillHisto);

    ret &= fabs(TheTree->MuD0BS[muidx]) < this->Config->MuonCuts.d0Cut;
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 7);

    double iso = (TheTree->MuIso03SumPt[muidx] + TheTree->MuIso03EmEt[muidx] + TheTree->MuIso03HadEt[muidx]);
    if (this->Config->LooseSelection)
        iso /= TheTree->MuPt[muidx];
    else
        iso /= std::max(TheTree->MuPt[muidx], 20.0f);
    ret &= (iso < this->Config->MuonCuts.isoCut);
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 8);

    if (ret && ((SSSusySelectorConfig*)this->Config)->RejectLeptonsFromZ) {
        math::RhoEtaPhiVector tmp;
        tmp.SetCoordinates(TheTree->MuPt[muidx]
                , TheTree->MuEta[muidx]
                , TheTree->MuPhi[muidx]);

        math::XYZTLorentzVector my_4vector;
        my_4vector.SetXYZT(tmp.X(), tmp.Y(), tmp.Z(), tmp.R());

        for (std::deque<int>::iterator muit = GoodMuons.begin(); muit != this->GoodMuons.end();) {
            int muid = *muit;
            tmp.SetCoordinates(TheTree->MuPt[muid],
                    TheTree->MuEta[muid],
                    TheTree->MuPhi[muid]);

            math::XYZTLorentzVector mu_4vector;
            mu_4vector.SetXYZT(tmp.X(), tmp.Y(), tmp.Z(), tmp.R());


            double invMass = (mu_4vector + my_4vector).mass();

            if (invMass > 76 && invMass < 106) {
                ret = false;
                muit = this->GoodMuons.erase(muit);

                if (fillHisto)
                    this->MuonSelectionHistos.FillAll(&muid, 9);

                //cout << "muon #" << muid << " and muon #" << muidx << " invariant mass is " << invMass << " both are rejected" << endl;

                if (GoodMuons.size() == 0)
                    break;

            } else
                muit++;
        }

        if (!ret && fillHisto)
            this->MuonSelectionHistos.FillAll(&muidx, 9);
    }

    return ret;
}

bool SSSusyEventSelector::CheckJet(int jetIdx) {
    bool ret = BASE::CheckJet(jetIdx);

    return ret;
}

void SSSusyEventSelector::AddSelectionStepsElectron() {
    BASE::AddSelectionStepsElectron();

    this->EleSelectionHistos.CreateHistos("ElectronsFromZ");
    this->EleSelectionHistos.CreateHistos("GoodElectrons");
}

void SSSusyEventSelector::AddSelectionStepsMuon() {
    BASE::AddSelectionStepsMuon();

    this->MuonSelectionHistos.CreateHistos("D0");
    this->MuonSelectionHistos.CreateHistos("Iso");
    this->MuonSelectionHistos.CreateHistos("MuonFromZ");
    this->MuonSelectionHistos.CreateHistos("GoodMuons");
}

void SSSusyEventSelector::AddSelectionStepsEvent() {
    BASE::EventSelectionHistos.CreateHistos("All");
    BASE::EventSelectionHistos.CreateHistos("AtLeast2Leptons");
    BASE::EventSelectionHistos.CreateHistos("FirstLeptonPt");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_InvMass12");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_InvMassZ");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_NJets");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_MET");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_NJets");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_MET");
    BASE::EventSelectionHistos.CreateHistos("AllSelectedEvents");

    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_InvMass12");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_InvMassZ");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_NJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_MET");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_NJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_MET");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("HT");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllSelectedEvents");
}

void SSSusyEventSelector::AddSelectionPlotsEvent() {
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

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::EventType<EVENTTYPE>());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::HT < 0 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::HT < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::HT < 2 > ());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::MHT < 0 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::MHT < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::MHT < 2 > ());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::InvariantMass<ElectronAnalysis::Electron::_Momentum > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::NumberOfJets<EVENTTYPE>());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::PFMet<EVENTTYPE>());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::LeptonEta < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::LeptonEta < 2 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::LeptonPt < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::LeptonPt < 2 > ());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::JetPt < 1 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::JetPt < 2 > ());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new ElectronAnalysis::DiLeptonEventProperties::JetPt < 3 > ());

}

void SSSusyEventSelector::AddTriggerHistos() {
    ObjectPropertyFromFltArray::ObjectInfo elPtInf("Pt", "P_{t}", 10, 200, 50, "Electron", 0);
    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    info::TheInfo->triggerCuts.AddHistogram(ElePt);

    ObjectPropertyFromIntArray::ObjectInfo elN("NElecs", "Number of electrons", 0, 20, 20, "Event", 0);
    ObjectPropertyFromIntArray* EleN = new ObjectPropertyFromIntArray(&(this->TheTree->NEles), elN);
    info::TheInfo->triggerCuts.AddHistogram(EleN);
}

void SSSusyEventSelector::OnChangeFile(FileInfo* fi, DataSet* ds) {
    if (((SSSusySelectorConfig*) Config)->SelectedEventTypesByDSName.existsAs< std::vector<double> >(fi->Name))
        CurrentFileAcceptedEventTypes = ((SSSusySelectorConfig*) Config)->SelectedEventTypesByDSName.getParameter< std::vector<double> >(fi->Name);
    else {
        CurrentFileAcceptedEventTypes.clear();
        for (double dd = 0.25; dd < 4.0; dd += 0.5)
            CurrentFileAcceptedEventTypes.push_back(dd);
        std::cout << "All of the events from this file is accepted" << std::endl;
    }
}

string GetStatusString(int stat) {
    switch (stat) {
        case 1:
            return "OK";
        case 2:
            return "!OK, Bad vertex";
        case 3:
            return "!OK, Just one electron";
        case 4:
            return "!OK, Just one muon";
        case 5:
            return "!OK, no lepton";
        case 6:
            return "!OK, Two leptons with pt < HighLeptonPt";
        case 7:
            return "!OK, More than two leptons with pt < HighLeptonPt";
        case 8:
            return "!OK, Inv mass < 12";
        case 9:
            return "!OK, Inv mass in the z region";
        case 10:
            return "!OK, No Jet";
        case 11:
            return "!OK, Just one jet";
        case 12:
            return "!OK, MET";
        case 13:
            return "!OK, HT";
        case 25:
            return "!OK, Trigger";
        case 100:
            return "!OK, user analyzer rejected";
        case 1000:
            return "!OK, event type should be rejected";
        case 101:
            return "!OK, this event is already analyzed and accepted";
    }

    return "No such status is defined";
}

void SSSusyEventSelector::AddSelectionPlotsElectron() {
    BASE::AddSelectionPlotsElectron();

    ObjectPropertyFromFltArray::ObjectInfo elCaloEtInf("CaloEt", "Calo Energy", 10, 200, 50, "Electron", 10);
    ObjectPropertyFromFltArray* EleCaloEt = new ObjectPropertyFromFltArray(this->TheTree->ElCaloEnergy, &(this->TheTree->NEles), elCaloEtInf);
    this->EleSelectionHistos.AddHisto1ToAll(EleCaloEt);
}
