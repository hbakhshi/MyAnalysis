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
#include "TMath.h"
#include "TCanvas.h"
#include "TRandom1.h"
#include "TVirtualFitter.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>

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

    void SetTreeFile(string fileName , double wee , double wem , double wmm) {
        this->Tree = true;
        this->useTree = true;
        this->nsignal2DFromTreeBins = 10000;
        int nbins = data->GetXaxis()->GetNbins();

        TFile* TreeFile = TFile::Open(fileName.c_str(), "UPDATE");
        signal2DFromTree = new TH2D("hsignal2DFromTree", "signal2DFromTree", nbins, -1.0, 1.0, 10000, -1.0, 1.0);
        TH1D* heventtype = new TH1D("heventtype" , "event type" , 15 , 0.0 , 15 );
        for (int nBin = 1; nBin < nbins + 1; nBin++) {
            double binCenter = data->GetBinCenter(nBin);
            ostringstream treeName;

            treeName << "Tree_" << nBin;
            TTree* tree = (TTree*) TreeFile->Get(treeName.str().c_str());

            double genCosTheta = 0.0;
            double eventWeight = 0.0;
            double lumiWeight = 0.0;
            int eventType = 0.0;
            
            tree->SetBranchAddress("GenCosTheta", &genCosTheta);
            tree->SetBranchAddress("EventWeight", &eventWeight);
            tree->SetBranchAddress("LumiWeight", &lumiWeight);
            tree->SetBranchAddress("eventType", &eventType);


            for (int entry = 0; entry < tree->GetEntries(); entry++) {

                tree->GetEntry(entry);
                heventtype->Fill(eventType);
                
                double evtweight = eventWeight ; //* lumiWeight;
                
                if(eventType == 3) { //dile
                    evtweight/=0.994;
                    evtweight*=wee; //0.192709;
                }else if(eventType == 1 || eventType == 2){//emu
                    evtweight/=0.995;
                    evtweight*=wem; //0.197058;
                }else if(eventType == 4){//emu
                    evtweight/=0.997;
                    evtweight*=wmm; //0.189709;
                }

                TreeInfo[nBin].push_back(make_pair(genCosTheta, evtweight));


                signal2DFromTree->Fill(binCenter, genCosTheta, evtweight);
            }
            // cout << "Tree_" << (nBin) << "is read" << endl;
        }

        //signal2DFromTree->Write();
        gROOT->cd();
        signal2DFromTree = (TH2*) signal2DFromTree->Clone("Cloned");
        
        TreeFile->cd();
        heventtype->Write();
        TreeFile->Close();
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
    TH2* setnsignal2DFromTreeBins(int n){
        nsignal2DFromTreeBins = n;
        stringstream newName ;
        newName << signal2DFromTree->GetName() << n ;
        gROOT->cd();
        return (this->signal2DFromTreeRebinned = signal2DFromTree->RebinY( signal2DFromTree->GetNbinsY()/n , newName.str().c_str() ) );
    }
    double wttee,wttmm,wttem;
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
protected:

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

