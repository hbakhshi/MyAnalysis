/* 
 * File:   FitValidator.h
 * Author: nadjieh
 *
 * Created on May 3, 2012, 11:02 AM
 */

#ifndef FITVALIDATOR_H
#define	FITVALIDATOR_H

//#define TEST
#include "TF1.h"
#include "TF3.h"
#include "TH1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TRandom1.h"
#include "TRandom.h"
#include "TROOT.h"
#include "TVirtualFitter.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include "ToyFitter.h"
#include "TFile.h"
#include <boost/algorithm/string.hpp>

using namespace std;

TRandom SeedGenerator = TRandom(1928374);
TRandom SeedGeneratorLumiEQ = TRandom(874546620);

class SamplesInfo {
public:

    SamplesInfo(string __Wpol_all_location) : Wpol_all_location(__Wpol_all_location) {
        files_xsec["TTBarSummer2011"] = 157.5;
        files_xsec["DYSummer2011"] = 3048.0;
        files_xsec["WJetsSummer2011"] = 31314.0;
        files_xsec["WWSummer2011"] = 4.65;
        files_xsec["SingleTopSummer2011"] = 64.5;
        files_xsec["SingleTopTWSummer2011"] = 2 * 7.9;
        files_xsec["WZSummer2011"] = 0.6;
        files_xsec["ZZSummer2011"] = 4.65;

        Channels["EE"] = 4529.518;
        Channels["MM"] = 4459.007;
        Channels["EM"] = 4631.724;
    }

    ~SamplesInfo() {
    };

    double ReadN0(string Sample, string Channel) const {
        TFile* file = TFile::Open((Wpol_all_location + "/WPol_" + Sample + ".root").c_str(), "READ");
        string selectionPlotName = "Selection/hEventSelection" + Channel;
        TH1* hSelection = (TH1*) file->Get(selectionPlotName.c_str());
        double N0 = hSelection->GetBinContent(1);
        file->Close();
        return N0;
    }

    TH1* GetCosThetaPlot(string Channel, string name, int nFinalBin = 10) {
        //cout << name << endl;
        if (files.count(name) == 0)
            files[name] = TFile::Open(("./WPol_SelectedTTBars_" + name + ".root").c_str(), "READ");

        //cout << "./WPol_SelectedTTBars_" + name + ".root" << endl;

        boost::algorithm::to_lower(Channel);
        TH1* h = (TH1*) files[name]->Get(("costheta_" + Channel +"/hCosThetaAllLepton").c_str());
        cout << ("costheta_" + Channel + "/hCosThetaAllLepton") << endl;
        if ((h->GetXaxis()->GetNbins()) == nFinalBin)
            return h;
        if ((h->GetXaxis()->GetNbins() % nFinalBin) == 0) {
            h->Rebin(((h->GetXaxis()->GetNbins()) / nFinalBin));
        } else {
            std::cout << nFinalBin << " does not count " << h->GetXaxis()->GetNbins() << std::endl;
            std::cout << "I will take the biggest number less than " << nFinalBin << "that counts it .." << std::endl;
            int n = 1;
            for (int i = 2; i < nFinalBin; i++) {
                if ((h->GetXaxis()->GetNbins() % i) == 0)
                    n = i;
            }
            h->Rebin(((h->GetXaxis()->GetNbins()) / n));
        }
        //    cout<<"Nbins after rebinning: "<<h->GetXaxis()->GetNbins()<<endl;
        return h;
    }

    string Wpol_all_location;
    std::map<std::string, double> files_xsec;
    std::map<std::string, TFile*> files;
    std::map<string, double> Channels;
};

//CHECK for max_size

class DistributionProducerFromSelected {
public:

    DistributionProducerFromSelected(TH1* hSelected, string MCName, string channel, SamplesInfo& mySampleInfo) :
    hInput(hSelected), mcName(MCName) {

        TRandom RandomGenerator(SeedGenerator.Integer(10000000));
        for (int iBin = 0; iBin < hSelected->GetXaxis()->GetNbins(); iBin++) {
            double cosTheta = hSelected->GetBinCenter(iBin + 1);
            //            double cosTheta = hSelected->GetBinCenter(iBin);
            //            cout<<"cosTheta is "<<cosTheta<<endl;
            //            cout<<"BinContents of "<<iBin<<" is "<<hSelected->GetBinContent( iBin+1 )<<endl;
            for (int eventID = 0; eventID < hSelected->GetBinContent(iBin + 1); eventID++) {
                int evtRndId = RandomGenerator.Integer(1000000000);
                while (sampleContent.count(evtRndId) > 0) {
                    evtRndId = RandomGenerator.Integer(1000000000);
                }
                sampleContent[evtRndId] = cosTheta;
            }
        }

        Lumi = mySampleInfo.Channels[channel];
        Xsec = mySampleInfo.files_xsec[MCName];
        N0 = mySampleInfo.ReadN0(MCName, channel);
        selEff = (double) sampleContent.size() / (double) N0;
    }

    ~DistributionProducerFromSelected() {
    }

    TH1* GeneratePartialSample(double fraction, int nPEX) {
        TRandom RandomGenerator(SeedGenerator.Integer(10000000));
        std::vector<double> selectedValues;
        std::map<int, double>::iterator evtIter = sampleContent.begin();
        for (; evtIter != sampleContent.end(); evtIter++) {
            if (RandomGenerator.Uniform() < fraction)
                selectedValues.push_back(evtIter->second);
        }
        //        cout<<"------------ "<<selectedValues.size()<<endl;
        //        cout<<"------------ "<<selectedValues.at(0)<<endl;
        //        cout<<"------------ "<<selectedValues.at(1)<<endl;
        double Weight = 0.0;
        if (selectedValues.size() != 0) {
            Weight = (double) (Lumi * Xsec) / (double) (fraction * N0);
            //            cout<<"Weight: "<<Weight<<endl;
        }
        stringstream s;
        s << mcName << "_" << hInput->GetName() << "_" << nPEX;
        string hName = s.str();
        s.str("");
        s << mcName << "_" << hInput->GetTitle() << "_" << nPEX;
        string hTitle = s.str();
        gROOT->cd();
        TH1* hRet = new TH1D(hName.c_str(), hTitle.c_str(), hInput->GetXaxis()->GetNbins()
                , hInput->GetXaxis()->GetXmin(), hInput->GetXaxis()->GetXmax());
        hRet->Sumw2();
        for (unsigned int i = 0; i < selectedValues.size(); i++)
            hRet->Fill(selectedValues.at(i), Weight);

        return hRet;
    }

    TH1* GeneratePartialSampleLumiEQ(int nPEX) {
        TRandom RandomGenerator(SeedGeneratorLumiEQ.Integer(10000000));
        double nSelectedEventsInLumi_ = Lumi * Xsec*selEff;
        double nSelectedEventsInLumi = RandomGenerator.Gaus(nSelectedEventsInLumi_, sqrt(nSelectedEventsInLumi_));
        unsigned int EventIndices = sampleContent.size();
        std::vector<double> selectedValues;
        std::map<int, double>::iterator evtIter = sampleContent.begin();

        for (int nEvt = 0; nEvt < int(nSelectedEventsInLumi); nEvt++) {
            int EvtID = RandomGenerator.Integer(EventIndices);
            evtIter = sampleContent.begin();
            for (int i = 0; i < EvtID; i++)
                evtIter++;
            selectedValues.push_back(evtIter->second);
        }

        stringstream s;
        s << mcName << "_" << hInput->GetName() << "_" << nPEX;
        string hName = s.str();
        s.str("");
        s << mcName << "_" << hInput->GetTitle() << "_" << nPEX;
        string hTitle = s.str();
        gROOT->cd();
        TH1* hRet = new TH1D(hName.c_str(), hTitle.c_str(), hInput->GetXaxis()->GetNbins()
                , hInput->GetXaxis()->GetXmin(), hInput->GetXaxis()->GetXmax());
        hRet->Sumw2();
        for (unsigned int i = 0; i < selectedValues.size(); i++)
            hRet->Fill(selectedValues.at(i));

        return hRet;
    }
private:
    TH1* hInput;
    string mcName;
    double Lumi;
    std::map<int, double> sampleContent;
    double Xsec;
    int N0;
    double selEff;
};
#endif	/* FITVALIDATOR_H */

