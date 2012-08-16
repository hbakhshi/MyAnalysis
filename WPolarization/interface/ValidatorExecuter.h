/* 
 * File:   ValidatorExecuter.h
 * Author: nadjieh
 *
 * Created on May 3, 2012, 1:55 PM
 */

#ifndef VALIDATOREXECUTER_H
#define	VALIDATOREXECUTER_H
using namespace std;
#include "FitValidator.h"
#include "ToyFitter.h"
#include "TF1.h"
#include "TF3.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TRandom1.h"
#include "TRandom.h"
#include "TROOT.h"
#include "TFile.h"
#include "TVirtualFitter.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

void RunFitValidation(int StartPEX, int LPEX, int StartPEXPull, int LPEXPull, string Channel, string AllEventsHistogramDirectory) {
    stringstream outName;
    outName << "FitValidation/Linearity_start-" << StartPEX << "-length-" << LPEX << "_Pull_start-" << StartPEXPull <<
            "-length-" << LPEXPull << ".root";

    double FposFixed = 0.0009;
    std::pair<TF1, WeightFunctionCreator*> WtbWeightor = WeightFunctionCreator::getWeightFunction("WtbWeightor");
    SamplesInfo mySampleInfo(AllEventsHistogramDirectory);
    double Lumi_ee = mySampleInfo.Channels["EE"];
    double Lumi_em = mySampleInfo.Channels["EM"];
    double Lumi_mm = mySampleInfo.Channels["MM"];
    std::map<string, DistributionProducerFromSelected*> bkg_samples_ee;
    std::map<string, DistributionProducerFromSelected*> bkg_samples_mm;
    std::map<string, DistributionProducerFromSelected*> bkg_samples_em;
    std::map<string, DistributionProducerFromSelected*> bkg_samples;
    std::map<string, DistributionProducerFromSelected*> signal_samples_ee;
    std::map<string, DistributionProducerFromSelected*> signal_samples_mm;
    std::map<string, DistributionProducerFromSelected*> signal_samples_em;
    std::map<string, DistributionProducerFromSelected*> signal_samples;
    std::map<string, double>::iterator sampleItr = mySampleInfo.files_xsec.begin();

    TH2* signalMC = 0;
    TH1* bkg = 0;
    for (; sampleItr != mySampleInfo.files_xsec.end(); sampleItr++) {

        TH1* hist_ee = mySampleInfo.GetCosThetaPlot("EE", sampleItr->first, 10);
        hist_ee->Sumw2();
        DistributionProducerFromSelected *myDist_ee = new DistributionProducerFromSelected(hist_ee, sampleItr->first, "EE", mySampleInfo);
        TH1* hist_mm = mySampleInfo.GetCosThetaPlot("MM", sampleItr->first, 10);
        hist_mm->Sumw2();
        DistributionProducerFromSelected *myDist_mm = new DistributionProducerFromSelected(hist_mm, sampleItr->first, "MM", mySampleInfo);
        TH1* hist_em = mySampleInfo.GetCosThetaPlot("EM", sampleItr->first, 10);
        hist_em->Sumw2();
        DistributionProducerFromSelected *myDist_em = new DistributionProducerFromSelected(hist_em, sampleItr->first, "EM", mySampleInfo);

        bkg_samples_ee[sampleItr->first] = myDist_ee;
        bkg_samples_mm[sampleItr->first] = myDist_mm;
        bkg_samples_em[sampleItr->first] = myDist_em;

        if (Channel == "EE")
            bkg_samples[sampleItr->first] = myDist_ee;
        else if (Channel == "EM")
            bkg_samples[sampleItr->first] = myDist_em;
        else if (Channel == "MM")
            bkg_samples[sampleItr->first] = myDist_mm;
        else if (Channel == "Combined") {
            bkg_samples[sampleItr->first + "mm"] = myDist_mm;
            bkg_samples[sampleItr->first + "ee"] = myDist_ee;
            bkg_samples[sampleItr->first + "em"] = myDist_em;
        }

        hist_ee->Scale(float(Lumi_ee * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "EE")));
        hist_mm->Scale(float(Lumi_mm * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "MM")));
        hist_em->Scale(float(Lumi_em * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "EM")));
        //           if(sampleItr == mySampleInfo.Xsections.begin())
        if (bkg == 0) {
            if (Channel == "EE")
                bkg = ((TH1*) hist_ee->Clone(string("bkg_" + string(hist_ee->GetName())).c_str()));
            else if (Channel == "EM")
                bkg = ((TH1*) hist_em->Clone(string("bkg_" + string(hist_em->GetName())).c_str()));
            else if (Channel == "MM")
                bkg = ((TH1*) hist_mm->Clone(string("bkg_" + string(hist_mm->GetName())).c_str()));
            else if (Channel == "Combined") {
                bkg = ((TH1*) hist_ee->Clone(string("bkg_" + string(hist_ee->GetName())).c_str()));
                bkg->Add(hist_mm);
                bkg->Add(hist_em);
            }
        } else {
            if (Channel == "EE")
                bkg->Add(hist_ee);
            else if (Channel == "EM")
                bkg->Add(hist_em);
            else if (Channel == "MM")
                bkg->Add(hist_mm);
            else if (Channel == "Combined") {
                bkg->Add(hist_ee);
                bkg->Add(hist_mm);
                bkg->Add(hist_em);
            }
        }

        if ((sampleItr->first.find("TTBarSummer2011") != string::npos)) {

            TH2* hist_ee2d = mySampleInfo.GetCosTheta2DPlot("EE", 10);
            hist_ee2d->Sumw2();
            DistributionProducerFromSelected *myDist_ee_signal = new DistributionProducerFromSelected(hist_ee2d, sampleItr->first + "Signal", "EE", mySampleInfo, true);

            TH2* hist_mm2d = mySampleInfo.GetCosTheta2DPlot("MM", 10);
            hist_mm2d->Sumw2();
            DistributionProducerFromSelected *myDist_mm_signal = new DistributionProducerFromSelected(hist_mm2d, sampleItr->first + "Signal", "MM", mySampleInfo, true);

            TH2* hist_em2d = mySampleInfo.GetCosTheta2DPlot("EM", 10);
            hist_em2d->Sumw2();
            DistributionProducerFromSelected *myDist_em_signal = new DistributionProducerFromSelected(hist_em2d, sampleItr->first + "Signal", "EM", mySampleInfo, true);

            //            cout<<sampleItr->first<<endl;
            signal_samples_ee[sampleItr->first] = myDist_ee;
            signal_samples_em[sampleItr->first] = myDist_em;
            signal_samples_mm[sampleItr->first] = myDist_mm;

            if (Channel == "EE")
                signal_samples[sampleItr->first] = myDist_ee;
            else if (Channel == "EM")
                signal_samples[sampleItr->first] = myDist_em;
            else if (Channel == "MM")
                signal_samples[sampleItr->first] = myDist_mm;
            else if (Channel == "Combined") {
                signal_samples[sampleItr->first + "mm"] = myDist_mm;
                signal_samples[sampleItr->first + "ee"] = myDist_ee;
                signal_samples[sampleItr->first + "em"] = myDist_em;
            }

            hist_ee2d->Scale(float(Lumi_ee * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "EE")));
            hist_mm2d->Scale(float(Lumi_mm * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "MM")));
            hist_em2d->Scale(float(Lumi_em * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "EM")));
            //            if(sampleItr == mySampleInfo.Xsections.begin())
            if (signalMC == 0) {
                if (Channel == "EE")
                    signalMC = ((TH2*) hist_ee2d->Clone(string("signal_" + string(hist_ee->GetName())).c_str()));
                else if (Channel == "EM")
                    signalMC = ((TH2*) hist_em2d->Clone(string("signal_" + string(hist_em->GetName())).c_str()));
                else if (Channel == "MM")
                    signalMC = ((TH2*) hist_mm2d->Clone(string("signal_" + string(hist_mm->GetName())).c_str()));
                else if (Channel == "Combined") {
                    signalMC = ((TH2*) hist_ee2d->Clone(string("signal_" + string(hist_ee->GetName())).c_str()));
                    signalMC->Add(hist_mm2d);
                    signalMC->Add(hist_em2d);
                }
            } else {
                if (Channel == "EE")
                    signalMC->Add(hist_ee2d);
                else if (Channel == "EM")
                    signalMC->Add(hist_em2d);
                else if (Channel == "MM")
                    signalMC->Add(hist_mm2d);
                else if (Channel == "Combined") {
                    signalMC->Add(hist_ee);
                    signalMC->Add(hist_mm2d);
                    signalMC->Add(hist_em2d);
                }
            }
        }
    }
    double correlation = 0.0;
    gROOT->cd();
    TH2D hFinalFNeg("hFinalFNeg", "Linearirty Check for F_{-};F_{-} input;F_{-} output", 300, 0., 1., 300, 0., 1.);
    TH2D hFinalF0("hFinalF0", "Linearirty Check for F_{0};F_{0} input;F_{0} output", 300, 0., 1., 300, 0., 1.);
    TH2D hFinalFPos("hFinalFPos", "Linearirty Check for F_{+};nPEX;F_{+} output", 300, 0., 300., 1000, -0.5, 0.5);
    int nFSteps = 300;

    double FNegValueSteps[nFSteps];
    for (int i = 0; i < nFSteps; i++) {
        FNegValueSteps[i] = float(i) / (float) nFSteps;
    }
    for (int nPEX = 0; nPEX < 300; nPEX++) {
        cout << "PEX: " << nPEX << endl;

        if (nPEX >= (StartPEX + LPEX)) {
            cout << " ... Skipped" << endl;
            continue;
        }

        if (nPEX < StartPEX) {
            for (unsigned int i = 0; i < mySampleInfo.files_xsec.size(); i++)
                SeedGenerator.Integer(10000);
            cout << " ... Skipped" << endl;
            continue;
        }


        stringstream name__;
        name__ << "CosTheta_bkg_" << nPEX;
        stringstream title__;
        title__ << "noWtb cos(#theta) : PEX=" << nPEX;
        TH1* hSumBGPartial = new TH1D(name__.str().c_str(), title__.str().c_str(), signalMC->GetXaxis()->GetNbins()
                , signalMC->GetXaxis()->GetXmin(), signalMC->GetXaxis()->GetXmax());
        hSumBGPartial->Sumw2();
        std::map<string, DistributionProducerFromSelected*>::iterator bkgItr = bkg_samples.begin();
        for (; bkgItr != bkg_samples.end(); bkgItr++) {
            //            cout<< "..." << bkgItr->first << "..." <<endl;
            TH1* tmp = bkgItr->second->GeneratePartialSample(1.0 / 3.0, nPEX);
            hSumBGPartial->Add(tmp);
            delete tmp;
        }
        //        for(int mybin = 0; mybin < hSumBGPartial->GetXaxis()->GetNbins(); mybin++)
        //            cout<<"hSumBGPartial: "<<hSumBGPartial->GetBinContent(mybin+1)<<endl;
        name__.str("");
        title__.str("");
        name__ << "CosTheta_signal_" << nPEX;
        title__ << "Wtb cos(#theta) : PEX=" << nPEX;
        TH2* hSumSIGPartial = new TH2D(name__.str().c_str(), title__.str().c_str(), signalMC->GetXaxis()->GetNbins()
                , signalMC->GetXaxis()->GetXmin(), signalMC->GetXaxis()->GetXmax(), signalMC->GetXaxis()->GetNbins()
                , signalMC->GetXaxis()->GetXmin(), signalMC->GetXaxis()->GetXmax());
        hSumSIGPartial->Sumw2();
        std::map<string, DistributionProducerFromSelected*> ::iterator sigItr = signal_samples.begin();
        for (; sigItr != signal_samples.end(); sigItr++) {
            //            cout<< "..." << sigItr->first << "..." <<endl;
            TH2* tmp = dynamic_cast<TH2*> (sigItr->second->GeneratePartialSample(1.0 / 3.0, nPEX));
            hSumSIGPartial->Add(tmp);
            delete tmp;
        }

        for (int i = 0; i < nFSteps; i++) {
            //            if ((FNegValueSteps[i]*nFSteps) % 50.0 == 0)
            //                cout<< "\tFStep: "<<FNegValueSteps[i]<<endl;
            if (FNegValueSteps[i] > (1.0 - FposFixed))
                continue;

            double F0Value = 1.0 - FposFixed - FNegValueSteps[i];

            WtbWeightor.first.SetParameters(F0Value, FNegValueSteps[i]);

            name__.str("");
            name__ << hSumBGPartial->GetName() << "_" << FNegValueSteps[i];
            title__.str("");
            title__ << hSumBGPartial->GetTitle() << " for F_ = " << FNegValueSteps[i];

            TH1* SIGinPEX = new TH1D(name__.str().c_str(), title__.str().c_str(), hSumSIGPartial->GetYaxis()->GetNbins(), hSumSIGPartial->GetYaxis()->GetXmin(), hSumSIGPartial->GetYaxis()->GetXmax());
            for (int bin_id__ = 1; bin_id__ < SIGinPEX->GetNbinsX() + 1; bin_id__++) {
                TH1* hproject = hSumSIGPartial->ProjectionY("_py", bin_id__, bin_id__, "o");
                hproject->Multiply(&WtbWeightor.first, 1);
                SIGinPEX->SetBinContent(bin_id__, hproject->Integral());
                delete hproject;
            }

            //            for(int mybin = 0; mybin < SIGinPEX->GetXaxis()->GetNbins(); mybin++)
            //                cout<<"SIGinPEX: "<<SIGinPEX->GetBinContent(mybin+1)<<endl;
            // SIGinPEX->Multiply(&WtbWeightor.first, 1);
#ifdef TEST
            TCanvas c;
            c.Divide(3);
            c.cd(1);
            WtbWeightor.first.Draw();
            c.cd(2);
            hSumSIGPartial->Draw();
            c.cd(3);
            hSumBGPartial->Draw();
            stringstream canvNme;
            canvNme << FNegValueSteps[i] << "_" << F0Value << "_";
            c.SaveAs(string(canvNme.str() + string(WtbWeightor.first.GetName()) + ".C").c_str());
#endif /*TEST*/
            //            cout<<"-----------------"<<endl;
            //            for(int mybin = 0; mybin < SIGinPEX->GetXaxis()->GetNbins(); mybin++)
            //                cout<<"SIGinPEX: "<<SIGinPEX->GetBinContent(mybin+1)<<endl;
            SIGinPEX->Add(hSumBGPartial);
            //            cout<<"After addition:"<<endl;
            //            for(int mybin = 0; mybin < SIGinPEX->GetXaxis()->GetNbins(); mybin++)
            //                cout<<"SIGinPEX: "<<SIGinPEX->GetBinContent(mybin+1)<<endl;
            std::pair<TF3, LikelihoodFunction*> LLinPEXforFNegValueArray = LikelihoodFunction::getLLFunction(
                    string("F_" + name__.str()), bkg, SIGinPEX, signalMC, true);
            TF3 LLinPEXforFNegValue = LLinPEXforFNegValueArray.first;
            //            for(int as = 0; as <1; as+=0.01)
            //                cout<<LLinPEXforFNegValue.Eval(as,1-as,1);

            double x[3] = {-1., -1., -1.};
            double xerr[3] = {-1., -1., -1.};
            GetMinimum(LLinPEXforFNegValue, x, xerr, correlation, false);
            hFinalFNeg.Fill(FNegValueSteps[i], x[1]);
            hFinalF0.Fill(F0Value, x[0]);
            //            hFinalFNeg.Fill(FNegValueSteps[i],x[0]);
            //            hFinalF0.Fill(F0Value,x[1]);
            hFinalFPos.Fill(nPEX, 1.0 - x[0] - x[1]);
            delete SIGinPEX;
            delete LLinPEXforFNegValueArray.second;
        }
        delete hSumBGPartial;
        delete hSumSIGPartial;
    }
    TH1D hPullFNeg("hPullFNeg", "Pull distribution for F_{-}", 1000, -5, 5);
    TH1D hPullF0("hPullF0", "Pull distribution for F_{0}", 1000, -5, 5);
    TH1D hPullFPos("hPullFPos", "Pull distribution for F_{+}", 1000, -5, 5);
    TH1D hPullFResp("hPullFResp", "Pull distribution for F_{resp}", 1000, -5, 5);

    TH1D hResFNeg("hResFNeg", "Residual for F_{-}", 1000, -0.5, 0.5);
    TH1D hResF0("hResF0", "Residual for F_{0}", 1000, -0.5, 0.5);
    TH1D hResFPos("hResFPos", "Residual for F_{+}", 1000, -0.5, 0.5);
    TH1D hResFResp("hResFResp", "Residual for F_{resp}", 1000, -0.5, 0.5);
    cout << StartPEXPull << "  " << LPEXPull << endl;
    for (int nPEXPull = 0; nPEXPull < 1000; nPEXPull++) {
        //        cout<< "PEXPull:"<<nPEXPull<<endl;

        if (nPEXPull >= (StartPEXPull + LPEXPull)) {
            cout << " ... Skipped pull" << endl;
            continue;
        }

        if (nPEXPull < StartPEXPull) {
            for (unsigned int i = 0; i < mySampleInfo.files_xsec.size(); i++)
                SeedGeneratorLumiEQ.Integer(10000);
            cout << " ... Skipped pull" << endl;
            continue;
        }

        cout << "PEXPull:" << nPEXPull << endl;

        stringstream name__;
        name__ << "CosTheta_Pull_" << nPEXPull;
        stringstream title__;
        title__ << "cos(#theta) : PEXPull=" << nPEXPull;

        TH1* hData = new TH1D(name__.str().c_str(), title__.str().c_str(), signalMC->GetXaxis()->GetNbins(),
                signalMC->GetXaxis()->GetXmin(), signalMC->GetXaxis()->GetXmax());
        hData->Sumw2();
        std::map<string, DistributionProducerFromSelected*>::iterator bkgItr = bkg_samples.begin();
        for (; bkgItr != bkg_samples.end(); bkgItr++) {
            //            cout<< "..." << bkgItr->first<< "..." <<endl;
            TH1* tmp = bkgItr->second->GeneratePartialSampleLumiEQ(nPEXPull);
            hData->Add(tmp);
            delete tmp;
        }
        std::map<string, DistributionProducerFromSelected*>::iterator sigItr = signal_samples.begin();
        for (; sigItr != signal_samples.end(); sigItr++) {
            //            cout<< "..." << sigItr->first<< "..." <<endl;
            TH2* tmp = dynamic_cast<TH2*> (sigItr->second->GeneratePartialSampleLumiEQ(nPEXPull));
            TH1* tmp2 = tmp->ProjectionX();
            hData->Add(tmp2);
            delete tmp;
            delete tmp2;
        }

        std::pair<TF3, LikelihoodFunction*> LLinPEXforFNegValueArray = LikelihoodFunction::getLLFunction(
                string("F_" + name__.str()), bkg, hData, signalMC, true);
        TF3 LLinPEXforFNegValue = LLinPEXforFNegValueArray.first;
        double x[3] = {-1., -1., -1.};
        double xerr[3] = {-1., -1., -1.};
        int fitres = GetMinimum(LLinPEXforFNegValue, x, xerr, correlation);
        if (fitres != 0)
            continue;
        double fneg = x[1];
        double f0 = x[0];
        double fpos = 1.0 - x[1] - x[0];

        double fresponse = x[2];
        double errfrespo = xerr[2];
        double resfresp0 = fresponse - 1.0;

        hPullFResp.Fill((double) resfresp0 / (double) errfrespo);
        hResFResp.Fill(resfresp0);

        double errfneg = xerr[1];
        double errf0 = xerr[0];
        double errfpos = sqrt(errf0 * errf0 + errfneg * errfneg + (2 * correlation));

        double resneg = fneg - 0.3;
        double respos = fpos - FposFixed;
        double resf0 = f0 - (0.7 - FposFixed);

        hPullFNeg.Fill((double) resneg / (double) errfneg);
        hPullF0.Fill((double) resf0 / (double) errf0);
        hPullFPos.Fill((double) respos / (double) errfpos);

        hResFPos.Fill(respos);
        hResFNeg.Fill(resneg);
        hResF0.Fill(resf0);

        delete hData;
        delete LLinPEXforFNegValueArray.second;
    }
    delete WtbWeightor.second;
    TFile * outFile = new TFile(outName.str().c_str(), "recreate");
    outFile->cd();
    hFinalF0.Write();
    hFinalFNeg.Write();
    hFinalFPos.Write();
    hPullFNeg.Write();
    hPullF0.Write();
    hPullFPos.Write();
    hPullFResp.Write();
    hResFNeg.Write();
    hResF0.Write();
    hResFPos.Write();
    hResFResp.Write();

    signalMC->Write();
    bkg->Write();

    outFile->Close();
}

void RunFit(string Channel, string AllEventsHistogramDirectory, string ttbarname = "") {
    string treeFileName = "trees_" + ttbarname + "_BSFDN.root";
    SamplesInfo mySampleInfo(AllEventsHistogramDirectory , ttbarname);
    SamplesInfo::ttbarSampleInfo ttbarInfo = mySampleInfo.GetTTBarInfo(ttbarname);
    double wttee, wttmm, wttem;
    double Lumi_ee = mySampleInfo.Channels["EE"];
    double Lumi_em = mySampleInfo.Channels["EM"];
    double Lumi_mm = mySampleInfo.Channels["MM"];

    std::map<string, double>::iterator sampleItr = mySampleInfo.files_xsec.begin();

    TH2* signalMC = 0;
    TH1* bkg = 0;
    TH1* data = 0;
    for (; sampleItr != mySampleInfo.files_xsec.end(); sampleItr++) {

        TH1* hist_ee = mySampleInfo.GetCosThetaPlot("EE", sampleItr->first, 10);
        hist_ee->Sumw2();

        TH1* hist_mm = mySampleInfo.GetCosThetaPlot("MM", sampleItr->first, 10);
        hist_mm->Sumw2();

        TH1* hist_em = mySampleInfo.GetCosThetaPlot("EM", sampleItr->first, 10);
        hist_em->Sumw2();

        hist_ee->Scale(float(Lumi_ee * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "EE")));
        hist_mm->Scale(float(Lumi_mm * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "MM")));
        hist_em->Scale(float(Lumi_em * sampleItr->second) / float(mySampleInfo.ReadN0(sampleItr->first, "EM")));
        //           if(sampleItr == mySampleInfo.Xsections.begin())
        if (bkg == 0) {
            if (Channel == "EE")
                bkg = ((TH1*) hist_ee->Clone(string("bkg_" + string(hist_ee->GetName())).c_str()));
            else if (Channel == "EM")
                bkg = ((TH1*) hist_em->Clone(string("bkg_" + string(hist_em->GetName())).c_str()));
            else if (Channel == "MM")
                bkg = ((TH1*) hist_mm->Clone(string("bkg_" + string(hist_mm->GetName())).c_str()));
            else if (Channel == "Combined") {
                bkg = ((TH1*) hist_ee->Clone(string("bkg_" + string(hist_ee->GetName())).c_str()));
                bkg->Add(hist_mm);
                bkg->Add(hist_em);
            }
        } else {
            if (Channel == "EE")
                bkg->Add(hist_ee);
            else if (Channel == "EM")
                bkg->Add(hist_em);
            else if (Channel == "MM")
                bkg->Add(hist_mm);
            else if (Channel == "Combined") {
                bkg->Add(hist_ee);
                bkg->Add(hist_mm);
                bkg->Add(hist_em);
            }
        }

        if ((sampleItr->first.find(ttbarname) != string::npos)) {

            TH2* hist_ee2d = mySampleInfo.GetCosTheta2DPlot("EE", 10);
            hist_ee2d->Sumw2();

            TH2* hist_mm2d = mySampleInfo.GetCosTheta2DPlot("MM", 10);
            hist_mm2d->Sumw2();

            TH2* hist_em2d = mySampleInfo.GetCosTheta2DPlot("EM", 10);
            hist_em2d->Sumw2();

            wttee = ttbarInfo.WEE;
            wttmm = ttbarInfo.WMM;
            wttem = ttbarInfo.WEM;;

            hist_ee2d->Scale(wttee);
            hist_mm2d->Scale(wttmm);
            hist_em2d->Scale(wttem);
            //            if(sampleItr == mySampleInfo.Xsections.begin())
            if (signalMC == 0) {
                if (Channel == "EE")
                    signalMC = ((TH2*) hist_ee2d->Clone(string("signal_" + string(hist_ee->GetName())).c_str()));
                else if (Channel == "EM")
                    signalMC = ((TH2*) hist_em2d->Clone(string("signal_" + string(hist_em->GetName())).c_str()));
                else if (Channel == "MM")
                    signalMC = ((TH2*) hist_mm2d->Clone(string("signal_" + string(hist_mm->GetName())).c_str()));
                else if (Channel == "Combined") {
                    signalMC = ((TH2*) hist_ee2d->Clone(string("signal_" + string(hist_ee->GetName())).c_str()));
                    signalMC->Add(hist_mm2d);
                    signalMC->Add(hist_em2d);
                }
            } else {
                if (Channel == "EE")
                    signalMC->Add(hist_ee2d);
                else if (Channel == "EM")
                    signalMC->Add(hist_em2d);
                else if (Channel == "MM")
                    signalMC->Add(hist_mm2d);
                else if (Channel == "Combined") {
                    signalMC->Add(hist_ee2d);
                    signalMC->Add(hist_mm2d);
                    signalMC->Add(hist_em2d);
                }
            }
        }
    }

    map<string, string> data_names;
    data_names["EE"] = "DoubleEle2011";
    data_names["MM"] = "DoubleMuon2011";
    data_names["EM"] = "ElectronMuon2011";

    for (map<string, string>::const_iterator data_itr = data_names.begin(); data_itr != data_names.end(); data_itr++) {
        TH1* hist_ = mySampleInfo.GetCosThetaPlot(data_itr->first, data_itr->second, 10);
        hist_->Sumw2();

        if (data == 0) {
            if ((Channel == data_itr->first) || (Channel == "Combined"))
                data = ((TH1*) hist_->Clone(string("data_" + string(hist_->GetName())).c_str()));
        } else {
            if (Channel == "Combined") {
                data->Add(hist_);
            }
        }
    }


    std::vector<TH2*> allHistos;
    allHistos.push_back(signalMC);    
    
    std::pair<TF3, LikelihoodFunction*> LLinPEXforFNegValueArray = LikelihoodFunction::getLLFunction(
            "Fitter", bkg, data, signalMC, true);


    double x[3] = {-1., -1., -1.};
    double xerr[3] = {-1., -1., -1.};
    double correlation = 0.0;

    int fitres = GetMinimum(LLinPEXforFNegValueArray.first, x, xerr, correlation);
    double fneg = x[1];
    double f0 = x[0];
    double fpos = 1.0 - x[1] - x[0];

    double errfneg = xerr[1];
    double errf0 = xerr[0];
    double errfpos = sqrt(errf0 * errf0 + errfneg * errfneg + (2 * correlation));

    cout << "| Old Results";
    cout << "|" << fneg << "+-" << errfneg;
    cout << "|" << f0 << "+-" << errf0;
    cout << "|" << correlation;
    cout << "|" << fpos << "+-" << errfpos;
    cout << "|" << x[2] << "+-" << xerr[2] << "|" << endl;

    if (treeFileName != "") {
        LLinPEXforFNegValueArray.second->SetTreeFile(treeFileName , wttee , wttem , wttmm);

        fitres = GetMinimum(LLinPEXforFNegValueArray.first, x, xerr, correlation);

        fneg = x[1];
        f0 = x[0];
        fpos = 1.0 - x[1] - x[0];

        errfneg = xerr[1];
        errf0 = xerr[0];
        errfpos = sqrt(errf0 * errf0 + errfneg * errfneg + (2 * correlation));

        cout << "|" << 0.0;
        cout << "|" << fneg << "+-" << errfneg;
        cout << "|" << f0 << "+-" << errf0;
        cout << "|" << correlation;
        cout << "|" << fpos << "+-" << errfpos;
        cout << "|" << x[2] << "+-" << xerr[2] << "|" << endl;

        LLinPEXforFNegValueArray.second->useTree = false;

        int nBins[] = {10, 20, 50, 80, 100, 200, 400, 500, 1000, 2000, 5000, 10000};

        for (int i = 0; i < 12; i++) {
            allHistos.push_back( LLinPEXforFNegValueArray.second->setnsignal2DFromTreeBins(nBins[i]) );

            fitres = GetMinimum(LLinPEXforFNegValueArray.first, x, xerr, correlation);

            fneg = x[1];
            f0 = x[0];
            fpos = 1.0 - x[1] - x[0];

            errfneg = xerr[1];
            errf0 = xerr[0];
            errfpos = sqrt(errf0 * errf0 + errfneg * errfneg + (2 * correlation));

            cout << "|" << nBins[i];
            cout << "|" << fneg << "+-" << errfneg;
            cout << "|" << f0 << "+-" << errf0;
            cout << "|" << correlation;
            cout << "|" << fpos << "+-" << errfpos;
            cout << "|" << x[2] << "+-" << xerr[2] << "|" << endl;
        }
    }
    
    TFile f("all2dhistos.root" , "RECREATE");
    for(vector<TH2*>::const_iterator histo_itr = allHistos.begin() ; histo_itr != allHistos.end() ; histo_itr++)
        (*histo_itr)->Write();
    
    f.Close();
}
#endif	/* VALIDATOREXECUTER_H */

