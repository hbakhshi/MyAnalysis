/* 
 * File:   EleIsoTemplate.cc
 * Author: hbakhshi
 * 
 * Created on June 30, 2010, 5:55 PM
 */

#include "../include/EleIsoTemplate.h"
#include "DataFormats/interface/TreeReaderEvent.h"
#include <algorithm>

using namespace std;

EleIsoTemplate::EleIsoTemplate(string name, string ctr, TreeReaderEvent* tree) : base(name, ctr, tree) {
    base::ParseCuts(base::leptonParams, &(base::Lep_Cuts), "El");
    base::ParseCuts(base::foParams, &(base::FO_Cuts), "El");

    AnalyzeArgs();
};


EleIsoTemplate::~EleIsoTemplate() {
}

string EleIsoTemplate::MakeString() {
    return "";
};

vector<BaseAlgorithm::FakeInfo> EleIsoTemplate::GetAll() {
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
        if (this->PARAM_GenFilter != -1) {
            int pdgID = fabs(TheTree->ElGenID[obj.at(ie).index]);
            switch (PARAM_GenFilter) {
                case 0: //from conversion
                    if (pdgID != 22)
                        continue;
                    break;
                case 1: //an electron (mostly from b or d decay)
                    if (pdgID != 11)
                        continue;
                    break;
                case 2: //other cases, but uid
                    if (pdgID == 11 || pdgID == 22 || pdgID == 0)
                        continue;
                    break;
                case 3: //all, but uid and conversion
                    if (pdgID == 22 || pdgID == 0)
                        continue;
                    break;
            }
        }

        if (this->PARAM_ConvRejAll)
            if (fabs(TheTree->ElConvPartnerTrkDCot[obj.at(ie).index]) < 0.02 &&
                    fabs(TheTree->ElConvPartnerTrkDist[obj.at(ie).index]) < 0.02)
                continue;

        if (this->ElectronTag == "") {
            if (base::PassCuts(&(base::Lep_Cuts), obj.at(ie).index)) {
                if (this->PARAM_ConvRejEle &&
                        fabs(TheTree->ElConvPartnerTrkDCot[obj.at(ie).index]) < 0.02 &&
                        fabs(TheTree->ElConvPartnerTrkDist[obj.at(ie).index]) < 0.02)
                    fi.hasEle = false;
                else {
                    fi.Electron = obj.at(ie);
                    fi.hasEle = true;
                }
            }
        } else {
            vector<int>* ies = &(base::TheTree->ElectronsToAnalyze[ElectronTag]);
            if (std::find(ies->begin(), ies->end(), obj.at(ie).index) != ies->end()) {
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

vector<EleIsoTemplate::base::Object> EleIsoTemplate::GetFakableObjs() {
    if (FakableElectronTag != "") {
        vector<base::Object> ret;
        vector<int> ies = base::TheTree->ElectronsToAnalyze[FakableElectronTag];

        if (ies.size() == 1) {
            double ele_pt = TheTree->ElPt[ies.at(0)];
            double ele_eta = this->PARAM_EtaRegion == 0 ? TheTree->ElEta[ies.at(0)] : fabs(TheTree->ElEta[ies.at(0)]);
            double ele_phi = TheTree->ElPhi[ies.at(0)];

            base::Object ele(ele_pt, ele_eta, ele_phi, ies.at(0));
            ret.push_back(ele);
        }
        
        return ret;
    } else {
        return base::GetFakableObjs();
    }
}

void EleIsoTemplate::AnalyzeArgs() {
    PARAM_GenFilter = (-1);
    PARAM_ConvRejAll = (false);
    PARAM_ConvRejEle = (false);
    PARAM_EtaRegion = (0);
    vector<Cut> params;
    ParseCuts(this->algoParams, &params, "Params");

    for (vector<Cut>::const_iterator param_it = params.begin(); param_it != params.end(); param_it++) {
        if (param_it->propertyName == "ConvAll" && param_it->minVal == param_it->maxVal)
            this->PARAM_ConvRejAll = true;
        else if (param_it->propertyName == "ConvEle" && param_it->minVal == param_it->maxVal)
            this->PARAM_ConvRejEle = true;
        else if (param_it->propertyName == "EtaRegion") {
            this->PARAM_EtaRegion = param_it->minVal;

            switch (this->PARAM_EtaRegion) {
                case 1: //barrel
                    //                    base::Eta->SetBinAll(10, 0, 1.4442);
                    break;
                case 2: //endcap
                    //                    base::Eta->SetBinAll(10, 1.566, 2.5);
                    break;
            }
        } else if (param_it->propertyName == "GenFilter" && param_it->minVal == param_it->maxVal) {
            this->PARAM_GenFilter = param_it->minVal;
        }
    }
}