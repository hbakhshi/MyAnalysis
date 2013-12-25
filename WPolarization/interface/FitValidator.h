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

#include "LHAPDF/LHAPDF.h"

#include "WPolarization/interface/PUWeightShifter.h"
class SamplesInfo {
public:

    string TTBarName;

    // wzqscale : >0 QU, <0 QD
    // sampletoreweight : > 0 *2 , < 0 *0
    //                  1 : DY, 2 : W, 3 : SingleTopTW, 4 : SignleTopTChannel    
    //                  5 : WW, 6 : WZ , 7 : ZZ
    // other_syst       : 0 Nothing, 1 : PU,2:SF,3:Trg, 100-141 : PDF+100
  int PdfSetID;

    SamplesInfo(string __Wpol_all_location, string ttbarName = "TTBarSummer2011", int WZQScale = 0, int sampleToReweight = 0, int other_syst = 0) : Wpol_all_location(__Wpol_all_location), TTBarName(ttbarName) {
        files_xsec[ttbarName] = 157.5;
        files_xsec["DY10To50Summer2011"] = 9611.0;

        string DYName = "DYSummer2011";
        string WName = "WJetsSummer2011";
        if (WZQScale < 0) {
            DYName = "SysZJetsQD";
            WName = "SysWJetsQD";
        } else if (WZQScale > 0) {
            DYName = "SysZJetsQU";
            WName = "SysWJetsQU";
        }
        files_xsec[DYName] = 3048.0;
        files_xsec[WName] = 31314.0;

        files_xsec["WWSummer2011"] = 4.65;
        files_xsec["SingleTopSummer2011"] = 64.5;
        files_xsec["SingleTopTWSummer2011"] = 2 * 7.9;
        files_xsec["WZSummer2011"] = 0.6;
        files_xsec["ZZSummer2011"] = 4.65;

        double weightOfSample = sampleToReweight > 0 ? 2.0 : 0.0;
        switch (abs(sampleToReweight)) {
            case 1:
                files_xsec[DYName] *= weightOfSample;
                //                cout << files_xsec[DYName] << endl;
                break;
            case 2:
                files_xsec[WName] *= weightOfSample;
                //                cout << files_xsec[WName] << endl;
                break;
            case 3:
                files_xsec["SingleTopTWSummer2011"] *= weightOfSample;
                //                cout << files_xsec["SingleTopTWSummer2011"] << endl;
                break;
            case 4:
                files_xsec["SingleTopSummer2011"] *= weightOfSample;
                //                cout << files_xsec["SingleTopSummer2011"] << endl;
                break;
            case 5:
                files_xsec["WWSummer2011"] *= weightOfSample;
                //                cout << files_xsec["WWSummer2011"] << endl;
                break;
            case 6:
                files_xsec["WZSummer2011"] *= weightOfSample;
                //                cout << files_xsec["WZSummer2011"] << endl;
                break;
            case 7:
                files_xsec["ZZSummer2011"] *= weightOfSample;
                //                cout << files_xsec["ZZSummer2011"] << endl;
                break;
        }



        //        map<string, double> files_xsec_temp;
        //        for(map<string, double>::iterator itr = files_xsec.begin() ; itr != files_xsec.end() ; itr ++)
        //            files_xsec_temp[ itr->first + "_BSFDN" ] = itr->second;
        //        
        //        TTBarName += "_BSFDN";
        //        files_xsec.clear();
        //        files_xsec = files_xsec_temp;

        Channels["EE"] = 4529.518;
        Channels["MM"] = 4459.007;
        Channels["EM"] = 4631.724;



        for (int pu = 0; pu < 100; pu++)
            PUWeights[pu] = 1.0;

        int abs_other_syst = abs(other_syst);
        switch (abs_other_syst) {
            case 1: //PU
            {
                reweight::PoissonMeanShifter shifter(other_syst * 0.5);
                for(int i = 0 ; i < 100 ; i++){
                    PUWeights[i] = shifter.ShiftWeight(i);
                }
            }
                break;
            case 2:
            {
                //isreseved for lepton SF systematics
                //it has been implemented in SetTreeFile method of the LLBase class
                int iiiiii_2222 = 1;
                iiiiii_2222++;
            }
                break;
            case 3:
            {
	      //trigger systematics
	      //it has been implemented in SetTreeFile method of the LLBase class
	      int iiiiii_22223 = 1;
	      iiiiii_22223++;
            }
                break;
        }
	if(abs_other_syst > 99 && abs_other_syst < 141)
	  PdfSetID = abs_other_syst - 100 ;
	else
	  PdfSetID = 0.0;
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

    ttbarSampleInfo GetTTBarInfo(string Name) { //usefull for systematics
        double N0 = ReadN0(Name, "EE");
        double lval = 157.5 / N0;
        ttbarSampleInfo ret(Name, Channels["EE"] * lval, Channels["EM"] * lval, Channels["MM"] * lval);
        return ret;
    }

    double ReadN0(string Sample, string Channel) const {
        int pos_ = Sample.find("_");
        if (pos_ != string::npos) {
            Sample = Sample.substr(0, pos_);
        }


        TFile* file = TFile::Open((Wpol_all_location + "/WPol_" + Sample + ".root").c_str(), "READ");
        string selectionPlotName = "Selection/hEventSelection" + Channel;
        TH1* hSelection = (TH1*) file->Get(selectionPlotName.c_str());
        double N0 = hSelection->GetBinContent(1);
        file->Close();
        return N0;
    }

  TH1* GetCosThetaPlotFromTrees(string Channel, string name) {
        gROOT->cd();
        TH1* ret = new TH1D(("from_tree_" + Channel + "_" + name + "_costheta").c_str(), "", 10, -1.0, 1.0);
        ret->Sumw2();

        int nFinalBin = 10;
        boost::algorithm::to_lower(Channel);

        std::pair<std::string, std::string> thePair = std::make_pair(name, Channel);
        if (tree_files.count(thePair) == 0)
            tree_files[thePair] = TFile::Open(("./tree_" + Channel + "_SelectedTTBars_" + name + ".root").c_str(), "READ");

       double genCosThetaValueHolder;
        double recCosThetaValueHolder;
        double eventWeight;
        bool isElectron;
        bool isFirstLepton;
        double LeptonPt;
        double LeptonEta;
        int nPU;
        int nPV;
        int eventType;
        int GenEventType;

	float  PDFScalePDF;
	float  PDFx1;
	float  PDFx2;
	float  PDFxPDF1;
	float  PDFxPDF2;
	int PDFID1;
	int PDFID2;

	LHAPDF::initLHAPDF();
	std::vector<LHAPDF::PDFSetInfo> pdfinfo = LHAPDF::getAllPDFSetInfo();

	std::vector<int> interestingpdfs;
	//std::cout << "loaded: " << pdfinfo.size() << " pdfs." << std::endl;
	// std::map<string , int> pdfstat;
	for(size_t ii=0; ii<pdfinfo.size(); ii++){
	  if(pdfinfo[ii].file=="cteq61.LHgrid"){
	    interestingpdfs.push_back(ii);

	    // if ( pdfstat.count( pdfinfo[ii].file ) > 0)
	    //   pdfstat[ pdfinfo[ii].file ] ++;
	    // else
	    //   pdfstat[ pdfinfo[ii].file ] = 0;

	    // cout << pdfinfo[ii] << endl;

	  }
	}
	// for( std::map<string , int>::const_iterator itr = pdfstat.begin() ; itr != pdfstat.end() ; itr++ )
	//   cout << itr->first << " : " << itr->second << endl;

	LHAPDF::setVerbosity ( LHAPDF::SILENT );
	LHAPDF::initPDFSet("/usr/share/lhapdf/PDFsets/" + pdfinfo[interestingpdfs[0]].file);
	//cout << interestingpdfs.size() << endl;
	

        for (int tree_id = 1; tree_id < nFinalBin + 1; tree_id++) {
            ostringstream treeName;

            treeName << "Tree_" << tree_id;
            TTree* tree = (TTree*) tree_files[thePair]->Get(treeName.str().c_str());

            tree->SetBranchAddress("GenCosTheta", &genCosThetaValueHolder);
            tree->SetBranchAddress("RecCosTheta", &recCosThetaValueHolder);

            tree->SetBranchAddress("EventWeight", &eventWeight);
            tree->SetBranchAddress("eventType", &eventType);
            tree->SetBranchAddress("GenEventType", &GenEventType);

            tree->SetBranchAddress("nPU", &nPU);
            tree->SetBranchAddress("nPV", &nPV);

            tree->SetBranchAddress("isElectron", &isElectron);
            tree->SetBranchAddress("isFirstLepton", &isFirstLepton);
            tree->SetBranchAddress("LeptonPt", &LeptonPt);
            tree->SetBranchAddress("LeptonEta", &LeptonEta);

	    tree->SetBranchAddress("PDFScalePDF", &PDFScalePDF);
	    tree->SetBranchAddress("PDFx1", &PDFx1);
	    tree->SetBranchAddress("PDFx2", &PDFx2);
	    //tree->SetBranchAddress("PDFxPDF1", &PDFxPDF1);
	    //tree->SetBranchAddress("PDFxPDF2", &PDFxPDF1);
	    tree->SetBranchAddress("PDFID1", &PDFID1);
	    tree->SetBranchAddress("PDFID2", &PDFID2);

            for (int entry = 0; entry < tree->GetEntries(); entry++) {
                tree->GetEntry(entry);
									
		LHAPDF::usePDFMember(0);
		PDFxPDF1 = LHAPDF::xfx(PDFx1 , PDFScalePDF , PDFID1 ) ; 
		PDFxPDF2 = LHAPDF::xfx(PDFx2 , PDFScalePDF , PDFID2 ) ; 
		
		LHAPDF::usePDFMember(PdfSetID);
		PDFxPDF1 = LHAPDF::xfx(PDFx1 , PDFScalePDF , PDFID1 ) / PDFxPDF1;
		PDFxPDF2 = LHAPDF::xfx(PDFx2 , PDFScalePDF , PDFID2 ) / PDFxPDF2;
		double pdf_weight = PDFxPDF2*PDFxPDF1 ; 
		
                //the trg SF needs to be added
                double trg_sf_weight = 0.0;
                if (isElectron) {
                    if (isFirstLepton) {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
                            if (fabs(LeptonEta) < 1.5)
                                trg_sf_weight = 0.9849;
                            else
                                trg_sf_weight = 0.9774;
                        } else {
                            if (fabs(LeptonEta) < 1.5)
                                trg_sf_weight = 0.9928;
                            else
                                trg_sf_weight = 0.9938;
                        }
                    } else {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
                            if (fabs(LeptonEta) < 1.5)
                                trg_sf_weight = 0.9923;
                            else
                                trg_sf_weight = 0.9953;
                        } else {
                            if (fabs(LeptonEta) < 1.5)
                                trg_sf_weight = 0.9948;
                            else
                                trg_sf_weight = 0.9956;
                        }
                    }
                } else {
                    if (isFirstLepton) {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
                            if (fabs(LeptonEta) < 0.8)
                                trg_sf_weight = 0.9648;
                            else if (fabs(LeptonEta) < 1.2)
                                trg_sf_weight = 0.9516;
                            else if (fabs(LeptonEta) < 2.1)
                                trg_sf_weight = 0.9480;
                            else
                                trg_sf_weight = 0.8757;
                        } else {
                            if (fabs(LeptonEta) < 0.8)
                                trg_sf_weight = 0.9666;
                            else if (fabs(LeptonEta) < 1.2)
                                trg_sf_weight = 0.9521;
                            else if (fabs(LeptonEta) < 2.1)
                                trg_sf_weight = 0.9485;
                            else
                                trg_sf_weight = 0.8772;
                        }
                    } else {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
                            if (fabs(LeptonEta) < 0.8)
                                trg_sf_weight = 0.9655;
                            else if (fabs(LeptonEta) < 1.2)
                                trg_sf_weight = 0.9535;
                            else if (fabs(LeptonEta) < 2.1)
                                trg_sf_weight = 0.9558;
                            else
                                trg_sf_weight = 0.9031;
                        } else {
                            if (fabs(LeptonEta) < 0.8)
                                trg_sf_weight = 0.9670;
                            else if (fabs(LeptonEta) < 1.2)
                                trg_sf_weight = 0.9537;
                            else if (fabs(LeptonEta) < 2.1)
                                trg_sf_weight = 0.9530;
                            else
                                trg_sf_weight = 0.8992;
                        }

                    }
                }//the trg_sf_weight is known

                if (GenEventType == -100 || //the geneventtype is not well-defined -> is background
                        GenEventType != eventType) // the backgrounds in ttbar events
                {
                    double pu_weight = this->PUWeights[nPU];
                    eventWeight *= (trg_sf_weight * pu_weight * pdf_weight);
		    

                    ret->Fill(recCosThetaValueHolder, eventWeight);
                }
            }//end of tree loop
        }//the last tree has been read here

        return ret;
    }

    TH1* GetCosThetaPlot(string Channel, string name, int nFinalBin = 10) {
        //cout << name << endl;
        if (files.count(name) == 0)
            files[name] = TFile::Open(("./WPol_SelectedTTBars_" + name + ".root").c_str(), "READ");

        //cout << "./WPol_SelectedTTBars_" + name + ".root" << endl;

        boost::algorithm::to_lower(Channel);
        TH1* h = (TH1*) files[name]->Get(("costheta_" + Channel + "/hCosThetaAllLepton").c_str());
        //cout << ("costheta_" + Channel + "/hCosThetaAllLepton") << endl;
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
            files[name] = TFile::Open(("./WPol_SelectedTTBars_" + name + ".root").c_str(), "READ");

        //cout << "./WPol_SelectedTTBars_" + name + ".root" << endl;

        boost::algorithm::to_lower(Channel);
        TH2* h = (TH2*) files[name]->Get(("costheta_" + Channel + "/hCosThetaAllGenVsREC").c_str());
        //cout << ("costheta_" + Channel + "/hCosThetaAllGenVsREC") << endl;
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
    std::map< std::pair< std::string, std::string >, TFile*> tree_files;
    std::map<string, double> Channels;
    std::map< int, double > PUWeights;
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

