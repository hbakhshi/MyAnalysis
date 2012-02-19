/*
 * File:   EleJetFR.cc
 * Author: hbakhshi
 *
 * Created on June 30, 2010, 5:55 PM
 */

#include "../include/EleJetFR.hh"
#include "DataFormats/interface/TreeReaderEvent.h"
#include "helper/Utilities.hh"

using namespace std;

EleJetFR::EleJetFR(string name, string ctr, TreeReaderEvent* tree) : base(name, ctr, tree) {
    base::ParseCuts(base::leptonParams, &(base::Lep_Cuts), "El");
    base::ParseCuts(base::foParams, &(base::FO_Cuts), "J");

    vector<Cut> params;
    base::ParseCuts(base::algoParams, &params, "JEl");
    for (vector<Cut>::const_iterator param_it = params.begin(); param_it != params.end(); param_it++) {
        if (param_it->propertyName == "DR") {
            this->PARAM_DR_MAX = param_it->maxVal;
            this->PARAM_DR_MIN = param_it->minVal;
        }
    }

    cout << "Delta R :" << PARAM_DR_MIN << "  " << PARAM_DR_MAX << endl;
};

EleJetFR::~EleJetFR() {
}

string EleJetFR::MakeString() {
    return "";
};

vector<BaseAlgorithm::FakeInfo> EleJetFR::GetAll() {
    vector<base::FakeInfo> ret;

    vector<base::Object> jet(GetFakableObjs());
    vector<base::Object> ele(GetElectrons());
    for (int ie = 0; ie < jet.size(); ie++) {
        FakeInfo fi;
        fi.FakableObject = jet.at(ie);
        fi.hasEle = false;

        int eleId = -1;
        double dist = 9999.;
        for (uint ei = 0; ei < ele.size(); ei++) {
            double dr = Util::GetDeltaR(fi.FakableObject.eta, ele.at(ei).eta, fi.FakableObject.phi, ele.at(ei).phi);
            if (dr < dist) {
                dist = dr;
                eleId = ei;
            }
        }
        if (eleId != -1 && dist < PARAM_DR_MAX && dist > PARAM_DR_MIN) {
            fi.Electron = ele.at(eleId);
            fi.hasEle = true;
        }

        ret.push_back(fi);
    }

    base::FillAll(&ret);

    return ret;
};
