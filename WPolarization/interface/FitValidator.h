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

    string TTBarName;
    SamplesInfo(string __Wpol_all_location , string ttbarName = "TTBarSummer2011") : Wpol_all_location(__Wpol_all_location) , TTBarName(ttbarName){
        files_xsec[ttbarName] = 157.5;
        files_xsec["DYSummer2011"] = 3048.0;
        files_xsec["WJetsSummer2011"] = 31314.0;
//        files_xsec["SysZJetsQD"] = 3048.0;
//        files_xsec["SysWJetsQD"] = 31314.0;
        files_xsec["WWSummer2011"] = 4.65;
        files_xsec["SingleTopSummer2011"] = 64.5;
        files_xsec["SingleTopTWSummer2011"] =  2 * 7.9;
        files_xsec["WZSummer2011"] = 0.6;
        files_xsec["ZZSummer2011"] = 4.65;

        map<string, double> files_xsec_temp;
        for(map<string, double>::iterator itr = files_xsec.begin() ; itr != files_xsec.end() ; itr ++)
            files_xsec_temp[ itr->first + "_BSFDN" ] = itr->second;
        
        TTBarName += "_BSFDN";
        files_xsec.clear();
        files_xsec = files_xsec_temp;
        
        Channels["EE"] = 4529.518;
        Channels["MM"] = 4459.007;
        Channels["EM"] = 4631.724;
    }

    ~SamplesInfo() {
    };

    struct ttbarSampleInfo {
    public:
        string Name;
        double WEE;
        double WEM;
        double WMM;

        ttbarSampleInfo(string name, double wee, double wem, double wmm) {
            Name = name;
            WEE = wee;
            WEM = wem;
            WMM = wmm;
        }
    };
    ttbarSampleInfo GetTTBarInfo(string Name){ //usefull for systematics
        double N0 = ReadN0(Name , "EE");
        double lval = 157.5 / N0;
        ttbarSampleInfo ret(Name , Channels["EE"]*lval , Channels["EM"]*lval  , Channels["MM"]*lval );
        return ret;
    }

    double ReadN0(string Sample, string Channel) const {
        int pos_ = Sample.find("_");
        if(pos_ != string::npos){
            Sample = Sample.substr(0 , pos_);
        }
        
        
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
            files[name] = TFile::Open(("./WPol_" + name + ".root").c_str(), "READ");

        //cout << "./WPol_SelectedTTBars_" + name + ".root" << endl;

        boost::algorithm::to_lower(Channel);
        TH1* h = (TH1*) files[name]->Get(("costheta_" + Channel + "/hCosThetaAllLepton").c_str());
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

    TH2* GetCosTheta2DPlot(string Channel, int nFinalBin = 10) {
        //cout << name << endl;
        string name = TTBarName;
        if (files.count(name) == 0)
            files[name] = TFile::Open(("./WPol_" + name + ".root").c_str(), "READ");

        //cout << "./WPol_SelectedTTBars_" + name + ".root" << endl;

        boost::algorithm::to_lower(Channel);
        TH2* h = (TH2*) files[name]->Get(("costheta_" + Channel + "/hCosThetaAllGenVsREC").c_str());
        cout << ("costheta_" + Channel + "/hCosThetaAllGenVsREC") << endl;
        if ((h->GetXaxis()->GetNbins()) == nFinalBin)
            return h;
        if ((h->GetXaxis()->GetNbins() % nFinalBin) == 0) {
            h->RebinX(((h->GetXaxis()->GetNbins()) / nFinalBin));
        } else {
            std::cout << nFinalBin << " does not count " << h->GetXaxis()->GetNbins() << std::endl;
            std::cout << "I will take the biggest number less than " << nFinalBin << "that counts it .." << std::endl;
            int n = 1;
            for (int i = 2; i < nFinalBin; i++) {
                if ((h->GetXaxis()->GetNbins() % i) == 0)
                    n = i;
            }
            h->RebinX(((h->GetXaxis()->GetNbins()) / n));
        }
        //    cout<<"Nbins after rebinning: "<<h->GetXaxis()->GetNbins()<<endl;

        h->RebinY(10);

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

    DistributionProducerFromSelected(TH1* hSelected, string MCName, string channel, SamplesInfo& mySampleInfo, bool isTwoD = false) :
    hInput(hSelected), mcName(MCName), isTWOD(isTwoD) {

        if (isTWOD) {
            TRandom RandomGenerator(SeedGenerator.Integer(10000000));
            for (int iBin = 0; iBin < hSelected->GetXaxis()->GetNbins(); iBin++)
                for (int jBin = 0; jBin < hSelected->GetYaxis()->GetNbins(); jBin++) {
                    double cosTheta = hSelected->GetXaxis()->GetBinCenter(iBin + 1);
                    double cosThetaGen = hSelected->GetYaxis()->GetBinCenter(jBin + 1);
                    //            double cosTheta = hSelected->GetBinCenter(iBin);
                    //            cout<<"cosTheta is "<<cosTheta<<endl;
                    //            cout<<"BinContents of "<<iBin<<" is "<<hSelected->GetBinContent( iBin+1 )<<endl;
                    for (int eventID = 0; eventID < hSelected->GetBinContent(iBin + 1, jBin + 1); eventID++) {
                        int evtRndId = RandomGenerator.Integer(1000000000);
                        while (sampleContent.count(evtRndId) > 0) {
                            evtRndId = RandomGenerator.Integer(1000000000);
                        }
                        sampleContent[evtRndId] = cosTheta;
                        sampleContentGen[evtRndId] = cosThetaGen;
                    }
                }
        } else {
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
        std::vector<int> selectedValues;
        std::map<int, double>::iterator evtIter = sampleContent.begin();
        for (; evtIter != sampleContent.end(); evtIter++) {
            if (RandomGenerator.Uniform() < fraction)
                selectedValues.push_back(evtIter->first);
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
        TH1* hRet;

        if (this->isTWOD) {
            hRet = new TH2D(hName.c_str(), hTitle.c_str(), hInput->GetXaxis()->GetNbins()
                    , hInput->GetXaxis()->GetXmin(), hInput->GetXaxis()->GetXmax(), hInput->GetYaxis()->GetNbins()
                    , hInput->GetYaxis()->GetXmin(), hInput->GetYaxis()->GetXmax());
            hRet->Sumw2();
            for (unsigned int i = 0; i < selectedValues.size(); i++)
                ((TH2*) hRet)->Fill(sampleContent[selectedValues.at(i)], sampleContentGen[selectedValues.at(i)], Weight);
        } else {
            hRet = new TH1D(hName.c_str(), hTitle.c_str(), hInput->GetXaxis()->GetNbins()
                    , hInput->GetXaxis()->GetXmin(), hInput->GetXaxis()->GetXmax());
            hRet->Sumw2();
            for (unsigned int i = 0; i < selectedValues.size(); i++)
                hRet->Fill(sampleContent[selectedValues.at(i)], Weight);
        }
        return hRet;
    }

    TH1* GeneratePartialSampleLumiEQ(int nPEX) {
        TRandom RandomGenerator(SeedGeneratorLumiEQ.Integer(10000000));
        double nSelectedEventsInLumi_ = Lumi * Xsec*selEff;
        double nSelectedEventsInLumi = RandomGenerator.Gaus(nSelectedEventsInLumi_, sqrt(nSelectedEventsInLumi_));
        unsigned int EventIndices = sampleContent.size();
        std::vector<int> selectedValues;
        std::map<int, double>::iterator evtIter = sampleContent.begin();

        for (int nEvt = 0; nEvt < int(nSelectedEventsInLumi); nEvt++) {
            int EvtID = RandomGenerator.Integer(EventIndices);
            evtIter = sampleContent.begin();
            for (int i = 0; i < EvtID; i++)
                evtIter++;
            selectedValues.push_back(evtIter->first);
        }

        stringstream s;
        s << mcName << "_" << hInput->GetName() << "_" << nPEX;
        string hName = s.str();
        s.str("");
        s << mcName << "_" << hInput->GetTitle() << "_" << nPEX;
        string hTitle = s.str();
        gROOT->cd();
        TH1 * hRet;
        if (this->isTWOD) {
            hRet = new TH2D(hName.c_str(), hTitle.c_str(), hInput->GetXaxis()->GetNbins()
                    , hInput->GetXaxis()->GetXmin(), hInput->GetXaxis()->GetXmax(), hInput->GetYaxis()->GetNbins()
                    , hInput->GetYaxis()->GetXmin(), hInput->GetYaxis()->GetXmax());
            hRet->Sumw2();
            for (unsigned int i = 0; i < selectedValues.size(); i++)
                ((TH2*) hRet)->Fill(sampleContent[selectedValues.at(i)], sampleContentGen[selectedValues.at(i)]);
        } else {
            hRet = new TH1D(hName.c_str(), hTitle.c_str(), hInput->GetXaxis()->GetNbins()
                    , hInput->GetXaxis()->GetXmin(), hInput->GetXaxis()->GetXmax());
            hRet->Sumw2();
            for (unsigned int i = 0; i < selectedValues.size(); i++)
                hRet->Fill(sampleContent[selectedValues.at(i)]);
        }
        return hRet;
    }

    bool isTWOD;

private:
    TH1* hInput;
    string mcName;
    double Lumi;
    std::map<int, double> sampleContent;
    std::map<int, double> sampleContentGen;
    double Xsec;
    int N0;
    double selEff;
};
#endif	/* FITVALIDATOR_H */

