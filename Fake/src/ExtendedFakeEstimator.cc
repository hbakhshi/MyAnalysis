/* 
 * File:   ExtendedFakeEstimator.cc
 * Author: hbakhshi
 * 
 * Created on August 4, 2010, 6:17 PM
 */

#include "../include/ExtendedFakeEstimator.h"

ExtendedFakeEstimator::ExtendedFakeEstimator(TH1F* hISOConv, TH1F* hIDConv, TH1F* hISOO, TH1F* hIDO, TH1F* hConvO,
        string LooseCuts, string IsoCuts, string TightCuts, string name, TreeReader* tree) : BaseAlgorithm(),
hFIsoGamma(*hISOConv),
hFtGamma(*hISOConv),
hFIsoOthers(*hISOO),
hFtOthers(*hISOO),

hIsoNotTight(*hISOConv),
hTight(*hISOConv),
hTightConv(*hISOConv),
hTightOthers(*hISOConv),
hLooseNotTight(*hISOConv),
hCoefLooseNotTight(*hISOConv),
hCoefIsoNotTight(*hISOConv),
hCoefDenominator(*hISOConv),

isoParams(IsoCuts) {
    cout << name << " --------------- : " << endl;
    base::algoName = name;
    base::foParams = LooseCuts;
    base::leptonParams = TightCuts;
    base::TheTree = tree;
    //histograms for holding the numbers of electrons are initialised just with the standard binning
    //at first they must be emptied
    //    hISOConv->Print("range");
    //    hIDConv->Print("range");
    //    hISOO->Print("range");
    //    hIDO->Print("range");
    //    hConvO->Print("range");

    this->ClearHistogram(&hTightConv);
    hTightConv.SetNameTitle("hNTightConv", "# tight electrons from conversion");

    this->ClearHistogram(&hTightOthers);
    hTightOthers.SetNameTitle("hNTightOthers", "# tight electrons, others");

    this->ClearHistogram(&hTight);
    hTight.SetNameTitle("hNTight", "# tight electrons");

    this->ClearHistogram(&hIsoNotTight);
    hIsoNotTight.SetNameTitle("hNIsoNotTight", "#isolated but not tight electrons");

    this->ClearHistogram(&hLooseNotTight);
    hLooseNotTight.SetNameTitle("hNLooseNotTight", "#loose but not tight electrons");

    this->ClearHistogram(&hCoefDenominator);
    hCoefDenominator.SetNameTitle("hCoefDenominator", "coef");

    this->ClearHistogram(&hCoefIsoNotTight);
    hCoefIsoNotTight.SetNameTitle("hCoefIsoNotTight", "coef");

    this->ClearHistogram(&hCoefLooseNotTight);
    hCoefLooseNotTight.SetNameTitle("hCoefLooseNotTight", "coef");

    ParseCuts(leptonParams, &(Lep_Cuts), "El");
    ParseCuts(foParams, &(FO_Cuts), "El");
    ParseCuts(isoParams, &(ISO_Cuts), "El");

    //calculate the coefficients:

    hFtOthers.Multiply(hIDO);
    hFtOthers.Multiply(hConvO);

    hFtGamma.Multiply(& (0.75 * (*hIDConv))); //I assumeed that 30% of electrons from coversion can pass conversion-rejection cuts

    hCoefIsoNotTight.Add(&hFtGamma, &hFtOthers, 1, -1);

    hCoefLooseNotTight.Multiply(&hFIsoGamma, &hFtOthers);
    hCoefLooseNotTight.Add(& (hFIsoOthers * hFtGamma), -1.0);

    hCoefDenominator.Add(&hFIsoGamma, &hFIsoOthers, 1, -1.0);
    hCoefDenominator.Add(&(-1.0 * hCoefIsoNotTight));
    hCoefDenominator.Add(&(-1.0 * hCoefLooseNotTight));

    //TH1F hCoef1(hCoefLooseNotTight);
    hCoefLooseNotTight.Divide(&hCoefDenominator);
    hCoefLooseNotTight.Print("ALL");

    //TH1F hCoef2(hCoefIsoNotTight);
    hCoefIsoNotTight.Divide(&hCoefDenominator);
    hCoefIsoNotTight.Print("ALL");
}

ExtendedFakeEstimator::~ExtendedFakeEstimator() {
}

void ExtendedFakeEstimator::ClearHistogram(TH1* h) {
    h->Clear();
    h->SetEntries(0);

    for (int i = 0; i <= h->GetNbinsX(); i++) {
        h->SetBinContent(i, 0);
        h->SetBinError(i, 0);
    }
}

string ExtendedFakeEstimator::MakeString() {
    return "EXTENDED";
}

void ExtendedFakeEstimator::WriteAll(TDirectory* dir) {
    cout << algoName << "--------------------" << endl;
    this->hLooseNotTight.Print("ALL");
    this->hIsoNotTight.Print("ALL");
    this->hTight.Print("ALL");
    this->hTightConv.Print("ALL");
    cout << "------------------------------------" << endl;
}

vector<BaseAlgorithm::FakeInfo> ExtendedFakeEstimator::GetAll() {
    vector<FakeInfo> ret;

    vector<base::Object> obj(GetFakableObjs());
    for (int ie = 0; ie < obj.size(); ie++) {
        FakeInfo fi;
        fi.FakableObject = obj.at(ie);
        fi.hasEle = false;

        bool passIso = false;
        bool passTight = false;

        if (this->PARAM_EtaRegion == 1) //Barrel
        {
            if (fabs(fi.FakableObject.eta) > 1.47)
                continue;
        } else if (this->PARAM_EtaRegion == 2) //Endcap
        {
            if (fabs(fi.FakableObject.eta) < 1.47)
                continue;
        }

        if (base::PassCuts(&(ISO_Cuts), obj.at(ie).index))
            passIso = true;


        bool convRej = !(this->PARAM_ConvRejAll &&
                fabs(TheTree->ElConvPartnerTrkDCot[obj.at(ie).index]) < 0.02 &&
                fabs(TheTree->ElConvPartnerTrkDist[obj.at(ie).index]) < 0.02);

        if (base::PassCuts(&(base::Lep_Cuts), obj.at(ie).index) && convRej)
            passTight = true;

        double pt = fi.FakableObject.pt;
        fi.coeff1 = passTight ? 0.0 : ReadHisto(&hCoefLooseNotTight, pt);
        fi.coeff1 += passIso && !passTight ? ReadHisto(&hCoefIsoNotTight, pt) : 0.0;

        // the probabiliry that this electron is comming from conversion        
        fi.coeff2 = passTight ? 0.0 : -1*ReadHisto(&hFIsoOthers, pt) ;
        fi.coeff2 += passIso ? 0.0 : ReadHisto(&hFtOthers, pt) ;
        fi.coeff2 += passIso && !passTight ? 1.0 : 0.0;
        fi.coeff2 *= ReadHisto(&hFtGamma, pt);
        fi.coeff2 /= ReadHisto(&hCoefDenominator, pt);
        if(fi.coeff2 == INFINITY || fi.coeff2 == NAN){
            cout << "infinity" << endl;
            fi.coeff2 == 0.0;
        }

        fi.coeff3 = passTight ? 1.0 : 0.0;

        if (passIso && !passTight)
            this->hIsoNotTight.Fill(pt);
        if (!passTight)
            this->hLooseNotTight.Fill(pt);
        if (passTight) {
            this->hTight.Fill(pt);
            if (this->TheTree->ElGenID[fi.FakableObject.index] == 22)
                this->hTightConv.Fill(pt);
            else
                this->hTightOthers.Fill(pt);
        }
        ret.push_back(fi);
    }

    return ret;
}