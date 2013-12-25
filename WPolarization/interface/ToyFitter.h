/* 
 * File:   LikelihoodFunction.h
 * Author: nadjieh
 *
 * Created on May 1, 2012, 7:06 PM
 */

#ifndef LikelihoodFunction_H
#define	LikelihoodFunction_H
#include "TF1.h"
#include "TF3.h"
#include "TH1.h"
#include "TH2.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TRandom1.h"
#include "TVirtualFitter.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "LHAPDF/LHAPDF.h"

using namespace std;

inline Double_t CosTheta__(double *x, double *par)
/*--------------------------------------------------------------------*/ {
    //par[0]: F01
    //par[1]: F-1
    //F+ = 1- F-i - F0i
    Double_t firstTerm1 = (1 - par[0] - par[1])*(1 + x[0])*(1 + x[0]);
    Double_t secondTerm1 = par[1]*(1 - x[0])*(1 - x[0]);
    Double_t thirdTerm1 = par[0]*(1 - x[0] * x[0]);
    Double_t First = (3.0 / 8.0)*(firstTerm1 + secondTerm1)+(3.0 / 4.0) * thirdTerm1;
    return First;
};

class WeightFunctionCreator {
public:

    WeightFunctionCreator(double std_f0, double std_fneg) :
    f0Std(std_f0), f_Std(std_fneg) {
        func = new TF1("WeightFunctionCreator", CosTheta__, -1, 1, 2);
        func->SetParameters(f0Std, f_Std);
    }

    ~WeightFunctionCreator() {
        delete func;
    }

    Double_t operator()(double * x, double * par) {
        func->SetParameters(f0Std, f_Std);
        //	cout<<"SM: "<< func->GetParameter(0)<<"  "<<func->GetParameter(1)<<endl;
        double stdVal = func->Eval(x[0]);
        func->SetParameters(par);
        //	cout<<func->GetParameter(0)<<"  "<<func->GetParameter(1)<<endl;
        double nonstdVal = func->Eval(x[0]);
        return ((double) nonstdVal / (double) stdVal);
    }

    static std::pair<TF1, WeightFunctionCreator*> getWeightFunction(string name, double std_f0 = 0.7, double std_fneg = 0.3) {
        WeightFunctionCreator * functor = new WeightFunctionCreator(std_f0, std_fneg);
        TF1 ret(name.c_str(), functor, -1.0, 1.0, 2);

        ret.SetParName(0, "F0");
        ret.SetParName(1, "FNeg");

        //        ret.SetParLimits(0 , 0.0 , 1.0);
        //        ret.SetParLimits(1 , 0.0 , 1.0);

        ret.SetParameters(std_f0, std_fneg);

        return make_pair(ret, functor);
    }
private:
    TF1 * func;
    double f0Std;
    double f_Std;


};

class LLBase {
public:

    LLBase(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum, double std_f0 = 0.7, double std_fneg = 0.3)
    : Name(name), bkg(nonWtbSum), data(hData), signal(WtbSum),
    weightor(WeightFunctionCreator::getWeightFunction("wieghtor" + name, std_f0, std_fneg)) {
        data->Sumw2();
        bkg->Sumw2();
        signal->Sumw2();
        this->Tree = false;
    }

    LLBase(string name, TH1* nonWtbSum, TH1* hData, TH2* WtbSum, string sss_tmp, double std_f0 = 0.7, double std_fneg = 0.3)
    : Name(name), bkg(nonWtbSum), data(hData), signal2D(WtbSum),
    weightor(WeightFunctionCreator::getWeightFunction("wieghtor" + name, std_f0, std_fneg)) {
        data->Sumw2();
        bkg->Sumw2();
        signal2D->Sumw2();
        signal = NULL;
        this->Tree = false;
    }

    virtual ~LLBase() {
        //        delete bkg;
        //        delete signal;
        //        delete data;
        //        delete smCosTheta;
    }

    Double_t operator()(double * x, double * par = 0) {
        //        x[0] = f_0
        //        x[1] = f_Neg
        //        x[2] = rec_gen factor
        //        no parameter is needed

        //        if (x[0] > 0 && x[1] < 0) {
        //            double newx[] = {x[0], 0.0, x[2]};
        //            return (*this)(newx);
        //        } else if (x[0] < 0 && x[1] > 0) {
        //            double newx[] = {0.0, x[1], x[2]};
        //            return this->operator()(newx);
        //        } else if (x[0] < 0 && x[1] < 0) {
        //            double newx[] = {0.0, 0.0, x[2]};
        //            return this->operator ()(newx);
        //        }else if( x[0] + x[1] > 1.0 ){
        //            double newx[] = { x[0]/(x[0] + x[1]) , x[1]/(x[0]+x[1]) , x[2] };
        //            return this->operator ()(newx);
        //        }

        double LL = 0.0;
        int nbins = data->GetXaxis()->GetNbins();
        for (int i = 0; i < nbins; i++) {
            vector<double> numbers = this->getNdataNmc(i + 1, x[0], x[1], x[2]);
            LL += this->Probability(numbers);
        }
        //        cout<<"LL: "<<LL<<endl;
        return LL;
    }

    double SumOfErrors(double sys, double stat) {
        return sqrt(sys * sys + stat * stat);
    }

    TH2* hLepSF(bool electron, string appendix = "") {
        TH2* hret = new TH2D();

        string name___;
        if (electron)
            name___ = "hElecSF_" + appendix;
        else
            name___ = "hMuonSF_" + appendix;

        hret->SetNameTitle(name___.c_str(), name___.c_str());

        double pt_bins [] = {20, 22, 24, 26, 28, 30, 32, 34, 36, 40, 1000};
        double eta_bins[] = {0, 1.5, 10};
        hret->SetBins(10, pt_bins, 2, eta_bins);

        if (electron) {
            hret->SetBinContent(1, 1, 0.990);
            hret->SetBinError(1, 1, SumOfErrors(0.004, 0.001));

            hret->SetBinContent(2, 1, 0.999);
            hret->SetBinError(2, 1, SumOfErrors(0.003, 0.001));

            hret->SetBinContent(3, 1, 1.003);
            hret->SetBinError(3, 1, SumOfErrors(0.003, 0.002));

            hret->SetBinContent(4, 1, 0.997);
            hret->SetBinError(4, 1, SumOfErrors(0.002, 0.002));

            hret->SetBinContent(5, 1, 1.001);
            hret->SetBinError(5, 1, SumOfErrors(0.002, 0.000));

            hret->SetBinContent(6, 1, 0.999);
            hret->SetBinError(6, 1, SumOfErrors(0.001, 0.002));

            hret->SetBinContent(7, 1, 1.001);
            hret->SetBinError(7, 1, SumOfErrors(0.001, 0.000));

            hret->SetBinContent(8, 1, 0.998);
            hret->SetBinError(8, 1, SumOfErrors(0.001, 0.000));

            hret->SetBinContent(9, 1, 0.996);
            hret->SetBinError(9, 1, SumOfErrors(0.001, 0.000));

            hret->SetBinContent(10, 1, 0.993);
            hret->SetBinError(10, 1, SumOfErrors(0.000, 0.000));

            hret->SetBinContent(1, 2, 1.002);
            hret->SetBinError(1, 2, SumOfErrors(0.007, 0.005));

            hret->SetBinContent(2, 2, 1.018);
            hret->SetBinError(2, 2, SumOfErrors(0.006, 0.005));

            hret->SetBinContent(3, 2, 1.012);
            hret->SetBinError(3, 2, SumOfErrors(0.005, 0.006));

            hret->SetBinContent(4, 2, 1.006);
            hret->SetBinError(4, 2, SumOfErrors(0.002, 0.004));

            hret->SetBinContent(5, 2, 1.013);
            hret->SetBinError(5, 2, SumOfErrors(0.001, 0.004));

            hret->SetBinContent(6, 2, 1.003);
            hret->SetBinError(6, 2, SumOfErrors(0.002, 0.003));

            hret->SetBinContent(7, 2, 1.005);
            hret->SetBinError(7, 2, SumOfErrors(0.003, 0.002));

            hret->SetBinContent(8, 2, 1.005);
            hret->SetBinError(8, 2, SumOfErrors(0.003, 0.002));

            hret->SetBinContent(9, 2, 1.000);
            hret->SetBinError(9, 2, SumOfErrors(0.002, 0.001));

            hret->SetBinContent(10, 2, 0.997);
            hret->SetBinError(10, 2, SumOfErrors(0.001, 0.000));
        } else { //for muon
            hret->SetBinContent(1, 1, 0.993);
            hret->SetBinError(1, 1, SumOfErrors(0.002, 0.001));

            hret->SetBinContent(2, 1, 0.992);
            hret->SetBinError(2, 1, SumOfErrors(0.002, 0.001));

            hret->SetBinContent(3, 1, 0.996);
            hret->SetBinError(3, 1, SumOfErrors(0.002, 0.001));

            hret->SetBinContent(4, 1, 0.997);
            hret->SetBinError(4, 1, SumOfErrors(0.001, 0.001));

            hret->SetBinContent(5, 1, 0.997);
            hret->SetBinError(5, 1, SumOfErrors(0.001, 0.001));

            hret->SetBinContent(6, 1, 1.000);
            hret->SetBinError(6, 1, SumOfErrors(0.001, 0.000));

            hret->SetBinContent(7, 1, 0.999);
            hret->SetBinError(7, 1, SumOfErrors(0.001, 0.001));

            hret->SetBinContent(8, 1, 0.999);
            hret->SetBinError(8, 1, SumOfErrors(0.001, 0.000));

            hret->SetBinContent(9, 1, 0.999);
            hret->SetBinError(9, 1, SumOfErrors(0.001, 0.000));

            hret->SetBinContent(10, 1, 0.998);
            hret->SetBinError(10, 1, SumOfErrors(0.000, 0.000));

            hret->SetBinContent(1, 2, 1.001);
            hret->SetBinError(1, 2, SumOfErrors(0.003, 0.003));

            hret->SetBinContent(2, 2, 0.999);
            hret->SetBinError(2, 2, SumOfErrors(0.003, 0.004));

            hret->SetBinContent(3, 2, 1.004);
            hret->SetBinError(3, 2, SumOfErrors(0.002, 0.004));

            hret->SetBinContent(4, 2, 1.001);
            hret->SetBinError(4, 2, SumOfErrors(0.002, 0.005));

            hret->SetBinContent(5, 2, 1.001);
            hret->SetBinError(5, 2, SumOfErrors(0.002, 0.004));

            hret->SetBinContent(6, 2, 1.004);
            hret->SetBinError(6, 2, SumOfErrors(0.002, 0.004));

            hret->SetBinContent(7, 2, 1.005);
            hret->SetBinError(7, 2, SumOfErrors(0.003, 0.002));

            hret->SetBinContent(8, 2, 1.002);
            hret->SetBinError(8, 2, SumOfErrors(0.001, 0.002));

            hret->SetBinContent(9, 2, 1.002);
            hret->SetBinError(9, 2, SumOfErrors(0.001, 0.001));

            hret->SetBinContent(10, 2, 1.000);
            hret->SetBinError(10, 2, SumOfErrors(0.000, 0.000));
        }

        return hret;
    }

  void SetTreeFile(string fileName, double wee, double wem, double wmm, std::map< int, double >* PUWeights = NULL, bool applyLepSF = false , int PdfSetID = 0 , double spinCorr = 0.0 , int trigger_syst = 0) {
        this->Tree = true;
        this->useTree = true;
        this->nsignal2DFromTreeBins = 10000;
        int nbins = data->GetXaxis()->GetNbins();

        //TFile* TreeFile = TFile::Open(fileName.c_str(), "UPDATE");
        gROOT->cd();
        signal2DFromTree = new TH2D("hsignal2DFromTree", "signal2DFromTree", nbins, -1.0, 1.0, 10000, -1.0, 1.0);
        TH1D* heventtype = new TH1D("heventtype", "event type", 15, 0.0, 15);

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

	double GenTopCosTheta, GenAntiTopCosTheta ;

	float  PDFScalePDF;
	float  PDFx1;
	float  PDFx2;
	float  PDFxPDF1;
	float  PDFxPDF2;
	int PDFID1;
	int PDFID2;

        TH2* hElecSF = hLepSF(true);
        TH2* hMuonSF = hLepSF(false);
        TH2* hLEPSF_temp = hLepSF(false, "temp");

	if ( PdfSetID != 0 ){
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

	}

        for (int nBin = 1; nBin < nbins + 1; nBin++) {
            double binCenter = data->GetBinCenter(nBin);
            ostringstream treeName;

            treeName << "Tree_" << nBin;
            TChain* tree = new TChain(treeName.str().c_str());
            //(TTree*) TreeFile->Get(treeName.str().c_str());

            tree->Add(fileName.c_str());

	    if( tree->FindBranch("GenTopCosTheta") ){
	      tree->SetBranchAddress("GenTopCosTheta", &GenTopCosTheta);
	      tree->SetBranchAddress("GenAntiTopCosTheta", &GenAntiTopCosTheta);
	    }else{
	      GenTopCosTheta = GenAntiTopCosTheta = 0.0 ;
	    }

	    tree->SetBranchAddress("PDFScalePDF", &PDFScalePDF);
	    tree->SetBranchAddress("PDFx1", &PDFx1);
	    tree->SetBranchAddress("PDFx2", &PDFx2);
	    //tree->SetBranchAddress("PDFxPDF1", &PDFxPDF1);
	    //tree->SetBranchAddress("PDFxPDF2", &PDFxPDF1);
	    tree->SetBranchAddress("PDFID1", &PDFID1);
	    tree->SetBranchAddress("PDFID2", &PDFID2);


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


            for (int entry = 0; entry < tree->GetEntries(); entry++) {

                tree->GetEntry(entry);
                heventtype->Fill(eventType);

                double evtweight = eventWeight; //* lumiWeight;


                //the trg SF needs to be added
                double trg_sf_weight = 0.0;
		double trg_sf_weight_err = 0.0;
                if (isElectron) {
                    if (isFirstLepton) {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
			  if (fabs(LeptonEta) < 1.5){
                                trg_sf_weight = 0.9849;			    
				trg_sf_weight_err = 3.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.9774;
				trg_sf_weight_err = 7.0e-4;
			  }
                        } else {
			  if (fabs(LeptonEta) < 1.5){
                                trg_sf_weight = 0.9928;
				trg_sf_weight_err = 1.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.9938;
				trg_sf_weight_err = 1.0e-4;
			  }
                        }
                    } else {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
			  if (fabs(LeptonEta) < 1.5){
                                trg_sf_weight = 0.9923;
				trg_sf_weight_err = 2.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.9953;
				trg_sf_weight_err = 3.0e-4;
			  }
                        } else {
			  if (fabs(LeptonEta) < 1.5){
                                trg_sf_weight = 0.9948;
				trg_sf_weight_err = 1.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.9956;
				trg_sf_weight_err = 1.0e-4;
			  }
                        }
                    }
                } else {
                    if (isFirstLepton) {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
			  if (fabs(LeptonEta) < 0.8){
                                trg_sf_weight = 0.9648;
				trg_sf_weight_err = 7.0e-4;
			  }
			  else if (fabs(LeptonEta) < 1.2){
                                trg_sf_weight = 0.9516;
				trg_sf_weight_err = 13.0e-4;
			  }
			  else if (fabs(LeptonEta) < 2.1){
                                trg_sf_weight = 0.9480;
				trg_sf_weight_err = 9.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.8757;
				trg_sf_weight_err = 26.0e-4;
			  }
                        } else {
			  if (fabs(LeptonEta) < 0.8){
                                trg_sf_weight = 0.9666;
				trg_sf_weight_err = 3.0e-4;
			  }
			  else if (fabs(LeptonEta) < 1.2){
                                trg_sf_weight = 0.9521;
				trg_sf_weight_err = 5.0e-4;
			  }
			  else if (fabs(LeptonEta) < 2.1){
                                trg_sf_weight = 0.9485;
				trg_sf_weight_err = 4.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.8772;
				trg_sf_weight_err = 12.0e-4;
			  }
                        }
                    } else {
                        if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
			  if (fabs(LeptonEta) < 0.8){
                                trg_sf_weight = 0.9655;
				trg_sf_weight_err = 7.0e-4;
			  }
			  else if (fabs(LeptonEta) < 1.2){
                                trg_sf_weight = 0.9535;
				trg_sf_weight_err = 13.0e-4;
			  }
			  else if (fabs(LeptonEta) < 2.1){
                                trg_sf_weight = 0.9558;
				trg_sf_weight_err = 9.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.9031;
				trg_sf_weight_err = 23.0e-4;
			  }
                        } else {
			  if (fabs(LeptonEta) < 0.8){
                                trg_sf_weight = 0.9670;
				trg_sf_weight_err = 3.0e-4;
			  }
			  else if (fabs(LeptonEta) < 1.2){
                                trg_sf_weight = 0.9537;
				trg_sf_weight_err = 5.0e-4;
			  }
			  else if (fabs(LeptonEta) < 2.1){
                                trg_sf_weight = 0.9530;
				trg_sf_weight_err = 4.0e-4;
			  }
			  else{
                                trg_sf_weight = 0.8992;
				trg_sf_weight_err = 11.0e-4;
			  }
                        }

                    }
                }//the trg_sf_weight is known

		if(trigger_syst != 0)
		  trigger_syst /= abs(trigger_syst) ;

		trg_sf_weight += (trigger_syst*trg_sf_weight_err);
		
                evtweight *= trg_sf_weight;

                if (applyLepSF) {
                    int bin_eta_pt = hLEPSF_temp->FindBin(LeptonPt, fabs(LeptonEta));
                    if (isElectron)
                        evtweight *= hElecSF->GetBinContent(bin_eta_pt);
                    else
                        evtweight *= hMuonSF->GetBinContent(bin_eta_pt);
                }
                
                if (eventType == 3) { //dile
                    //                    evtweight /= 0.994;
                    evtweight *= wee; //0.192709;
                } else if (eventType == 1 || eventType == 2) {//emu
                    //                    evtweight /= 0.995;
                    evtweight *= wem; //0.197058;
                } else if (eventType == 4) {//emu
                    //                    evtweight /= 0.997;
                    evtweight *= wmm; //0.189709;
                }

                if (PUWeights != NULL) {
                    evtweight *= PUWeights->at(nPU);
                }

		if( spinCorr != 0.0){
		  double spin_corr_w = ( 1.0 - spinCorr*GenTopCosTheta*GenAntiTopCosTheta );
		  evtweight *= spin_corr_w ;
		}

		if ( PdfSetID != 0 ){
		  LHAPDF::usePDFMember(0);
		  PDFxPDF1 = LHAPDF::xfx(PDFx1 , PDFScalePDF , PDFID1 ) ; 
		  PDFxPDF2 = LHAPDF::xfx(PDFx2 , PDFScalePDF , PDFID2 ) ; 
		
		  LHAPDF::usePDFMember(PdfSetID);
		  PDFxPDF1 = LHAPDF::xfx(PDFx1 , PDFScalePDF , PDFID1 ) / PDFxPDF1;
		  PDFxPDF2 = LHAPDF::xfx(PDFx2 , PDFScalePDF , PDFID2 ) / PDFxPDF2;
		  double pdf_weight = PDFxPDF2*PDFxPDF1 ; 

		  evtweight *= pdf_weight ;
		}
		
                if (eventType == GenEventType && evtweight != 0.0) {

                    TreeInfo[nBin].push_back(make_pair(genCosThetaValueHolder, evtweight));


                    signal2DFromTree->Fill(binCenter, genCosThetaValueHolder, evtweight);
                }
            }
            // cout << "Tree_" << (nBin) << "is read" << endl;
        }

        //signal2DFromTree->Write();
        //        gROOT->cd();
        //        signal2DFromTree = (TH2*) signal2DFromTree->Clone("Cloned");

        //        TreeFile->cd();
        //        heventtype->Write();
        //        TreeFile->Close();
    }
protected:
    string Name;
    TH1* bkg;
    TH1* data;
    TH1* signal;
    TH2* signal2D;
    std::pair<TF1, WeightFunctionCreator*> weightor;

    bool Tree;
    map< int, vector< pair<double, double> > > TreeInfo;
    TH2* signal2DFromTree;
    TH2* signal2DFromTreeRebinned;
    int nsignal2DFromTreeBins;
public:
    bool useTree;

    TH2* setnsignal2DFromTreeBins(int n) {
        nsignal2DFromTreeBins = n;
        stringstream newName;
        newName << signal2DFromTree->GetName() << n;
        gROOT->cd();
        return (this->signal2DFromTreeRebinned = signal2DFromTree->RebinY(signal2DFromTree->GetNbinsY() / n, newName.str().c_str()));
    }
    double wttee, wttmm, wttem;
protected:
    virtual vector<double> getNdataNmc(int bin, double f0, double f_, double rec_gen) = 0;

    double getWeight(double costheta, double f0, double f_) {
        weightor.first.SetParameters(f0, f_);
        return weightor.first.Eval(costheta);
    }

    virtual double Probability(vector<double>) = 0;
};

class LikelihoodFunction : public LLBase {
public:

    LikelihoodFunction(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum) : LLBase(name, nonWtbSum, hData, WtbSum) {

    };

    LikelihoodFunction(string name, TH1* nonWtbSum, TH1* hData, TH2* WtbSum, string sss_tmp) : LLBase(name, nonWtbSum, hData, WtbSum, sss_tmp) {

    };

    ~LikelihoodFunction() {
    }

    static std::pair<TF3, LikelihoodFunction*> getLLFunction(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum, bool castTOH2) {
        LikelihoodFunction * functor;
        if (!castTOH2)
            functor = new LikelihoodFunction(name, nonWtbSum, hData, WtbSum);
        else
            functor = new LikelihoodFunction(name, nonWtbSum, hData, (TH2*) WtbSum, "");
        TF3 ret(name.c_str(), functor, 0.0, 1.0, 0.0, 0.1, 0.0, 2.0, 0, "LikelihoodFunction");
        ret.SetRange(0.0, 0.0, 0.000001, 1.0, 1.0, 2.0);
        return make_pair(ret, functor);
    }


    virtual vector<double> getNdataNmc(int bin, double f0, double f_, double rec_gen) {
        int nbins = data->GetXaxis()->GetNbins();
        if (bin > nbins || nbins < 0) {
            cout << "No value for this cos(theta) bin" << endl;
            vector<double> ret;
            ret.push_back(-100);
            ret.push_back(-100);
            return ret;
        }
        double nData = data->GetBinContent(bin);
        double costheta = data->GetBinCenter(bin);

        double nSignal = 0.0;

        if (this->Tree) {
            if (this->useTree) {
                for (vector < pair<double, double > > ::const_iterator itr = this->TreeInfo[bin].begin();
                        itr != this->TreeInfo[bin].end(); itr++) {

                    double evtweight = itr->second;
                    double WeightVal = getWeight(itr->first, f0, f_);

                    nSignal += (evtweight * WeightVal);
                }
            } else {
                weightor.first.SetParameters(f0, f_);
                gROOT->cd();

                TH1* hithrecbin = this->signal2DFromTreeRebinned->ProjectionY("_py", bin, bin, "o");

                hithrecbin->Multiply(&(weightor.first), 1.0);
                nSignal = hithrecbin->Integral();
            }
        } else if (this->signal != NULL) {
            double weight = getWeight(costheta, f0, f_);
            nSignal = weight * signal->GetBinContent(bin);
        } else {
            weightor.first.SetParameters(f0, f_);
            gROOT->cd();
            TH1* hithrecbin = this->signal2D->ProjectionY("_py", bin, bin, "o");
            hithrecbin->Multiply(&(weightor.first), 1.0);
            nSignal = hithrecbin->Integral();
        }
        double nMC = bkg->GetBinContent(bin) + (nSignal * rec_gen);
        //        cout<<"****** "<<nData<<"\t"<<nMC<<endl;
        vector<double> ret;
        ret.push_back(nData);
        ret.push_back(nMC);
        ret.push_back(nSignal);
        return ret;
    }

    virtual double Probability(vector<double> numbers) {
        double x_mean = numbers[1];
        double x = numbers[0];
        if (x_mean <= 0.0)
            return 0.0;
        if (x <= 0.0)
            return 0.0;
        double log_pow_1 = x * log(x_mean);
        double log_exp_1 = -x_mean;
        double log_factoral = x * log(x) - x + (log(x * (1 + 4 * x * (1 + 2 * x))) / 6) + log(M_PI) / 2;

        return log_factoral - log_exp_1 - log_pow_1;
    }

};

class ChiSquaredFunction : public LLBase {
public:

    ChiSquaredFunction(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum) : LLBase(name, nonWtbSum, hData, WtbSum) {
        smMC = (TH1*) signal->Clone("smMC");
        smMC->Sumw2();
        smMC->Add(bkg);
    }

    virtual ~ChiSquaredFunction() {
    }

    static std::pair<TF3, ChiSquaredFunction *> getChiSquaredFunction(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum) {
        ChiSquaredFunction * functor = new ChiSquaredFunction(name, nonWtbSum, hData, WtbSum);
        TF3 ret(name.c_str(), functor, 0.0, 1.0, 0.0, 0.1, 0.0, 2.0, 0, "ChiSquaredFunction");
        //        TF3 ret(name.c_str(), "(x*x)+(y*y)+(z*z)", 0.0 , 1.0 , 0.0 , 0.1 , 0.0 , 2.0 );
        ret.SetRange(0.0, 0.0, 0.000001, 1.0, 1.0, 2.0);
        return make_pair(ret, functor);
    }
protected:
    TH1* smMC;

    virtual vector<double> getNdataNmc(int bin, double f0, double f_, double rec_gen) {
        int nbins = data->GetXaxis()->GetNbins();
        if (bin > nbins || nbins < 0) {
            cout << "No value for this cos(theta) bin" << endl;
            vector<double> ret;
            ret.push_back(-100);
            ret.push_back(-100);
            ret.push_back(-100);
            return ret;
        }
        double nData = data->GetBinContent(bin);
        double costheta = data->GetBinCenter(bin);
        double weight = getWeight(costheta, f0, f_) * rec_gen;
        double nSignal = weight * signal->GetBinContent(bin);
        double sigErr = weight * signal->GetBinError(bin);
        double nMC = bkg->GetBinContent(bin) + nSignal;
        double errMC = sqrt((sigErr * sigErr) + (bkg->GetBinError(bin) * bkg->GetBinError(bin)));

        vector<double> ret;
        ret.push_back(nData);
        ret.push_back(nMC);
        ret.push_back(errMC);
        return ret;
    }

    virtual double Probability(vector<double> numbers) {

        double x_mean = numbers[1];
        double x = numbers[0];
        if (x + x_mean < 0)
            return 0.0;
        double sigma = sqrt((numbers[2] * numbers[2]) + (numbers[0]));

        return (x - x_mean)*(x - x_mean) / (2 * sigma * sigma);
    }
};

int GetMinimum(TF3 F, double * x, double * xerr, double & corr12, bool CalcError = true) {
    //    based on the documentation of TF3::GetMinimumXYZ from
    //    http://root.cern.ch/root/html532/src/TF3.cxx.html#QUjxjE
    //    F.Print("all");
    //    cout<<x[0]<<"\t"<<x[1]<<"\t"<<x[2]<<endl;
    if (!CalcError) {
        F.GetMinimumXYZ(x[0], x[1], x[2]);

        return 0;
    }
    //    go to minuit for the final minimization

    TVirtualFitter * minuit = TVirtualFitter::Fitter(&F, 3);
    minuit->Clear();
    minuit->SetFitMethod("F3Minimizer");
    double arg_list[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    int nNargs = 1;
    minuit->ExecuteCommand("SET PRINT", arg_list, nNargs);
    double xl = 0.0;
    double xu = 0.0;
    double yl = 0.0;
    double yu = 0.0;
    double zl = 0.0;
    double zu = 0.0;
    minuit->SetParameter(0, "x", 0.7, 0.1, xl, xu);
    minuit->SetParameter(1, "y", 0.3, 0.1, yl, yu);
    minuit->SetParameter(2, "z", 1.0, 0.1, zl, zu);
    for (int i = 0; i < 10; i++)
        arg_list[i] = 1.;
    Int_t fitResult = minuit->ExecuteCommand("MIGRAD", arg_list, 0);
    if (fitResult != 0) {
        cout << "Abnormal termination of minimization" << endl;
        x[0] = -1.0;
        x[1] = -1.0;
        x[2] = -1.0;
        delete minuit;
        return fitResult;
    }

    x[0] = minuit->GetParameter(0);
    x[1] = minuit->GetParameter(1);
    x[2] = minuit->GetParameter(2);

    double globcc = 0.0;
    minuit->GetErrors(0, xu, xl, xerr[0], globcc);
    minuit->GetErrors(1, yu, yl, xerr[1], globcc);
    minuit->GetErrors(2, zu, zl, xerr[2], globcc);
    corr12 = minuit->GetCovarianceMatrixElement(0, 1);
    delete minuit;
    return fitResult;
}

#endif	/* LikelihoodFunction_H */

