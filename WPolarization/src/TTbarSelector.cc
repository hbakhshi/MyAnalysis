#include <deque>
#include <stdio.h>

#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "WPolarization/interface/TTbarSelector.h"
#include "base/Configuration.h"
#include "boost/detail/container_fwd.hpp"
#include "../interface/setTDRStyle.h"

TTbarEventSelector::TTbarEventSelector(TreeReader* tree, TTbarSelectorConfig* config, map<string, bool>* triggerResults) : BASE(tree, config, triggerResults),
hBTagScaleFactors("hBTagScaleFactors", "hBTagScaleFactors", 1000, 0.0, 2.0),
BTagWeighter(config->BTagScaleFactorSystematics) {
    
    setTDRStyle();
    
    TheEvent.FirstElectron = NULL;
    TheEvent.SecondElectron = NULL;

    if (!config->Data) {
        TFile* f = TFile::Open(config->PUWFiles.c_str(), "READ");
        TH1F h_tmp(*((TH1F*) f->Get("pileup")));
        //h_tmp.Print("ALL");
        f->Close();
        h_tmp.Copy(this->hPileUpWeights);
        //hPileUpWeights.Print("ALL");
        //delete f;
    }

    switch (config->getBTagAlgo()) {
        case 1:
            btag_1 = new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 1, 1 > ::type();
            btag_2 = new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 2, 1 > ::type();
            break;
        case 2:
            btag_1 = new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 1, 2 > ::type();
            btag_2 = new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 2, 2 > ::type();
            break;
    }

    InvariantMass_Prop.NumberOfBins *= 5;
    hInvMassEEvsNBJets = InvariantMass_Prop.GetH2(&NumberOfBJets_Prop, "EE", "");
    hInvMassMMvsNBJets = InvariantMass_Prop.GetH2(&NumberOfBJets_Prop, "MM", "");
    hInvMassEMvsNBJets = InvariantMass_Prop.GetH2(&NumberOfBJets_Prop, "EM", "");

    hInvMassEEvsNBJets_NoW = InvariantMass_Prop.GetH2(&NumberOfBJets_Prop, "NoW_EE", "");
    hInvMassMMvsNBJets_NoW = InvariantMass_Prop.GetH2(&NumberOfBJets_Prop, "NoW_MM", "");
    hInvMassEMvsNBJets_NoW = InvariantMass_Prop.GetH2(&NumberOfBJets_Prop, "NoW_EM", "");

    hInvMassEEvsMET = InvariantMass_Prop.GetH2(&MET_Prop, "EE", "");
    hInvMassMMvsMET = InvariantMass_Prop.GetH2(&MET_Prop, "MM", "");
    hInvMassEMvsMET = InvariantMass_Prop.GetH2(&MET_Prop, "EM", "");

    hInvMassEEvsMET_NoW = InvariantMass_Prop.GetH2(&MET_Prop, "NoW_EE", "");
    hInvMassMMvsMET_NoW = InvariantMass_Prop.GetH2(&MET_Prop, "NoW_MM", "");
    hInvMassEMvsMET_NoW = InvariantMass_Prop.GetH2(&MET_Prop, "NoW_EM", "");

    hEventSelectionMuonMuon.SetNameTitle("hEventSelectionMM", "Event Selection #mu #mu");
    hEventSelectionMuonMuon.SetBins(9, 1, 10);
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(1, "ALL");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(2, "Trigger");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(3, "Cleaning");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(4, "PairChoose");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(5, "InvMass12");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(6, "InvMassZ");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(7, "Njets");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(8, "MET");
    hEventSelectionMuonMuon.GetXaxis()->SetBinLabel(9, "NBjets");

    hEventSelectionMuonMuon.Copy(hEventSelectionMuonMuonW);
    hEventSelectionMuonMuonW.SetNameTitle("hEventSelectionMMW", "Event Selection #mu #mu, weighted");

    hEventSelectionMuonMuon.Copy(hEventSelectionElectronElectron);
    hEventSelectionElectronElectron.SetNameTitle("hEventSelectionEE", "Event Selection e-e");

    hEventSelectionMuonMuon.Copy(hEventSelectionElectronElectronW);
    hEventSelectionElectronElectronW.SetNameTitle("hEventSelectionEEW", "Event Selection e-e, weighted");


    hEventSelectionElectronMuon.SetNameTitle("hEventSelectionEM", "Event Selection, e #mu");
    hEventSelectionElectronMuon.SetBins(7, 1, 8);
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(1, "ALL");
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(2, "Trigger");
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(3, "Cleaning");
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(4, "PairChoose");
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(5, "Njets");
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(6, "MET");
    hEventSelectionElectronMuon.GetXaxis()->SetBinLabel(7, "NBjets");

    hEventSelectionElectronMuon.Copy(hEventSelectionElectronMuonW);
    hEventSelectionElectronMuonW.SetNameTitle("hEventSelectionEMW", "Event Selection, e #mu, weighted");

    hEventSelectionElectronMuon.Copy(hEventSelectionDiLepton);
    hEventSelectionDiLepton.SetNameTitle("hEventSelectionDiLepton", "Event Selection, lepton-lepton");

    hEventSelectionElectronMuon.Copy(hEventSelectionDiLeptonW);
    hEventSelectionDiLeptonW.SetNameTitle("hEventSelectionDiLeptonW", "Event Selection, lepton-lepton, weighted");

    if (((TTbarSelectorConfig*)this->Config)->ApplyJES != 0) {
        JetCorrectorParameters *p = new JetCorrectorParameters(((TTbarSelectorConfig*)this->Config)->JESFileName, "Total");
        JUnc = new JetCorrectionUncertainty(*p);
    }
}

TTBarDileptonicEvent* TTbarEventSelector::Read(int& stat) {

    if (!this->Config->CheckRunEventID(TheTree->Run, TheTree->Event)) {
        stat = 101;
        return NULL;
    }

    double event_weight = 1.0;
    if (!(((TTbarSelectorConfig*)this->Config)->Data)) {
        int nPUs = this->TheTree->PUnumInteractions;

        //cout << nPUs << endl;
        //this->hPileUpWeights.Print("ALL");

        int nBin = this->hPileUpWeights.GetXaxis()->FindBin(nPUs);
        event_weight = this->hPileUpWeights.GetBinContent(nBin);
    }
    this->TheEvent.PUnumInteractions = this->TheTree->PUnumInteractions;
    this->TheEvent.NPrimaryVertices = this->TheTree->NVrtx;

    this->TheEvent.PDFID1 = this->TheTree->PDFID1;
    this->TheEvent.PDFID2 = this->TheTree->PDFID2;
    this->TheEvent.PDFScalePDF = this->TheTree->PDFScalePDF;
    this->TheEvent.PDFx1 = this->TheTree->PDFx1;
    this->TheEvent.PDFx2 = this->TheTree->PDFx2;
    this->TheEvent.PDFxPDF1 = this->TheTree->PDFxPDF1;
    this->TheEvent.PDFxPDF2 = this->TheTree->PDFxPDF2;
    
    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelector::TTbarEventSelectionSteps_All));

    hEventSelectionElectronElectron.Fill(1);
    hEventSelectionElectronElectronW.Fill(1, event_weight);
    hEventSelectionMuonMuon.Fill(1);
    hEventSelectionMuonMuonW.Fill(1, event_weight);
    hEventSelectionElectronMuon.Fill(1);
    hEventSelectionElectronMuonW.Fill(1, event_weight);
    hEventSelectionDiLepton.Fill(1);
    hEventSelectionDiLeptonW.Fill(1, event_weight);

    bool Trigger = this->Config->TriggerNames.size() == 0;
    for (vector<string>::iterator trg = this->Config->TriggerNames.begin(); trg != Config->TriggerNames.end(); trg++) {
        map<string, bool>::const_iterator res = TriggerResults->find(*trg);
        if (res == TriggerResults->end())
            throw SelectionException("TTbarEventSelector::Read ==> Trigger " + *trg + " not found");

        Trigger = (Trigger || res->second);
    }

    if (!Trigger) {
        stat = 25;
        return NULL;
    }

    for (int __DecayMode = 0; __DecayMode < 3; __DecayMode++) {
        vector<string>* accept_triggers(NULL);
        vector<string>* veto_triggers(NULL);

        switch (__DecayMode) {
            case 0:
                accept_triggers = &(((TTbarSelectorConfig*) Config)->DiElectronTrigger);
                veto_triggers = &(((TTbarSelectorConfig*) Config)->DiElectronTrigger_Veto);
                break;
            case 1:
                accept_triggers = &(((TTbarSelectorConfig*) Config)->DiMuonTrigger);
                veto_triggers = &(((TTbarSelectorConfig*) Config)->DiMuonTrigger_Veto);
                break;
            case 2:
                accept_triggers = &(((TTbarSelectorConfig*) Config)->ElectronMuonTrigger);
                veto_triggers = &(((TTbarSelectorConfig*) Config)->ElectronMuonTrigger_Veto);
                break;
        }

        Trigger = (accept_triggers->size() == 0);
        for (vector<string>::iterator trg = accept_triggers->begin(); trg != accept_triggers->end(); trg++) {
            map<string, bool>::const_iterator res = TriggerResults->find(*trg);
            if (res == TriggerResults->end())
                throw SelectionException("TTbarEventSelector::Read ==> Trigger " + *trg + " not found", __LINE__, __FILE__);

            Trigger = (Trigger || res->second);
        }

        if (Trigger) {
            for (vector<string>::iterator trg = veto_triggers->begin(); trg != veto_triggers->end(); trg++) {
                map<string, bool>::const_iterator res = TriggerResults->find(*trg);
                if (res == TriggerResults->end())
                    throw SelectionException("TTbarEventSelector::Read ==> Trigger " + *trg + " not found", __LINE__, __FILE__);

                Trigger = (Trigger && !(res->second));
            }
        }

        switch (__DecayMode) {
            case 0:
                TheEvent.TRG_DiEle = Trigger;
                break;
            case 1:
                TheEvent.TRG_DiMuon = Trigger;
                break;
            case 2:
                TheEvent.TRG_EleMuon = Trigger;
                break;
        }
    }
    TheEvent.AllSolutions.clear();

    if (((TTbarSelectorConfig*) BASE::Config)->IsTTBarSample) {
        //cout << "gen is going to be set" ;
        this->SetGenTops();
        //cout << "it is set ";
        if (((TTbarSelectorConfig*) BASE::Config)->MCStudy){
            this->TheEvent.Weight = 1.0;
            return &TheEvent;
        }
    } else
        TheEvent.hasGenInfo = false;

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


    if (((TTbarSelectorConfig*)this->Config)->ApplyJES != 0) {
        this->ApplyJES(((TTbarSelectorConfig*)this->Config)->ApplyJES);
    }
    FillGoodJets();
    k = 0;
    for (int i = 0; i < GoodJets.size(); i++) {
        k = GoodJets[i];
        this->JetSelectionHistos.FillAll(&k, this->JetSelectionHistos.size() - 2);
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

    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelector::TTbarEventSelectionSteps_Trigger));

    hEventSelectionElectronElectron.Fill(2);
    hEventSelectionElectronElectronW.Fill(2, event_weight);
    hEventSelectionMuonMuon.Fill(2);
    hEventSelectionMuonMuonW.Fill(2, event_weight);
    hEventSelectionElectronMuon.Fill(2);
    hEventSelectionElectronMuonW.Fill(2, event_weight);
    hEventSelectionDiLepton.Fill(2);
    hEventSelectionDiLeptonW.Fill(2, event_weight);

    if (((TTbarSelectorConfig*)this->Config)->Data)
        if (TheTree->PrimVtxIsFake == 1 || TheTree->PrimVtxNdof < 5 || fabs(TheTree->PrimVtxz) > 24 || TheTree->PrimVtxRho > 2.0) {
            stat = 2;
            return NULL;
        }
    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelector::TTbarEventSelectionSteps_Cleaning));

    hEventSelectionElectronElectron.Fill(3);
    hEventSelectionElectronElectronW.Fill(3, event_weight);
    hEventSelectionMuonMuon.Fill(3);
    hEventSelectionMuonMuonW.Fill(3, event_weight);
    hEventSelectionElectronMuon.Fill(3);
    hEventSelectionElectronMuonW.Fill(3, event_weight);
    hEventSelectionDiLepton.Fill(3);
    hEventSelectionDiLeptonW.Fill(3, event_weight);

    if (!((this->GoodElecs.size() + this->GoodMuons.size()) > 1)) {
        if (GoodElecs.size() == 1)
            stat = 3;
        else if (GoodMuons.size() == 1)
            stat = 4;
        else
            stat = 5;

        return NULL;
    }


    //start choosing the lepton pair. the charge of them is important to be checked
    int nNeg = 0;
    int nPos = 0;

    std::multimap<float, int, greater_equal<float> > lepton_pts;
    std::map<int, int> lepton_charges;
    for (int ii = 0; ii < GoodElecs.size(); ii++) {
        lepton_pts.insert(std::pair<float, int>(TheTree->ElPt[GoodElecs.at(ii)], ii));
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


    if (nNeg == 0 || nPos == 0) {
        stat = 6; //SS
        return NULL;
    }

    Lepton* firstLepton;
    Lepton* secondLepton;

    multimap<float, int, greater_equal<float> >::const_iterator itr_1 = lepton_pts.begin();
    int chargeOfHighestPtLep = lepton_charges[itr_1->second];

    multimap<float, int, greater_equal<float> >::const_iterator itr_2;

    multimap<float, int, greater_equal<float> >::const_iterator itr = lepton_pts.begin();
    for (; itr != lepton_pts.end(); itr++)
        if (lepton_charges[itr->second] != chargeOfHighestPtLep) {
            itr_2 = itr;
            break;
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

    if (this->TheEvent.FirstElectron != NULL) {
        delete this->TheEvent.FirstElectron;
        this->TheEvent.FirstElectron = NULL;
    }
    if (this->TheEvent.SecondElectron != NULL) {
        delete this->TheEvent.SecondElectron;
        this->TheEvent.SecondElectron = NULL;
    }

    //    this->TheEvent.FirstElectron = firstLepton;
    //    this->TheEvent.SecondElectron = secondLepton;
    this->TheEvent.SetLeptons(firstLepton, secondLepton);

    this->TheEvent.CurrentEntryInTree = TheTree->current_entry;


    this->TheEvent.Weight = event_weight;

    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_PairChoose));

    switch (TheEvent.RecDecayMode) {
        case TopAnalysis::TTBarDileptonicEvent::DiEle:
            hEventSelectionElectronElectron.Fill(4);
            hEventSelectionElectronElectronW.Fill(4, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::DiMu:
            hEventSelectionMuonMuon.Fill(4);
            hEventSelectionMuonMuonW.Fill(4, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
        case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
            hEventSelectionElectronMuon.Fill(4);
            hEventSelectionElectronMuonW.Fill(4, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::EE_Inconsistent_With_Trigger:
            stat = 15;
            return NULL;
        case TopAnalysis::TTBarDileptonicEvent::EM_Inconsistent_With_Trigger:
            stat = 17;
            return NULL;
        case TopAnalysis::TTBarDileptonicEvent::MM_Inconsistent_With_Trigger:
            stat = 16;
            return NULL;
    }

    this->TheEvent.Jets.clear();
    this->TheEvent.BJets.clear();
    this->TheEvent.Jets_TrackCountingHighEff.clear();
    this->TheEvent.Jets_simpleSecondaryVertexHighEff.clear();
    for (deque<int>::const_iterator iJet = GoodJets.begin(); iJet < GoodJets.end(); iJet++) {
        math::XYZTLorentzVector jet(this->TheTree->JPx[*iJet], this->TheTree->JPy[*iJet], this->TheTree->JPz[*iJet], this->TheTree->JE[*iJet]);
        double dr1 = reco::deltaR(jet.eta(), jet.phi(), firstLepton->get4Vector(0).eta(), firstLepton->get4Vector(0).phi());
        double dr2 = reco::deltaR(jet.eta(), jet.phi(), secondLepton->get4Vector(0).eta(), secondLepton->get4Vector(0).phi());

        if (dr1 > ((TTbarSelectorConfig*) Config)->DRJetsLeptons &&
                dr2 > ((TTbarSelectorConfig*) Config)->DRJetsLeptons) {
            k = *iJet;
            this->JetSelectionHistos.FillAll(&k, this->JetSelectionHistos.size() - 1);

            this->TheEvent.Jets.push_back(jet);
            this->TheEvent.Jets_TrackCountingHighEff.push_back(this->TheTree->JbTagProbTkCntHighEff[*iJet]);
            this->TheEvent.Jets_simpleSecondaryVertexHighEff.push_back(this->TheTree->JbTagProbSimpSVHighEff[*iJet]);

            switch (((TTbarSelectorConfig*) Config)->getBTagAlgo()) {
                case 1:
                    if (TheTree->JbTagProbTkCntHighEff[*iJet] > ((TTbarSelectorConfig*) Config)->BJetSelectionBTag)
                        this->TheEvent.BJets.push_back(this->TheEvent.Jets.size() - 1);
                    break;
                case 2:
                    if (TheTree->JbTagProbSimpSVHighEff[*iJet] > ((TTbarSelectorConfig*) Config)->BJetSelectionBTag)
                        this->TheEvent.BJets.push_back(this->TheEvent.Jets.size() - 1);
                    break;
            }
        }
    }
    //cout << TheEvent.BJets.size() << "  " <<  ((TTbarSelectorConfig*) Config)->NBJets << endl;
    TheEvent.NJets = this->TheEvent.Jets.size();

    this->TheEvent.CaloMET.SetCoordinates(TheTree->MuJESCorrMETpx, TheTree->MuJESCorrMETpy, 0);
    this->TheEvent.PFMET.SetCoordinates(TheTree->PFMETpx, TheTree->PFMETpy, 0);

    this->TheEvent.RunID = TheTree->Run;
    this->TheEvent.EventNumber = TheTree->Event;

    double invMass = this->TheEvent.GetInvMass(Electron::_Momentum);

    //                cout << "invMass:" << invMass << std::endl;
    double METCutValue = 0.0;

    if (TheEvent.IsSameFlavour()) {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

        METCutValue = ((TTbarSelectorConfig*) Config)->METCutSF;
    } else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_OppositeFlavours));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_OppositeFlavours - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

        METCutValue = ((TTbarSelectorConfig*) Config)->METCutOF;
    }

    double invMassCut = 12.0;
    if (TheEvent.IsSameFlavour()) {
        if (((((TTbarSelectorConfig*) Config)->RejectInvMassLessThan12) ? invMass < invMassCut : false)) {
            stat = 8;
            return NULL;
        }
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_InvMass12));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_InvMass12 - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

        switch (TheEvent.RecDecayMode) {
            case TopAnalysis::TTBarDileptonicEvent::DiEle:
                hEventSelectionElectronElectron.Fill(5);
                hEventSelectionElectronElectronW.Fill(5, event_weight);
                break;
            case TopAnalysis::TTBarDileptonicEvent::DiMu:
                hEventSelectionMuonMuon.Fill(5);
                hEventSelectionMuonMuonW.Fill(5, event_weight);
                break;
            case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
            case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
                hEventSelectionElectronMuon.Fill(100);
                hEventSelectionElectronMuonW.Fill(100, event_weight);
                break;
        }

        if (((((TTbarSelectorConfig*) Config)->RejectZRegion) ? (invMass > 76.0 && invMass < 106.0) : false)) {
            stat = 9;
            if (TheEvent.NJets >= ((TTbarSelectorConfig*) Config)->NJets)
                switch (TheEvent.RecDecayMode) {
                    case TopAnalysis::TTBarDileptonicEvent::DiEle:
                        hInvMassEEvsNBJets->Fill(&TheEvent, event_weight);
                        hInvMassEEvsNBJets_NoW->Fill(&TheEvent);

                        hInvMassEEvsMET->Fill(&TheEvent, event_weight);
                        hInvMassEEvsMET_NoW->Fill(&TheEvent);
                        break;
                    case TopAnalysis::TTBarDileptonicEvent::DiMu:
                        hInvMassMMvsNBJets->Fill(&TheEvent, event_weight);
                        hInvMassMMvsNBJets_NoW->Fill(&TheEvent);

                        hInvMassMMvsMET->Fill(&TheEvent, event_weight);
                        hInvMassMMvsMET_NoW->Fill(&TheEvent);
                        break;
                    case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
                    case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
                        hInvMassEMvsNBJets->Fill(&TheEvent, event_weight);
                        hInvMassEMvsNBJets_NoW->Fill(&TheEvent);

                        hInvMassEMvsMET->Fill(&TheEvent, event_weight);
                        hInvMassEMvsMET_NoW->Fill(&TheEvent);
                        break;
                }


            return NULL;
        }
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_InvMassZ));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_InvMassZ - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

        switch (TheEvent.RecDecayMode) {
            case TopAnalysis::TTBarDileptonicEvent::DiEle:
                hEventSelectionElectronElectron.Fill(6);
                hEventSelectionElectronElectronW.Fill(6, event_weight);
                break;
            case TopAnalysis::TTBarDileptonicEvent::DiMu:
                hEventSelectionMuonMuon.Fill(6);
                hEventSelectionMuonMuonW.Fill(6, event_weight);
                break;
            case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
            case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
                hEventSelectionElectronMuon.Fill(100);
                hEventSelectionElectronMuonW.Fill(100, event_weight);
                break;
        }
    }

    hEventSelectionDiLepton.Fill(4);
    hEventSelectionDiLeptonW.Fill(4, event_weight);
    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_AllFlavours_PairChoose));
    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_AllFlavours_PairChoose - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

    if (TheEvent.NJets < ((TTbarSelectorConfig*) Config)->NJets) {
        stat = 10 + TheEvent.NJets; //10 and 11
        return NULL;
    }

    if (TheEvent.IsSameFlavour()) {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_NJets));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_NJets - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    } else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_OppositeFlavours_NJets));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_OppositeFlavours_NJets - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    }
    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_AllFlavours_NJets));
    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_AllFlavours_NJets - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

    switch (TheEvent.RecDecayMode) {
        case TopAnalysis::TTBarDileptonicEvent::DiEle:
            hEventSelectionElectronElectron.Fill(7);
            hEventSelectionElectronElectronW.Fill(7, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::DiMu:
            hEventSelectionMuonMuon.Fill(7);
            hEventSelectionMuonMuonW.Fill(7, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
        case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
            hEventSelectionElectronMuon.Fill(5);
            hEventSelectionElectronMuonW.Fill(5, event_weight);
            break;
    }
    hEventSelectionDiLepton.Fill(5);
    hEventSelectionDiLeptonW.Fill(5, event_weight);

    if (TheTree->PFMET < METCutValue) {
        stat = 12;
        return NULL;
    }
    if (TheEvent.IsSameFlavour()) {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_MET));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_MET - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    } else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_OppositeFlavours_MET));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_OppositeFlavours_MET - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    }
    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_AllFlavours_MET));
    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_AllFlavours_MET - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);

    switch (TheEvent.RecDecayMode) {
        case TopAnalysis::TTBarDileptonicEvent::DiEle:
            hEventSelectionElectronElectron.Fill(8);
            hEventSelectionElectronElectronW.Fill(8, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::DiMu:
            hEventSelectionMuonMuon.Fill(8);
            hEventSelectionMuonMuonW.Fill(8, event_weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
        case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
            hEventSelectionElectronMuon.Fill(6);
            hEventSelectionElectronMuonW.Fill(6, event_weight);
            break;
    }
    hEventSelectionDiLepton.Fill(6);
    hEventSelectionDiLeptonW.Fill(6, event_weight);

    if (btag_1->ReadValue(&TheEvent) < ((TTbarSelectorConfig*) Config)->BTag1) {
        stat = 7;
        return NULL;
    }
    if (TheEvent.IsSameFlavour()) {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_bTag1));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_bTag1 - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    } else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_OppositeFlavours_bTag1));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_OppositeFlavours_bTag1 - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    }

    if (btag_2->ReadValue(&TheEvent) < ((TTbarSelectorConfig*) Config)->BTag2) {
        stat = 13;
        return NULL;
    }
    if (TheEvent.IsSameFlavour()) {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_bTag2));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_bTag2 - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    } else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_OppositeFlavours_bTag2));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_OppositeFlavours_bTag2 - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    }

    int nBjets(TheEvent.BJets.size());
    if (nBjets < ((TTbarSelectorConfig*) Config)->NBJets) {
        stat = 14;
        return NULL;
    }

    if (!(((TTbarSelectorConfig*)this->Config)->Data) && ((TTbarSelectorConfig*) Config)->NBJets > 0) {
        vector< vector<BTagWeight::JetInfo> > jInfosToReWeight(TheEvent.Jets.size());
        if (((TTbarSelectorConfig*)this->Config)->getBTagAlgo() == 1) {
            double sf, eff;
            for (int iJet = 0; iJet < TheEvent.Jets.size(); iJet++) {
                BTagWeight::GetEffSF_TCHEL(TheEvent.Jets[iJet].Pt(), TheEvent.Jets[iJet].Eta(), TheEvent.Jets_TrackCountingHighEff[iJet], eff, sf , BTagWeighter.Systematics);
                BTagWeight::JetInfo jinfo(eff, sf);
                jInfosToReWeight[iJet].push_back(jinfo);
            }
        } else if (((TTbarSelectorConfig*)this->Config)->getBTagAlgo() == 2) {
            double sf, eff;
            for (int iJet = 0; iJet < TheEvent.Jets.size(); iJet++) {
                BTagWeight::GetEffSF_SSVHEM(TheEvent.Jets[iJet].Pt(), TheEvent.Jets[iJet].Eta(), TheEvent.Jets_simpleSecondaryVertexHighEff[iJet], eff, sf , BTagWeighter.Systematics);
                BTagWeight::JetInfo jinfo(eff, sf);
                jInfosToReWeight[iJet].push_back(jinfo);
            }
        } else
            throw SelectionException("the sf and eff for this btagging is not implemented", __LINE__, __FILE__);

        double bw = BTagWeighter.weight(jInfosToReWeight);
        hBTagScaleFactors.Fill(bw);
        TheEvent.Weight *= bw;
    }

    switch (TheEvent.RecDecayMode) {
        case TopAnalysis::TTBarDileptonicEvent::DiEle:
            hEventSelectionElectronElectron.Fill(9);
            hEventSelectionElectronElectronW.Fill(9, TheEvent.Weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::DiMu:
            hEventSelectionMuonMuon.Fill(9);
            hEventSelectionMuonMuonW.Fill(9, TheEvent.Weight);
            break;
        case TopAnalysis::TTBarDileptonicEvent::ElP_MuM:
        case TopAnalysis::TTBarDileptonicEvent::ElM_MuP:
            hEventSelectionElectronMuon.Fill(7);
            hEventSelectionElectronMuonW.Fill(7, TheEvent.Weight);
            break;
    }
    hEventSelectionDiLepton.Fill(7);
    hEventSelectionDiLeptonW.Fill(7, event_weight);

    if (TheEvent.IsSameFlavour()) {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_SameFlavours_NumberOfBJets));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_SameFlavours_NumberOfBJets - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    } else {
        BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_OppositeFlavours_NumberOfBJets));
        BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_OppositeFlavours_NumberOfBJets - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    }
    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_AllFlavours_NumberOfBJets));
    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_AllFlavours_NumberOfBJets - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);


    double evtType = EventTypeReader.ReadValue(&TheEvent);
    if (std::find(CurrentFileAcceptedEventTypes.begin(), CurrentFileAcceptedEventTypes.end(), evtType) == CurrentFileAcceptedEventTypes.end()) {
        stat = 1000;
        return NULL;
    }

    BASE::FillAllValue(BASE::EventSelectionHistos.at(TTbarEventSelectionSteps_AllSelectedEvents));
    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_AllSelectedEvents - TTbarEventSelectionSteps_SameFlavours, TheEvent.Weight);
    BASE::EventSelectionHistosAfterObjectCreation.FillAll(&TheEvent, TTbarEventSelectionSteps_AllSelectedEventsNoW - TTbarEventSelectionSteps_SameFlavours, 1.0);

    stat = 1;
    RunEventNumbers.insert(std::pair<int, int>(TheTree->Run, TheTree->Event));
    return &(TheEvent);

}

void TTbarEventSelector::ApplyJES(int JES) {
    for (uint jet = 0; jet < TheTree->NJets; jet++) {
        JUnc->setJetPt(TheTree->JPt[jet]);
        JUnc->setJetEta(TheTree->JEta[jet]);
        double correctionfactor = JUnc->getUncertainty(JES > 0);
        double correction = correctionfactor * TheTree->JPt[jet];
        if (JES > 0)
            TheTree->JPt[jet] += correction;
        else
            TheTree->JPt[jet] -= correction;
    }
}

void TTbarEventSelector::End(TDirectory * dir) {
    BASE::End(dir);

    dir->cd("Selection");
    hEventSelectionElectronElectron.Write();
    hEventSelectionElectronElectronW.Write();

    hEventSelectionElectronMuon.Write();
    hEventSelectionElectronMuonW.Write();

    hEventSelectionMuonMuon.Write();
    hEventSelectionMuonMuonW.Write();

    hEventSelectionDiLepton.Write();
    hEventSelectionDiLeptonW.Write();

    hBTagScaleFactors.Write();
    //    for(multimap<int, int>::const_iterator event = RunEventNumbers.begin() ; event != RunEventNumbers.end(); event++){
    //        cout << "events.push_back(" << event->second << "); //Run = " << event->first << endl;
    //    }
    hInvMassEEvsNBJets->Write();
    hInvMassMMvsNBJets->Write();
    hInvMassEMvsNBJets->Write();

    hInvMassEEvsNBJets_NoW->Write();
    hInvMassMMvsNBJets_NoW->Write();
    hInvMassEMvsNBJets_NoW->Write();

    hInvMassEEvsMET->Write();
    hInvMassMMvsMET->Write();
    hInvMassEMvsMET->Write();

    hInvMassEEvsMET_NoW->Write();
    hInvMassMMvsMET_NoW->Write();
    hInvMassEMvsMET_NoW->Write();
}

bool TTbarEventSelector::CheckElectron(int eleidx, bool fillHisto) {
    bool ret = BASE::CheckElectron(eleidx, fillHisto);

    if (ret && ((TTbarSelectorConfig*)this->Config)->RejectLeptonsFromZ) {
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

    ret &= (fabs(TheTree->ElDzPV[eleidx]) < 1.0);
    if (ret && fillHisto)
        EleSelectionHistos.FillAll(&eleidx, 12);

    return ret;
}

bool TTbarEventSelector::CheckMuon(int muidx, bool fillHisto) {
    bool ret = BASE::CheckMuon(muidx, fillHisto);

    ret &= fabs(TheTree->MuD0BS[muidx]) < this->Config->MuonCuts.d0Cut;
    ret &= fabs(TheTree->MuDzPV[muidx]) < 1.0;
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 7);

    //TheTree->PFMu2
    double iso = (TheTree->MuIso03SumPt[muidx] + TheTree->MuIso03EmEt[muidx] + TheTree->MuIso03HadEt[muidx]);
    //    if (false && this->Config->LooseSelection)
    //        iso /= TheTree->MuPt[muidx];
    //    else
    iso /= std::max(TheTree->MuPt[muidx], 20.0f);
    ret &= (iso < this->Config->MuonCuts.isoCut);
    if (ret && fillHisto)
        this->MuonSelectionHistos.FillAll(&muidx, 8);

    if (ret && ((TTbarSelectorConfig*)this->Config)->RejectLeptonsFromZ) {
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

bool TTbarEventSelector::CheckJet(int jetIdx) {
    bool ret = BASE::CheckJet(jetIdx);

    return ret;
}

void TTbarEventSelector::AddSelectionStepsJet() {

    BASE::AddSelectionStepsJet();

    this->JetSelectionHistos.CreateHistos("GoodJets_LeptonCleaning");
}

void TTbarEventSelector::AddSelectionStepsElectron() {

    BASE::AddSelectionStepsElectron();

    this->EleSelectionHistos.CreateHistos("ElectronsFromZ");
    this->EleSelectionHistos.CreateHistos("DZ");
    this->EleSelectionHistos.CreateHistos("GoodElectrons");
}

void TTbarEventSelector::AddSelectionStepsMuon() {

    BASE::AddSelectionStepsMuon();

    this->MuonSelectionHistos.CreateHistos("D0");
    this->MuonSelectionHistos.CreateHistos("Iso");
    this->MuonSelectionHistos.CreateHistos("MuonFromZ");
    this->MuonSelectionHistos.CreateHistos("GoodMuons");
}

void TTbarEventSelector::AddSelectionStepsEvent() {

    BASE::EventSelectionHistos.CreateHistos("All");
    BASE::EventSelectionHistos.CreateHistos("Trigger");
    BASE::EventSelectionHistos.CreateHistos("Cleaning");
    BASE::EventSelectionHistos.CreateHistos("PairChoose");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_InvMass12");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_InvMassZ");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_NJets");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_MET");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_bTag1");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_bTag2");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_NumberOfBJets");
    BASE::EventSelectionHistos.CreateHistos("SameFlavours_Triggers");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_NJets");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_MET");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_bTag1");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_bTag2");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_NumberOfBJets");
    BASE::EventSelectionHistos.CreateHistos("OppositeFlavours_Triggers");

    BASE::EventSelectionHistos.CreateHistos("AllFlavours_PairChoose");
    BASE::EventSelectionHistos.CreateHistos("AllFlavours_NJets");
    BASE::EventSelectionHistos.CreateHistos("AllFlavours_MET");
    BASE::EventSelectionHistos.CreateHistos("AllFlavours_NumberOfBJets");
    BASE::EventSelectionHistos.CreateHistos("AllSelctedEvents");

    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_InvMass12");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_InvMassZ");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_NJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_MET");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_bTag1");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_bTag2");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_NumberOfBJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("SameFlavours_Triggers");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_NJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_MET");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_bTag1");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_bTag2");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_NumberOfBJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("OppositeFlavours_Triggers");

    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllFlavours_PairChoose");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllFlavours_NJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllFlavours_MET");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllFlavours_NumberOfBJets");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllSelctedEvents");
    BASE::EventSelectionHistosAfterObjectCreation.CreateHistos("AllSelctedEventsNoW");
}

void TTbarEventSelector::AddSelectionPlotsEvent() {

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

    ObjectPropertyFromIntArray::ObjectInfo puNInteractions("puNInteractions", "pu - NInteractions", 0, 50, 50, "MET", 100);
    ObjectPropertyFromIntArray* PUNInteractions = new ObjectPropertyFromIntArray(&(this->TheTree->PUnumInteractions), puNInteractions);
    BASE::EventSelectionHistos.AddHisto1ToAll(PUNInteractions);

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::EventType());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* Prop_HT =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::HT < 0 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::HT < 1 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::HT < 2 > ::type());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::MHT < 0 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::MHT < 1 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::MHT < 2 > ::type());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* Prop_InvMass
            = BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::InvariantMass<ElectronAnalysis::Electron::_Momentum >::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* Prop_NJets =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::NumberOfJets());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* Prop_NBJets =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::NumberOfBJets());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* Prop_PFMet =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::PFMet());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep1_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonEta < 1 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep2_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonEta < 2 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep3_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonEta < 3 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep4_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonEta < 4 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep1_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonPt < 1 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep2_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonPt < 2 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep3_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonPt < 3 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* lep4_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonPt < 4 > ::type());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* bjet1_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::BJetPt < 1 >() );
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* bjet2_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::BJetPt < 2 >() );
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* bjet3_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::BJetPt < 3 >() );

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* jet1_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetPt < 1 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* jet2_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetPt < 2 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* jet3_pt =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetPt < 3 > ::type());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* jet1_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetEta < 1 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* jet2_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetEta < 2 > ::type());
    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* jet3_eta =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetEta < 3 > ::type());

    ObjectProperty<TopAnalysis::TTBarDileptonicEvent>* nprvtx =
            BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::NPrimaryVertices());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::LeptonsDPhi() );

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(nprvtx);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 1, 1 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 2, 1 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 3, 1 > ::type());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 1, 2 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 2, 2 > ::type());
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto1ToAll(new TopAnalysis::DiLeptonTTBarEventProperties::JetBTag < 3, 2 > ::type());

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), nprvtx);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), new TopAnalysis::DiLeptonTTBarEventProperties::LeptonsDPhi() );    
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), Prop_HT);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), Prop_InvMass);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), Prop_PFMet);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), Prop_NJets);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), Prop_NBJets);

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep1_eta);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep1_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep2_eta);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep2_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep3_eta);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep3_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep4_eta);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), lep4_pt);

    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), bjet1_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), bjet2_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), bjet3_pt);
    
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), jet1_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), jet2_pt);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), jet3_pt);
    
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), jet1_eta);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), jet2_eta);
    BASE::EventSelectionHistosAfterObjectCreation.AddHisto2ToAll(&(this->EventTypeReader), jet3_eta);
}

void TTbarEventSelector::AddTriggerHistos() {
    //    ObjectPropertyFromFltArray::ObjectInfo elPtInf("Pt", "P_{t}", 10, 200, 50, "Electron", 0);
    //    ObjectPropertyFromFltArray* ElePt = new ObjectPropertyFromFltArray(this->TheTree->ElPt, &(this->TheTree->NEles), elPtInf);
    //    info::TheInfo->triggerCuts.AddHistogram(ElePt);

    ObjectPropertyFromIntArray::ObjectInfo elN("NElecs", "Number of electrons", 0, 20, 20, "Event", 0);
    ObjectPropertyFromIntArray* EleN = new ObjectPropertyFromIntArray(&(this->TheTree->NEles), elN);
    info::TheInfo->triggerCuts.AddHistogram(EleN);
}

void TTbarEventSelector::OnChangeFile(FileInfo* fi, DataSet * ds) {
    if (((TTbarSelectorConfig*) Config)->SelectedEventTypesByDSName.existsAs< std::vector<double> >(fi->Name))
        CurrentFileAcceptedEventTypes = ((TTbarSelectorConfig*) Config)->SelectedEventTypesByDSName.getParameter< std::vector<double> >(fi->Name);
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
            return "!OK, SS dileptons";
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
        case 7:
            return "!OK, first btag";
        case 13:
            return "!OK, second btag";
        case 14:
            return "!OK, number of bjets";
        case 15:
            return "!OK, EE, but wrong trigger";
        case 16:
            return "!OK, MM, but wrong trigger";
        case 17:
            return "!OK, EM, but wrong trigger";
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

void TTbarEventSelector::AddSelectionPlotsElectron() {

    BASE::AddSelectionPlotsElectron();

    ObjectPropertyFromFltArray::ObjectInfo elCaloEtInf("CaloEt", "Calo Energy", 10, 200, 50, "Electron", 10);
    ObjectPropertyFromFltArray* EleCaloEt = new ObjectPropertyFromFltArray(this->TheTree->ElCaloEnergy, &(this->TheTree->NEles), elCaloEtInf);
    this->EleSelectionHistos.AddHisto1ToAll(EleCaloEt);
}

void TTbarEventSelector::SetGenTops() {

    int nmuons(0);
    int nelecs(0);
    int nnus(0);
    int nbs(0);
    int ntaus(0);

    int lepp_id(-1);
    int nu_id(-1);
    int b_id(-1);

    int lepm_id(-1);
    int nub_id(-1);
    int bbar_id(-1);

    for (int gen_it = 0; gen_it < this->TheTree->NGenLeptons; gen_it++) {

        if (abs(this->TheTree->GenLeptonID[gen_it]) == 13)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 24) {
                nmuons++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    lepm_id = gen_it;
                else
                    lepp_id = gen_it;
            }

        if (abs(this->TheTree->GenLeptonID[gen_it]) == 12)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 24) {
                nnus++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    nu_id = gen_it;
                else
                    nub_id = gen_it;
            }

        if (abs(this->TheTree->GenLeptonID[gen_it]) == 14)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 24) {
                nnus++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    nu_id = gen_it;
                else
                    nub_id = gen_it;
            }

        if (abs(this->TheTree->GenLeptonID[gen_it]) == 11)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 24) {
                nelecs++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    lepm_id = gen_it;
                else
                    lepp_id = gen_it;
            }

        if (abs(this->TheTree->GenLeptonID[gen_it]) == 16)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 24) {
                nnus++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    nu_id = gen_it;
                else
                    nub_id = gen_it;
            }

        if (abs(this->TheTree->GenLeptonID[gen_it]) == 15)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 24) {
                ntaus++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    lepm_id = gen_it;
                else
                    lepp_id = gen_it;
            }


        if (abs(this->TheTree->GenLeptonID[gen_it]) == 5)
            if (abs(this->TheTree->GenLeptonMID[gen_it]) == 6) {
                nbs++;
                if (this->TheTree->GenLeptonID[gen_it] > 0)
                    b_id = gen_it;
                else
                    bbar_id = gen_it;
            }
    }

    TheEvent.TOP_Gen.Clear();
    TheEvent.TOPBar_Gen.Clear();

    if (nbs != 2) { //inconsistent number of b's
        TheEvent.GenDecayMode = (nbs == 1 ? TTBarDileptonicEvent::Just1B :
                (nbs == 0 ? TTBarDileptonicEvent::ZeroBs : TTBarDileptonicEvent::ExtraBs));
        TheEvent.hasGenInfo = false;

        return;
    }

    TheEvent.TOP_Gen.b.SetPtEtaPhiM(this->TheTree->GenLeptonPt[b_id], this->TheTree->GenLeptonEta[b_id], this->TheTree->GenLeptonPhi[b_id], 4.67);
    TheEvent.TOPBar_Gen.b.SetPtEtaPhiM(this->TheTree->GenLeptonPt[bbar_id], this->TheTree->GenLeptonEta[bbar_id], this->TheTree->GenLeptonPhi[bbar_id], 4.67);

    if ((nelecs + nmuons + ntaus == 1) && (nnus == 1)) { //semi leptonic (lep == electron)
        if (lepp_id == -1) { //tbar decays to lepton
            TheEvent.TOPBar_Gen.W.lepton.SetPtEtaPhiM(this->TheTree->GenLeptonPt[lepm_id], this->TheTree->GenLeptonEta[lepm_id], this->TheTree->GenLeptonPhi[lepm_id], 0.000510);
            TheEvent.TOPBar_Gen.W.neutrino.SetPtEtaPhiM(this->TheTree->GenLeptonPt[nub_id], this->TheTree->GenLeptonEta[nub_id], this->TheTree->GenLeptonPhi[nub_id], 0.0);
        } else {
            TheEvent.TOP_Gen.W.lepton.SetPtEtaPhiM(this->TheTree->GenLeptonPt[lepp_id], this->TheTree->GenLeptonEta[lepp_id], this->TheTree->GenLeptonPhi[lepp_id], 0.000510);
            TheEvent.TOP_Gen.W.neutrino.SetPtEtaPhiM(this->TheTree->GenLeptonPt[nu_id], this->TheTree->GenLeptonEta[nu_id], this->TheTree->GenLeptonPhi[nu_id], 0.0);
        }

        if (nelecs == 1)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::ElJets;
        else if (nmuons == 1)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::MuJets;
        else if (ntaus == 1)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::TauJets;
        else
            TheEvent.GenDecayMode = TTBarDileptonicEvent::Others;
    } else if ((nelecs + nmuons + ntaus == 2) && (nnus == 2)) { //full leptonic
        TheEvent.TOPBar_Gen.W.lepton.SetPtEtaPhiM(this->TheTree->GenLeptonPt[lepm_id], this->TheTree->GenLeptonEta[lepm_id], this->TheTree->GenLeptonPhi[lepm_id], 0.000510);
        TheEvent.TOPBar_Gen.W.neutrino.SetPtEtaPhiM(this->TheTree->GenLeptonPt[nub_id], this->TheTree->GenLeptonEta[nub_id], this->TheTree->GenLeptonPhi[nub_id], 0.0);

        TheEvent.TOP_Gen.W.lepton.SetPtEtaPhiM(this->TheTree->GenLeptonPt[lepp_id], this->TheTree->GenLeptonEta[lepp_id], this->TheTree->GenLeptonPhi[lepp_id], 0.000510);
        TheEvent.TOP_Gen.W.neutrino.SetPtEtaPhiM(this->TheTree->GenLeptonPt[nu_id], this->TheTree->GenLeptonEta[nu_id], this->TheTree->GenLeptonPhi[nu_id], 0.0);

        if (this->TheTree->GenLeptonID[lepm_id] == 11 && nelecs == 1 && ntaus == 0)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::ElM_MuP;
        else if (this->TheTree->GenLeptonID[lepm_id] == 13 && nelecs == 1 && ntaus == 0)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::ElP_MuM;
        else if (nelecs == 2)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::DiEle;
        else if (nmuons == 2)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::DiMu;
        else if (ntaus == 2)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::DiTau;
        else if (ntaus == 1 && nelecs == 1)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::ElTau;
        else if (ntaus == 1 && nelecs == 0)
            TheEvent.GenDecayMode = TTBarDileptonicEvent::MuTau;
        else
            TheEvent.GenDecayMode = TTBarDileptonicEvent::Others;
    } else if ((nelecs + nmuons + ntaus == 0) && (nnus == 0)) { //full hadronic
        TheEvent.GenDecayMode = TTBarDileptonicEvent::FullHadronic;
    } else { //inconsistent number of muons and leptons
        TheEvent.GenDecayMode = TTBarDileptonicEvent::BadNu;
    }
    //cout << TheEvent.GenDecayMode ;
    TheEvent.hasGenInfo = true;
}
