/* 
 * File:   PMMReader.cc
 * Author: hbakhshi
 * 
 * Created on November 4, 2010, 2:13 PM
 */

#include <iostream>
#include <math.h>
#include <TFile.h>
#include <TDictionary.h>
#include <TH1.h>

#include "ElectronCharge/include/PMMReader.h"

using namespace std;

const PMMReader::ValueError PMMReader::GetPMM(double eta ){
    PMMReader::ValueError ve;
    if(SeparateBarrelEndCap){
        ve.Value = this->hPMM.GetBinContent(hPMM.GetXaxis()->FindBin(fabs(eta)));
        ve.Error = this->hPMM.GetBinError(hPMM.GetXaxis()->FindBin(fabs(eta)));
    }else{
        ve.Value = this->hPMM.GetBinContent(1);
        ve.Error = this->hPMM.GetBinError(1);
    }

    return ve;
}

PMMReader::PMMReader(std::string fileName, ChargeSource cs, int cuts, bool separateBarrelEndCap) : SeparateBarrelEndCap(separateBarrelEndCap) {
    TFile* f = TFile::Open(fileName.c_str(), "read");

    std::string chargeDirName;
    switch (cs) {
        case PMMReader::GSFCharge:
            chargeDirName = "GsfCharge";
            break;
        case PMMReader::AllChargesMatch:
            chargeDirName = "AllCharge";
            break;
        case PMMReader::MajorityCharge:
            chargeDirName = "EleCharge";
            break;
    }
    TDirectory* charge_dir = f->GetDirectory(chargeDirName.c_str());
    if (!charge_dir)
        throw PMMReaderException("The charge directory doesn't exist in the file");

    TDirectory* CutsDir = NULL;

    TList * ls = charge_dir->GetListOfKeys();
    TListIter itr(ls);
    TObject * obj(0);
    while ((obj = itr.Next()) != 0) {
        string subdir(obj->GetName());
        bool isItThat = true;

        if (cuts == PMMReader::NoCUT) {
            if (subdir != "All")
                isItThat = false;
        } else {
            if (cuts & PMMReader::D0) {
                if (subdir.find("D0") == string::npos)
                    isItThat = false;
            }
            if (cuts & PMMReader::CTFMatch) {
                if (subdir.find("CTF") == string::npos)
                    isItThat = false;
            }
            if (cuts & PMMReader::ConvRej0) {
                if (subdir.find("ConvTight") == string::npos)
                    isItThat = false;
            }
            if (cuts & PMMReader::ConvRej1) {
                if (subdir.find("Conv") == string::npos)
                    isItThat = false;
            }
        }

        if (isItThat) {
            charge_dir->cd(subdir.c_str());
            CutsDir = gDirectory;
            break;
        }
    }

    if (CutsDir == NULL)
        throw PMMReaderException("Combination of the cuts you want, is not available in the file.");

    cout << cuts << "--" <<CutsDir->GetPath() << " is being used for Pmm" << endl;

    TH1D* h;
    if (separateBarrelEndCap) {
        std::string obj_name("BarrelEndcap/");
        obj_name += CutsDir->GetName();
        obj_name += "_EffPoissonErr_BarrelEndcap";
        h = ((TH1D*) CutsDir->Get(obj_name.c_str()));
    } else {
        std::string obj_name("avg_Total/");
        obj_name += CutsDir->GetName();
        obj_name += "_EffPoissonErr_avg_Total";
        h = ((TH1D*) CutsDir->Get(obj_name.c_str()));
    }

    int nBins = h->GetNbinsX();
    double bins[nBins + 1];
    for (int kkk = 0; kkk < nBins; kkk++)
        bins[kkk] = h->GetBinLowEdge(kkk + 1);
    bins[nBins] = bins[nBins - 1] + h->GetBinWidth(nBins + 1);
    this->hPMM.SetBins(nBins, bins);
    this->hPMM.Add(h);

    f->Close();
    delete f;
}

PMMReader::~PMMReader() {
    this->hPMM.Delete();
}

