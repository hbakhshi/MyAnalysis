/* 
 * File:   BaseAlgorithm.cc
 * Author: hbakhshi
 * 
 * Created on June 30, 2010, 3:33 PM
 */

#include <iomanip>
#include "TF1.h"

#include "../include/BaseAlgorithm.h"

BaseAlgorithm::BaseAlgorithm(string name, string constructor, TreeReaderEvent* tree) : TheTree(tree) {
    vector<string> parts = this->SplitString(constructor, "_");
    algoName = name;
    algoParams = parts[0];
    foParams = parts[1];
    leptonParams = parts[2];

    string objName;
    vector<Cut> params;
    ParseCuts(this->algoParams, &params, "Params");
    for (vector<Cut>::const_iterator param = params.begin(); param != params.end(); param++)
        if (param->propertyName == "ObjectType")
            switch (int(param->minVal)) {
                case 0:
                    objName = "El";
                    break;
                case 1:
                    objName = "Mu";
                    break;
                case 2:
                    objName = "PFJ";
                    break;
            }

    this->FakeRates = new EfficiencyHandler<FakeInfo > ("FakeRates_" + algoName);
    FakeRates->Condition = FakeInfo::HasEle;
    FakeRates->AllCondition = 0;

    FakeInfo::All_Property* all_prop = new FakeInfo::All_Property(objName);
    FakeInfo::Eta_Property* eta_prop = new FakeInfo::Eta_Property(objName, false);
    FakeInfo::Eta_Property* eta_prop_be = new FakeInfo::Eta_Property(objName, true);
    FakeInfo::Eta_Property* eta_prop_be_abs = new FakeInfo::Eta_Property(objName, true,true);
    FakeInfo::PT_Property* pt_prop = new FakeInfo::PT_Property(objName);
    vector<double> pt_bins;
    pt_bins.push_back(0.0);
    pt_bins.push_back(20.0);
    pt_bins.push_back(50.0);
    pt_bins.push_back(100.0);
    pt_bins.push_back(200.0);
    pt_bins.push_back(500.0);
    FakeInfo::PT_Property* pt_prop_bins = new FakeInfo::PT_Property(objName, &pt_bins);
    FakeInfo::Phi_Property* phi_prop = new FakeInfo::Phi_Property(objName);

    effTotal = FakeRates->AddProp(all_prop);
    FakeRates->AddProp(eta_prop);
    FakeRates->AddProp(eta_prop_be);
    FakeRates->AddProp(eta_prop_be_abs);
    FakeRates->AddProp(pt_prop);
    FakeRates->AddProp(pt_prop_bins);
    FakeRates->AddProp(phi_prop);
    FakeRates->AddProp2(pt_prop_bins,eta_prop_be);
    FakeRates->AddProp2(pt_prop_bins,eta_prop_be_abs);
    FakeRates->AddProp2(pt_prop,eta_prop_be);

    this->ObjectMode = Calculation;
}

BaseAlgorithm::~BaseAlgorithm() {
}

vector<BaseAlgorithm::Object> BaseAlgorithm::GetElectrons() {
    vector<Object> ret;

    for (int ie = 0; ie < TheTree->NEles; ie++) {

        if (PassCuts(&(this->Lep_Cuts), ie)) {

            double ele_pt = TheTree->ElPt[ie];
            double ele_eta = TheTree->ElEta[ie];
            double ele_phi = TheTree->ElPhi[ie];

            Object ele(ele_pt, ele_eta, ele_phi, ie);
            ret.push_back(ele);
        }
    }

    return ret;
}

vector<BaseAlgorithm::Object> BaseAlgorithm::GetFakableObjs() {
    vector<Object> ret;

    if (this->FO_Cuts.begin()->objectType == "El")
        for (int ie = 0; ie < TheTree->NEles; ie++) {
            if (PassCuts(&(this->FO_Cuts), ie)) {
                double ele_pt = TheTree->ElPt[ie];
                double ele_eta = TheTree->ElEta[ie];
                double ele_phi = TheTree->ElPhi[ie];

                Object ele(ele_pt, ele_eta, ele_phi, ie);
                ret.push_back(ele);
            }
        } else if (this->FO_Cuts.begin()->objectType == "J")
        for (int ij = 0; ij < TheTree->NJets; ij++) {
            if (PassCuts(&(this->FO_Cuts), ij)) {

                double jet_pt = TheTree->JPt[ij];
                double jet_eta = TheTree->JEta[ij];
                double jet_phi = TheTree->JPhi[ij];

                Object jet(jet_pt, jet_eta, jet_phi, ij);
                ret.push_back(jet);
            }
        } else if (this->FO_Cuts.begin()->objectType == "Mu")
        for (int ij = 0; ij < TheTree->NMus; ij++) {
            if (PassCuts(&(this->FO_Cuts), ij)) {

                double mu_pt = TheTree->MuPt[ij];
                double mu_eta = TheTree->MuEta[ij];
                double mu_phi = TheTree->MuPhi[ij];

                Object mu(mu_pt, mu_eta, mu_phi, ij);
                ret.push_back(mu);
            }
        }

    return ret;
}

bool BaseAlgorithm::PassCuts(vector<Cut>* cuts, int id) {
    double *fval;
    int *ival;

    for (vector<Cut>::iterator ct = cuts->begin(); ct != cuts->end(); ct++) {
        if (ct->leaf == NULL) {
            TBranch* bbb = TheTree->fChain->GetBranch(ct->LeafName().c_str());
            TLeaf* lll = bbb->GetLeaf(ct->LeafName().c_str());
            ct->leaf = lll;
            ct->type = ct->leaf->GetTypeName();
        }
        if (!strcmp(ct->type.c_str(), "Double_t")) {
            fval = (Double_t*) ct->leaf->GetValuePointer();

            if (fval[id] < ct->minVal) return false;
            if (fval[id] > ct->maxVal) return false;
        } else if (!strcmp(ct->type.c_str(), "Int_t")) {
            ival = (Int_t*) ct->leaf->GetValuePointer();

            if (ival[id] < ct->minVal) return false;
            if (ival[id] > ct->maxVal) return false;
        } else
            cout << "UNKNOWN TYPE:" << ct->LeafName() << "  " << ct->type << endl;
    }

    return true;
}

void BaseAlgorithm::WriteAll(TDirectory * dir) {
    this->FakeRates->WriteAll(dir);

    dir->cd(FakeRates->GetName().c_str());

    TString alg_params(this->algoParams);
    TString ele_params(this->leptonParams);
    TString fo_params(this->foParams);

    gDirectory->WriteObject<TString > (&alg_params, "AlgoParams");
    gDirectory->WriteObject<TString > (&ele_params, "ElecParams");
    gDirectory->WriteObject<TString > (&fo_params, "FOParams");

    dir->cd();

    std::cout << "|" << this->algoName << "|"
            << int(this->effTotal->GetEntries()) << "/" << int(this->effTotal->GetHAll()->GetEntries()) << "|"
            << std::setprecision(2) << 100.0 * effTotal->GetPoissonianEff()->GetBinContent(1) << " ± " << 100.0 * effTotal->GetPoissonianEff()->GetBinError(1) / effTotal->GetPoissonianEff()->GetBinContent(1) << " % |"
            << std::endl;
}

void BaseAlgorithm::FillAll(vector<FakeInfo>* info) {
    for (vector<BaseAlgorithm::FakeInfo>::const_iterator it = info->begin(); it != info->end(); it++)
        this->FakeRates->Fill(&(*it));
}

int BaseAlgorithm::ParseCuts(string CUT, vector<Cut>* cts, string objName) {

    vector<string> cuts = SplitString(CUT, "&");

    for (int i = 0; i < cuts.size(); i++) {
        string cut = cuts[i];

        vector<string> splits = SplitString(cut, "(,)");

        Cut theCut;
        theCut.objectType = objName;
        theCut.propertyName = splits[0];

        if (splits.size() > 1 && splits[1] != "*")
            theCut.minVal = atof(splits[1].c_str());

        if (splits.size() > 2 && splits[2] != "*")
            theCut.maxVal = atof(splits[2].c_str());

        cts->push_back(theCut);

    }

    return cuts.size();
}
