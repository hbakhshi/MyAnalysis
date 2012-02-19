/* 
 * File:   MuonIsoTemplate.cc
 * Author: hbakhshi
 * 
 * Created on February 9, 2011, 2:38 PM
 */

#include "../include/MuonIsoTemplate.h"
#include "DataFormats/interface/TreeReaderEvent.h"
#include <algorithm>

using namespace std;

MuonIsoTemplate::MuonIsoTemplate(string name, string ctr, TreeReaderEvent* tree) : base(name, ctr, tree) {
    base::ParseCuts(base::leptonParams, &(base::Lep_Cuts), "Mu");
    base::ParseCuts(base::foParams, &(base::FO_Cuts), "Mu");

    AnalyzeArgs();
};

MuonIsoTemplate::~MuonIsoTemplate() {
}

string MuonIsoTemplate::MakeString() {
    return "";
};

vector<BaseAlgorithm::FakeInfo> MuonIsoTemplate::GetAll() {
    vector<base::FakeInfo> ret;

    vector<base::Object> obj(GetFakableObjs());
    for (int ie = 0; ie < obj.size(); ie++) {
        FakeInfo fi;
        fi.FakableObject = obj.at(ie);
        fi.hasEle = false;

        if (this->PARAM_EtaRegion == 1) //Barrel
        {
            if (fabs(fi.FakableObject.eta) > 1.4442)
                continue;
        } else if (this->PARAM_EtaRegion == 2) //Endcap
        {
            if (fabs(fi.FakableObject.eta) < 1.566)
                continue;
        }

        if (this->MuonTag == "") {
            if (base::PassCuts(&(base::Lep_Cuts), obj.at(ie).index)) {
                fi.Electron = obj.at(ie);
                fi.hasEle = true;
            }
        } else {
            vector<int>* mus = &(base::TheTree->MuonsToAnalyze[MuonTag]);
            if (std::find(mus->begin(), mus->end(), obj.at(ie).index) != mus->end()) {
                //std::cout << this->algoName << " " << this->eleParams << " " << this->ElectronTag << " It's matched !" << endl;
                fi.Electron = obj.at(ie);
                fi.hasEle = true;
            }
        }


        ret.push_back(fi);
    }
    base::FillAll(&ret);

    return ret;
};

vector<MuonIsoTemplate::base::Object> MuonIsoTemplate::GetFakableObjs() {

    if (FakableMuonTag != "") {
        vector<base::Object> ret;
        vector<int> mus = base::TheTree->MuonsToAnalyze[FakableMuonTag];

        if (mus.size() == 1) {
            double pt = TheTree->MuPt[mus.at(0)];
            double eta = TheTree->MuEta[mus.at(0)];
            double phi = TheTree->MuPhi[mus.at(0)];

            base::Object mu(pt, eta, phi, mus.at(0));
            ret.push_back(mu);
        }

        return ret;
    } else
        return base::GetFakableObjs();
}

void MuonIsoTemplate::AnalyzeArgs() {
    PARAM_EtaRegion = (0);
    vector<Cut> params;
    ParseCuts(this->algoParams, &params, "Params");

    for (vector<Cut>::const_iterator param_it = params.begin(); param_it != params.end(); param_it++) {
        if (param_it->propertyName == "EtaRegion") {
            this->PARAM_EtaRegion = param_it->minVal;

            switch (this->PARAM_EtaRegion) {
                case 1: //barrel
//                    base::Eta->SetBinAll(10, 0, 1.4442);
                    break;
                case 2: //endcap
//                    base::Eta->SetBinAll(10, 1.566, 2.5);
                    break;
            }
        }
    }
}
