/* 
 * File:   FakeOrigin.cc
 * Author: hbakhshi
 * 
 * Created on July 12, 2010, 3:23 PM
 */

#include "helper/Utilities.hh"

#include <cmath>
#include <stdlib.h>
#include <TH1.h>
#include <THStack.h>
//#include "FakeRate/FakeOrigin.h"

using namespace std;

CutInfo::CutInfo(string name, string CUT, TreeReader* theTree, string decay_name) : Name(name), TheTree(theTree), NumberOfPassed(0), NumberOfPassedBarrel(0) {
    vector<string> cuts = SplitString(CUT, "&");

    for (int i = 0; i < cuts.size(); i++) {
        string cut = cuts[i];

        vector<string> splits = SplitString(cut, "(,)");

        Cut theCut;
        theCut.objectType = "El";
        theCut.propertyName = splits[0];

        if (splits.size() > 1 && splits[1] != "*")
            theCut.minVal = atof(splits[1].c_str());

        if (splits.size() > 2 && splits[2] != "*")
            theCut.maxVal = atof(splits[2].c_str());

        this->Cuts.push_back(theCut);

    }

    allHs.push_back(this->hDRGamma = new TH1I((decay_name + "_" + name + "_hDRGamma").c_str(), "Dr with nearest #gamma", 50, 0, 0.5));
    allHs.push_back(this->hGamma_HOE = new TH1I((decay_name + "_" + name + "_hGammaHOE").c_str(), "H over E", 300, -1, 3));
    allHs.push_back(this->hDRGamma_E = new TH2I((decay_name + "_" + name + "_hDRGammaERatio").c_str(), "Dr vs energy ratio with nearest #gamma", 50, 0, 0.5, 300, 0, 3));

    allHs.push_back(this->hDRJet = new TH1I((decay_name + "_" + name + "_hDRJet").c_str(), "Dr with nearest jet", 50, 0., 0.5));
    allHs.push_back(this->hJERatio = new TH1I((decay_name + "_" + name + "_hJERatio").c_str(), "energy ratio with nearest jet", 300, 0., 3.));
    allHs.push_back(this->hDRJet_E = new TH2I((decay_name + "_" + name + "_hDRJetERatio").c_str(), "Dr vs energy ratio with nearest jet", 50, 0, 0.5, 300, 0, 3));
    allHs.push_back(this->hJet_ChrgFrc = new TH1I((decay_name + "_" + name + "_hChrgFrac").c_str(), "charge fraction of nearest jet", 150, -0.2, 1.3));
    allHs.push_back(this->hJet_ECALSze = new TH1I((decay_name + "_" + name + "_hECALSze").c_str(), "ECALSize of nearest jet", 500, 0., 0.5));
    allHs.push_back(this->hJet_HCALSze = new TH1I((decay_name + "_" + name + "_hHCALSze").c_str(), "HCALSize of nearest jet", 500, 0., 0.5));
    allHs.push_back(this->hJet_EMF = new TH1I((decay_name + "_" + name + "_hEMF").c_str(), "EMF of nearest jet", 200, 0., 1.));
    allHs.push_back(this->hJet_NAssTrk = new TH1I((decay_name + "_" + name + "_hNAssTrk").c_str(), "NAssTrk of nearest jet", 30, 0., 30.));
    allHs.push_back(this->hJet_NConsts = new TH1I((decay_name + "_" + name + "_hNConsts").c_str(), "NConsts of nearest jet", 30, 0., 30.));

    allHs.push_back(this->hEIso_ECl04 = new TH1I((decay_name + "_" + name + "_hIsoECl04").c_str(), "Isolation of the electron", 50, 0., 1.));
    allHs.push_back(this->hEIso_HCl04 = new TH1I((decay_name + "_" + name + "_hIsoHCl04").c_str(), "Isolation of the electron", 50, 0., 1.));
    allHs.push_back(this->hEIso_Trk04 = new TH1I((decay_name + "_" + name + "_hIsoTrk04").c_str(), "Isolation of the electron", 50, 0., 1.));
    allHs.push_back(this->hEIso_Rel04 = new TH1I((decay_name + "_" + name + "_hIsoTot04").c_str(), "Isolation of the electron", 50, 0., 1.));
    allHs.push_back(this->hEIso_D0 = new TH1I((decay_name + "_" + name + "_hD0").c_str(), "D0 of the electron", 100, 0., 0.2));
    allHs.push_back(this->hEIso_D0Res = new TH1I((decay_name + "_" + name + "_hD0Res").c_str(), "D0 Sig. of the electron", 100, 0., 10));

    allHs.push_back(this->hEConv_Dis = new TH1I((decay_name + "_" + name + "_hEConvDis").c_str(), "Dis_Conv. of the electron", 200, -0.1, 0.1));
    allHs.push_back(this->hEConv_DCot = new TH1I((decay_name + "_" + name + "_hEConvDCot").c_str(), "DCot_Conv. of the electron", 200, -0.1, 0.1));
    allHs.push_back(this->hEConv_NMisHit = new TH1I((decay_name + "_" + name + "_hEConvNMisHit").c_str(), "NMisHits_Conv. of the electron", 20, 0., 20));
    allHs.push_back(this->hEConv_DistDCot = new TH2I((decay_name + "_" + name + "_hEConvDistDCot").c_str(), "Conv_DistDCot. of the electron", 200, -0.1, 0.1, 200, -0.1, 0.1));

    allHs.push_back(this->hEID_DEtaClus = new TH1I((decay_name + "_" + name + "_hEIDEtaClus").c_str(), "ID. D#eta seed of the electron", 100, -3., 3.));
    allHs.push_back(this->hEID_DEtaSuper = new TH1I((decay_name + "_" + name + "_hEIDEtaSuper").c_str(), "ID. D#eta super of the electron", 100, -3., 3.));
    allHs.push_back(this->hEID_DPhiSuper = new TH1I((decay_name + "_" + name + "_hEIDPhiSuper").c_str(), "ID. D#phi super of the electron", 100, -3., 3.));
    allHs.push_back(this->hEID_DPhiClus = new TH1I((decay_name + "_" + name + "_hEIDPhiClus").c_str(), "ID. D#phi seed of the electron", 100, -3., 3.));
    allHs.push_back(this->hEID_E5E25 = new TH1I((decay_name + "_" + name + "_hE5E25").c_str(), "ID. E5/E25 of the electron", 100, 0, 1));
    allHs.push_back(this->hEID_EoverP = new TH1I((decay_name + "_" + name + "_hEoverP").c_str(), "ID. E/P of the electron", 200, 0, 2));
    allHs.push_back(this->hEID_SIeIe = new TH1I((decay_name + "_" + name + "_hSIeIe").c_str(), "ID. sigma ieta ieta of the electron", 200, -0.1, 0.1));

    allHs.push_back(this->hERatio_Gen = new TH1I((decay_name + "_" + name + "_hERatioGen").c_str(), "energy ratio with gen-match particle", 100, 0., 5));
}

bool CutInfo::Pass(int id) {
    double *fval;
    int *ival;

    bool pass = true;

    for (vector<Cut>::iterator ct = Cuts.begin(); ct != Cuts.end(); ct++) {
        if (ct->leaf == NULL) {
            if (ct->propertyName == "D0") {
                double d0 = fabs(TheTree->ElD0PV[id] / TheTree->ElD0E[id] );
                if(d0 > ct->maxVal || d0 < ct->minVal)
                    pass = false;
                continue;
            } else {
                TBranch* bbb = TheTree->fChain->GetBranch(ct->LeafName().c_str());
                TLeaf* lll = bbb->GetLeaf(ct->LeafName().c_str());
                ct->leaf = lll;
                ct->type = ct->leaf->GetTypeName();
            }
        }
        if (!strcmp(ct->type.c_str(), "Double_t")) {
            fval = (Double_t*) ct->leaf->GetValuePointer();

            if (fval[id] < ct->minVal) pass = false;
            if (fval[id] > ct->maxVal) pass = false;
        } else if (!strcmp(ct->type.c_str(), "Int_t")) {
            ival = (Int_t*) ct->leaf->GetValuePointer();

            if (ival[id] < ct->minVal) pass = false;
            if (ival[id] > ct->maxVal) pass = false;
        } else
            cout << "UNKNOWN TYPE:" << ct->LeafName() << "  " << ct->type << endl;
    }

    return pass;
}

void CutInfo::AddEle(int id) {
    bool pass = Pass(id);

    if (pass) {
        this->hEIso_ECl04->Fill(TheTree->ElDR04EcalRecHitSumEt[id] / TheTree->ElPt[id]);
        this->hEIso_HCl04->Fill(TheTree->ElDR04HcalTowerSumEt[id] / TheTree->ElPt[id]);
        this->hEIso_Trk04->Fill(TheTree->ElDR04TkSumPt[id] / TheTree->ElPt[id]);
        this->hEIso_Rel04->Fill(TheTree->ElRelIso04[id]);
        this->hEIso_D0->Fill(fabs(TheTree->ElD0PV[id]));
        this->hEIso_D0Res->Fill(fabs(TheTree->ElD0PV[id] / TheTree->ElD0E[id]));

        this->hEConv_DCot->Fill(TheTree->ElConvPartnerTrkDCot[id]);
        this->hEConv_Dis->Fill(TheTree->ElConvPartnerTrkDist[id]);
        this->hEConv_NMisHit->Fill(TheTree->ElNumberOfMissingInnerHits[id]);
        this->hEConv_DistDCot->Fill(TheTree->ElConvPartnerTrkDist[id], TheTree->ElConvPartnerTrkDCot[id]);

        this->hEID_DEtaClus->Fill( TheTree->ElDeltaEtaSeedClusterAtCalo[id]);
        this->hEID_DPhiClus->Fill( TheTree->ElDeltaPhiSeedClusterAtCalo[id]);
        this->hEID_DPhiSuper->Fill( TheTree->ElDeltaPhiSuperClusterAtVtx[id]);
        this->hEID_DEtaSuper->Fill( TheTree->ElDeltaEtaSuperClusterAtVtx[id]);
        this->hEID_E5E25->Fill( TheTree->ElE1x5[id] / TheTree->ElE5x5[id]);
        this->hEID_EoverP->Fill( TheTree->ElESuperClusterOverP[id]);
        this->hEID_SIeIe->Fill( TheTree->ElSigmaIetaIeta[id]);

        this->hERatio_Gen->Fill(TheTree->ElPt[id] / TheTree->ElGenPt[id]);

        double eta = TheTree->ElEta[id];
        double phi = TheTree->ElPhi[id];
        double pt = TheTree->ElPt[id];

        NumberOfPassed++;
        if (fabs(eta) < 1.479)
            this->NumberOfPassedBarrel++;

        int GMID = TheTree->ElGenGMID[id];
        if (this->GMIDs.count(GMID) == 0)
            this->GMIDs.insert(pair<int, int>(GMID, 1));
        else
            this->GMIDs[GMID]++;

        int j_ID = -1;
        double j_mindr = 9999.0;
        double j_eratio = 0;
        for (int j_id = 0; j_id < TheTree->NJets; j_id++) {
            double j_eta = TheTree->JEta[j_id];
            double j_phi = TheTree->JPhi[j_id];
            double j_pt = TheTree->JPt[j_id] / TheTree->JEcorr[j_id];

            if (j_pt < 20)
                continue;

            double dr = Util::GetDeltaR(eta, j_eta, phi, j_phi);
            if (dr < j_mindr) {
                j_mindr = dr;
                j_ID = j_id;
                j_eratio = j_pt / pt;
            }
        }
        if (j_ID > -1) {
            this->hDRJet->Fill(j_mindr);
            this->hDRJet_E->Fill(j_mindr, j_eratio);
            this->hJet_ChrgFrc->Fill(TheTree->JChfrac[j_ID]);
            this->hJet_EMF->Fill(TheTree->JEMfrac[j_ID]);
            this->hJet_NAssTrk->Fill(TheTree->JNAssoTracks[j_ID]);
            this->hJet_NConsts->Fill(TheTree->JNConstituents[j_ID]);
            this->hJERatio->Fill(j_eratio);
            this->hJet_ECALSze->Fill(sqrt(pow(TheTree->JPhiEMrms[j_ID], 2) + pow(TheTree->JEtaEMrms[j_ID], 2)));
            this->hJet_HCALSze->Fill(sqrt(pow(TheTree->JPhiHADrms[j_ID], 2) + pow(TheTree->JEtaHADrms[j_ID], 2)));
        }

        int g_ID = -1;
        double g_mindr = 9999.0;
        double g_eratio = 0;
        for (int g_id = 0; g_id < TheTree->NPhotons; g_id++) {
            double g_eta = TheTree->PhoEta[g_id];
            double g_phi = TheTree->PhoPhi[g_id];
            double g_pt = TheTree->PhoPt[g_id];

            if (g_pt < 15 || TheTree->PhoIsElDupl[g_id] > -1)
                continue;

            double dr = Util::GetDeltaR(eta, g_eta, phi, g_phi);
            if (dr < j_mindr) {
                g_mindr = dr;
                g_ID = g_id;
                g_eratio = g_pt / pt;
            }
        }
        if (g_ID > -1) {
            this->hDRGamma->Fill(g_mindr);
            this->hDRGamma_E->Fill(g_mindr, g_eratio);
            this->hGamma_HOE->Fill(this->TheTree->PhoHoverE[g_ID]);
        }
    }
}

DecayChainInfo::DecayChainInfo(int pid, int mid, TreeReader* theTree) : PID(pid),
MID(mid),
TheTree(theTree),
TotalNumber(0),
TotalNumberBarrel(0) {

}

void DecayChainInfo::AddCut(string name, string cuts) {
    CutInfo cut(name, cuts, this->TheTree, DecayChainInfo::fPDGMap[this->PID].get_name() + "_" + DecayChainInfo::fPDGMap[this->MID].get_name());
    this->Cuts.push_back(cut);
}

bool DecayChainInfo::AddEle(int id) {
    int e_id = fabs(this->TheTree->ElGenID[id]);
    int e_mid = fabs(this->TheTree->ElGenMID[id]);

    if ((e_id == this->PID) && (e_mid == this->MID)) {
        this->TotalNumber++;

        if (fabs(TheTree->ElEta[id]) < 1.479)
            this->TotalNumberBarrel++;

        int GMID = fabs(TheTree->ElGenGMID[id]);
        if (this->GMIDs.count(GMID) == 0)
            this->GMIDs.insert(pair<int, int>(GMID, 1));
        else
            this->GMIDs[GMID]++;

        for (vector<CutInfo>::iterator cut = Cuts.begin(); cut != Cuts.end(); cut++) {
            cut->AddEle(id);
        }

        return true;
    }

    return false;
}

map<int, pdgparticle> DecayChainInfo::fPDGMap;

void DecayChainInfo::ReadPDGTable(const char* filename) {
    // Fills the fPDGMap map from a textfile to associate pdgids with names
    int pdgid(0), type(0);
    string Name, Texname, Typename;
    ifstream IN(filename);
    char buff1[200], buff2[200], buff3[200];
    char readbuff[200];
    // Loop over lines of datafile
    while (IN.getline(readbuff, 200, '\n')) {
        if (readbuff[0] == '#') {
            continue;
        } // Skip lines commented with '#'
        sscanf(readbuff, "%d %s %d %s %s", &type, buff1, &pdgid, buff2, buff3);
        // Convert chararrays to strings
        Typename = string(buff1);
        Name = string(buff2);
        Texname = string(buff3);
        pdgparticle p(pdgid, Name, Texname, type, Typename);
        // Fill map
        fPDGMap[pdgid] = p;
    }
}

void DecayChainInfo::WriteAndPrint(TDirectory* dir) {
    cout << "|<-" << DecayChainInfo::fPDGMap[this->MID].get_name() << "->|" << this->TotalNumber << "|";

    dir->cd();
    string dirName = DecayChainInfo::fPDGMap[this->MID].get_name() + "->" + DecayChainInfo::fPDGMap[this->PID].get_name();
    TDirectory* newdir = dir->mkdir(dirName.c_str());
    newdir->cd();
    TH1I hmothers("hGMIDs", "ID os the mothers", this->GMIDs.size(), 0, this->GMIDs.size());
    int bin_id = 1;
    for (map<int, int>::const_iterator m = this->GMIDs.begin(); m != GMIDs.end(); m++, bin_id++) {
        hmothers.GetXaxis()->SetBinLabel(bin_id, DecayChainInfo::fPDGMap[m->first].get_texname().c_str());
        hmothers.SetBinContent(bin_id, m->second);
    }
    hmothers.Write();

    for (vector<CutInfo>::iterator ct = Cuts.begin(); ct != Cuts.end(); ct++) {
        cout << ct->NumberOfPassed << "|";
        ct->WriteAndPrint(newdir);
    }

    cout << endl;
}

void CutInfo::WriteAndPrint(TDirectory* dir) {
    dir->cd();
    dir->mkdir(this->Name.c_str())->cd();

    for (int hi = 0; hi < allHs.size(); hi++)
        allHs.at(hi)->Write();

    TH1I hmothers("hGMIDs", "ID os the mothers", this->GMIDs.size(), 0, this->GMIDs.size());
    int bin_id = 1;
    for (map<int, int>::const_iterator m = this->GMIDs.begin(); m != GMIDs.end(); m++, bin_id++) {
        hmothers.GetXaxis()->SetBinLabel(bin_id, DecayChainInfo::fPDGMap[m->first].get_texname().c_str());
        hmothers.SetBinContent(bin_id, m->second);
    }
    hmothers.Write();
    dir->cd();
}

void FakeAnalyzer::AddCut(string cut, string name) {
    for (vector<DecayChainInfo>::iterator decay = AllDecays.begin(); decay != AllDecays.end(); decay++) {
        decay->AddCut(name, cut);
    }
    OtherDecays.push_back(0);
    OtherDecaysBarrel.push_back(0);
}

void FakeAnalyzer::Analyze() {
    for (int ele = 0; ele != this->TheTree->NEles; ele++) {
        if (this->EtaStatus == EndCap && fabs(TheTree->ElEta[ele]) < 1.479)
            continue;
        if (this->EtaStatus == Barel && fabs(TheTree->ElEta[ele]) > 1.49)
            continue;

        if (this->ConversionRejection &&
                ((fabs(TheTree->ElConvPartnerTrkDCot[ele]) < 0.02 && fabs(TheTree->ElConvPartnerTrkDist[ele]) < 0.02))
                || TheTree->ElNumberOfMissingInnerHits[ele] > 1)
            continue;
        if (this->TheTree->ElEcalDriven[ele] == 0)
            continue;

        int ndecays = 0;
        for (vector<DecayChainInfo>::iterator decay = AllDecays.begin(); decay != AllDecays.end(); decay++) {
            ndecays += decay->AddEle(ele) ? 1 : 0;
        }

        if (ndecays == 0) {
            OtherDecays[0]++;

            if (fabs(TheTree->ElEta[ele]) < 1.479)
                OtherDecaysBarrel[0]++;

            uint cut_idx = 1;
            for (vector<CutInfo>::iterator cut = AllDecays.begin()->Cuts.begin(); cut != AllDecays.begin()->Cuts.end(); cut++, cut_idx++)
                if (cut->Pass(ele)) {
                    OtherDecays[cut_idx]++;
                    if (fabs(TheTree->ElEta[ele]) < 1.479)
                        OtherDecaysBarrel[cut_idx]++;
                }
        }
    }
}


//#include "FakeRate/StyleManager.h"
void FakeAnalyzer::End(TDirectory* dir) {
    TDirectory* mydir = dir->mkdir(this->Name.c_str());
    mydir->cd();

    std::map<int, vector<DecayChainInfo*> > FinalStates;
    std::map<int, vector<int> > FinalStateCounts;
    std::map<int, vector<int> > FinalStateCountsBarrel;
    for (vector<DecayChainInfo>::iterator decay = AllDecays.begin(); decay != AllDecays.end(); decay++) {
        FinalStates[decay->PID].push_back(&(*decay));


        if (FinalStateCounts.count(decay->PID) == 0) {
            (FinalStateCounts[decay->PID]).push_back(decay->TotalNumber);
            (FinalStateCountsBarrel[decay->PID]).push_back(decay->TotalNumberBarrel);
            for (vector<CutInfo>::iterator cut = decay->Cuts.begin(); cut != decay->Cuts.end(); cut++) {
                (FinalStateCounts[decay->PID]).push_back(cut->NumberOfPassed);
                (FinalStateCountsBarrel[decay->PID]).push_back(cut->NumberOfPassedBarrel);
            }
        } else {
            (FinalStateCounts[decay->PID])[0] += (decay->TotalNumber);
            (FinalStateCountsBarrel[decay->PID])[0] += (decay->TotalNumberBarrel);
            int counter_tmp = 1;
            for (vector<CutInfo>::iterator cut = decay->Cuts.begin(); cut != decay->Cuts.end(); cut++, counter_tmp++) {
                (FinalStateCounts[decay->PID])[counter_tmp] += cut->NumberOfPassed;
                (FinalStateCountsBarrel[decay->PID])[counter_tmp] += cut->NumberOfPassedBarrel;
            }
        }
    }

    vector<int> Sums;
    for (std::map<int, vector<int> >::iterator count1 = FinalStateCounts.begin();
            count1 != FinalStateCounts.end(); count1++) {
        if (Sums.size() == 0)
            Sums = count1->second;
        else {
            int iii = 0;
            for (vector<int>::const_iterator summ = count1->second.begin();
                    summ != count1->second.end(); summ++, iii++)
                Sums[iii] += (*summ);
        }
    }

    if (AllDecays.size() > 0) {
        cout << endl << "* " << this->Name << endl;
        cout << "|" << this->Name << "|Total|";
        DecayChainInfo* firstDecay = &(*AllDecays.begin());
        for (vector<CutInfo>::iterator cut = firstDecay->Cuts.begin();
                cut != firstDecay->Cuts.end(); cut++) {
            cout << cut->Name << "|";
        }
        cout << endl;
    }

    std::map<string, std::map<string, THStack*> > cutStacks;
    uint finalStateIdx(0);
    for (std::map<int, vector<DecayChainInfo*> >::iterator finalState = FinalStates.begin();
            finalState != FinalStates.end(); finalState++ , finalStateIdx++) {
        cout << "|-|" << endl;
        cout << "|-|" << endl;

        cout << "|" << DecayChainInfo::fPDGMap[finalState->first].get_name() << "|";
        for (int ii = 0; ii < FinalStateCounts[finalState->first].size(); ii++) {
            cout.precision(1);
            cout << fixed << 100 * double(FinalStateCounts[finalState->first][ii]) / (Sums[ii] + OtherDecays[ii]) << "(" << (100.0 * FinalStateCountsBarrel[finalState->first][ii]) / FinalStateCounts[finalState->first][ii] << ")|";
        }
        cout << endl;

        cout << "|-|" << endl;

        TDirectory* fs_dir = mydir->mkdir(DecayChainInfo::fPDGMap[finalState->first].get_name().c_str());

        for (uint i = 0; i < finalState->second.size(); i++)
            finalState->second.at(i)->WriteAndPrint(fs_dir);

        DecayChainInfo* firstDecay = &(*AllDecays.begin());
        int cut_idx = 0;
        for (vector<CutInfo>::iterator cut = firstDecay->Cuts.begin();
                cut != firstDecay->Cuts.end(); cut++, cut_idx++) {
            TDirectory* FSCut_dir = fs_dir->mkdir(cut->Name.c_str());
            FSCut_dir->cd();

            std::map<string, THStack*>* stacks = &(cutStacks[cut->Name]);

            for (uint histo_id = 0; histo_id < cut->allHs.size(); histo_id++) {
                string oldName = cut->allHs.at(histo_id)->GetName();
                string newName = (string(fs_dir->GetName()) + "_" + cut->Name + "_" + SplitString(oldName, "_").rbegin()->c_str());
                string title = (string(fs_dir->GetName())) + " " + cut->allHs.at(histo_id)->GetTitle();
                int nBins = cut->allHs.at(histo_id)->GetNbinsX();
                double min = cut->allHs.at(histo_id)->GetXaxis()->GetXmin();
                double max = cut->allHs.at(histo_id)->GetXaxis()->GetXmax();

                THStack* stack;
                if(stacks->count( *(SplitString(oldName, "_").rbegin() ) ) == 0 ){
                    string stackName = "stack_"+ cut->Name + "_" + SplitString(oldName, "_").rbegin()->c_str();
                    stack = new THStack(stackName.c_str() , title.c_str());
                    stacks->insert( pair<string, THStack*>( *(SplitString(oldName, "_").rbegin() ) , stack) );
                }else
                    stack =(stacks->find( *(SplitString(oldName, "_").rbegin())))->second;
                
                FSCut_dir->cd();
                //                cout << gDirectory->GetPath() << endl;

                TH1* hNew;
                if (strcmp(cut->allHs.at(histo_id)->ClassName(), "TH1I") == 0)
                    hNew = new TH1I(newName.c_str(), title.c_str(), nBins, min, max);
                else if (strcmp(cut->allHs.at(histo_id)->ClassName(), "TH2I") == 0) {
                    int nBins2 = cut->allHs.at(histo_id)->GetNbinsY();
                    double min2 = cut->allHs.at(histo_id)->GetYaxis()->GetXmin();
                    double max2 = cut->allHs.at(histo_id)->GetYaxis()->GetXmax();
                    hNew = new TH2I(newName.c_str(), title.c_str(), nBins, min, max, nBins2, min2, max2);
                } else
                    cout << newName << " is from " << cut->allHs.at(histo_id)->ClassName() << endl;

                //                hNew->Print("base");
                //                cut->allHs.at(histo_id)->Print("base");
                for (uint i = 0; i < finalState->second.size(); i++)
                    for (uint ihisto__ = 0; ihisto__ < finalState->second.at(i)->Cuts[cut_idx].allHs.size(); ihisto__++) {
                        TH1* old_hh = finalState->second.at(i)->Cuts[cut_idx].allHs.at(ihisto__);
                        if (strcmp(SplitString(old_hh->GetName(), "_").rbegin()->c_str(), SplitString(oldName, "_").rbegin()->c_str()) == 0)
                            hNew->Add(old_hh);
                        //                            old_hh->Print("base");
                    }
                StyleManager::SetStyle(hNew,finalStateIdx)->Write();

                stack->Add(hNew);
            }

            fs_dir->cd();
        }
        mydir->cd();
    }

    for(std::map<string, std::map<string, THStack*> >::iterator cutStk = cutStacks.begin() ;
            cutStk != cutStacks.end() ; cutStk++){
        mydir->mkdir(cutStk->first.c_str())->cd();
        for(std::map<string, THStack*>::iterator stak = cutStk->second.begin() ; stak != cutStk->second.end() ; stak++)
            stak->second->Write();
    }

    cout << "|-|" << endl;
    cout << "|-|" << endl;
    cout << "|Others|";
    uint sum_idx(0);
    for (vector<int>::const_iterator other = OtherDecays.begin();
            other != OtherDecays.end(); other++, sum_idx++) {
        cout.precision(1);
        cout << fixed << (100.0 * OtherDecays[sum_idx]) / Sums[sum_idx] << "(" << (100.0 * OtherDecaysBarrel[sum_idx]) / OtherDecays[sum_idx] << ")|";
    }
    cout << endl << "|-|" << endl;
    cout << "|Sum|";
    sum_idx = 0;
    for (vector<int>::const_iterator summ = Sums.begin();
            summ != Sums.end(); summ++, sum_idx++)
        cout << (*summ) << "+" << OtherDecays[sum_idx] << "|";
    cout << endl;
    cout << endl;
    cout << endl;
}

FakeAnalyzer::FakeAnalyzer(TreeReader* tree, string name, double pt, EC_BAREL etaStatus, bool ConvRej) : TheTree(tree), Name(name), EtaStatus(etaStatus), ConversionRejection(ConvRej) {
    DecayChainInfo d1(0, 0, tree);
    this->AllDecays.push_back(d1);


    DecayChainInfo d15_11(11, 15, tree); //tau->e
    this->AllDecays.push_back(d15_11);

    DecayChainInfo d111_11(11, 111, tree); //pi0->e
    this->AllDecays.push_back(d111_11);

    DecayChainInfo d411_11(11, 411, tree); //d+ -> e
    this->AllDecays.push_back(d411_11);

    DecayChainInfo d421_11(11, 421, tree); //d0 -> e
    this->AllDecays.push_back(d421_11);

    DecayChainInfo d431_11(11, 431, tree); //d0 -> e
    this->AllDecays.push_back(d431_11);

    DecayChainInfo d521_11(11, 521, tree); //b+ -> e
    this->AllDecays.push_back(d521_11);

    DecayChainInfo d511_11(11, 511, tree); //b0 -> e
    this->AllDecays.push_back(d511_11);

    DecayChainInfo d531_11(11, 531, tree); //bs0 -> e
    this->AllDecays.push_back(d531_11);

    DecayChainInfo d5122_11(11, 5122, tree); //Lambda_b0 -> e
    this->AllDecays.push_back(d5122_11);

    DecayChainInfo d24_11(11, 24, tree); //Lambda_b0 -> e
    this->AllDecays.push_back(d24_11);

    //--------------------------------
    DecayChainInfo d111_22(22, 111, tree); //pi0 -> photon
    this->AllDecays.push_back(d111_22);

    DecayChainInfo d2_22(22, 2, tree); //u -> photon
    this->AllDecays.push_back(d2_22);

    DecayChainInfo d221_22(22, 221, tree); //eta -> photon
    this->AllDecays.push_back(d221_22);

    DecayChainInfo d223_22(22, 223, tree); //omega -> photon
    this->AllDecays.push_back(d223_22);

    //--------------------------------
    DecayChainInfo d92_211(211, 92, tree); //string2 -> pi+
    this->AllDecays.push_back(d92_211);

    DecayChainInfo d113_211(211, 113, tree); //rho -> pi+
    this->AllDecays.push_back(d113_211);

    DecayChainInfo d213_211(211, 213, tree); //rho -> pi+
    this->AllDecays.push_back(d213_211);

    DecayChainInfo d221_211(211, 221, tree); //eta -> pi+
    this->AllDecays.push_back(d221_211);

    DecayChainInfo d313_211(211, 313, tree); //k* -> pi+
    this->AllDecays.push_back(d313_211);

    DecayChainInfo d323_211(211, 323, tree); //k* -> pi+
    this->AllDecays.push_back(d323_211);

    DecayChainInfo d431_211(211, 431, tree); //d0 -> pi+
    this->AllDecays.push_back(d431_211);

    //----------------------------------
    DecayChainInfo d311_310(310, 311, tree); //k0 -> k0s
    this->AllDecays.push_back(d311_310);

    //----------------------------------
    DecayChainInfo d92_321(321, 92, tree); //k0 -> k+
    this->AllDecays.push_back(d92_321);

    DecayChainInfo d313_321(321, 313, tree); //k* -> k+
    this->AllDecays.push_back(d313_321);

    DecayChainInfo d323_321(321, 323, tree); //k* -> k+
    this->AllDecays.push_back(d323_321);

    DecayChainInfo d333_321(321, 333, tree); //phi -> k+
    this->AllDecays.push_back(d333_321);

    DecayChainInfo d421_321(321, 421, tree); //d0 -> k+
    this->AllDecays.push_back(d421_321);

    //------------------------------------
    DecayChainInfo d92_2212(2212, 92, tree); //stirng -> p
    this->AllDecays.push_back(d92_2212);

    DecayChainInfo d2214_2212(2212, 2214, tree); //delta -> p
    this->AllDecays.push_back(d2214_2212);

    DecayChainInfo d2114_2212(2212, 2114, tree); //delta -> p
    this->AllDecays.push_back(d2114_2212);

    DecayChainInfo d2224_2212(2212, 2224, tree); //delta -> p
    this->AllDecays.push_back(d2224_2212);

    DecayChainInfo d4122_2212(2212, 4122, tree); //lamdba -> p
    this->AllDecays.push_back(d4122_2212);


    //---------------------------------------
    //SOME DEFAULT CUTS:
    //---------------------------------------
    if (pt > 5) {
        OtherDecays.push_back(0);
        OtherDecaysBarrel.push_back(0);
        std::ostringstream pt_cut;
        pt_cut << "Pt(" << pt << ",*)";
        AddCut(pt_cut.str(), "pt_cut");

        AddCut(pt_cut.str() + "&RelIso04(*,0.7)", "isol");

        AddCut(pt_cut.str() + "&IDRobustLoose(1,1)", "idrl");
        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)", "isolidrl");
//        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)&ConvPartnerTrkDCot(-0.02,0.02)&ConvPartnerTrkDist(-0.02,0.02)", "isolidrlconv0");
//        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)&ConvPartnerTrkDCot(-0.01,0.01)", "isolidrlconv1");
//        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)&ConvPartnerTrkDist(-0.01,0.01)", "isolidrlconv2");
//        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)&NumberOfMissingInnerHits(1,*)", "isolidrlconv3");
//
        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)&D0(1,*)", "isolidrld0");
        AddCut(pt_cut.str() + "&RelIso04(*,0.2)&IDRobustLoose(1,1)&D0(1,*)", "isoidrld0");
//
//        AddCut(pt_cut.str() + "&RelIso04(*,0.7)&IDRobustLoose(1,1)&IDRobustTight(0,0)", "isolidrlnidrt");
//        AddCut(pt_cut.str() + "&RelIso04(0.3,0.7)&IDRobustLoose(1,1)&IDRobustTight(0,0)", "nisolidrlnidrt");
//        AddCut(pt_cut.str() + "&RelIso04(0.3,0.7)&IDRobustTight(1,1)", "nisoidrt");
//        AddCut(pt_cut.str() + "&RelIso04(*,0.15)&IDLoose(1,1)&NumberOfMissingInnerHits(0,1)&D0BS(-0.04,0.04)&Eta(-2.4,2.4)", "nisoidrl");
        //        AddCut(pt_cut.str() + "&DR04TkSumPt(*,3)&IDRobustLoose(1,1)", "isotrk_idrl");
        //        AddCut(pt_cut.str() + "&DR04HcalTowerSumEt(*,3)&IDRobustLoose(1,1)", "isohcl_idrl");
        //        AddCut(pt_cut.str() + "&DR04EcalRecHitSumEt(*,3)&IDRobustLoose(1,1)", "isoecl_idrl");
        AddCut(pt_cut.str() + "&IDRobustTight(1,1)", "idrt");
        AddCut(pt_cut.str() + "&&IDRobustTight(1,1)&D0PV(-0.02,0.02)", "idrtd0");
        AddCut(pt_cut.str() + "&RelIso04(*,0.2)&IDRobustTight(1,1)&D0PV(-0.02,0.02)", "isoidrtd0");
        //        AddCut(pt_cut.str() + "&DR04TkSumPt(*,3)&IDRobustTight(1,1)", "isotrk_idrt");
        //        AddCut(pt_cut.str() + "&DR04HcalTowerSumEt(*,3)&IDRobustTight(1,1)", "isohcl_idrt");
        //        AddCut(pt_cut.str() + "&DR04EcalRecHitSumEt(*,3)&IDRobustTight(1,1)", "isoecl_idrt");
    }
}
