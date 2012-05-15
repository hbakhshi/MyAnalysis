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
#include "TMath.h"
#include "TCanvas.h"
#include "TRandom1.h"
#include "TVirtualFitter.h"
#include <string>
#include <iostream>

using namespace std;

inline Double_t CosTheta(double *x, double *par)
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

class LLBase {
public:

    LLBase(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum)
    : Name(name), bkg(nonWtbSum), data(hData), signal(WtbSum) {
        data->Sumw2();
        bkg->Sumw2();
        signal->Sumw2();
        smCosTheta = new TF1(string("smCosTheta_" + Name).c_str(), CosTheta, -1., 1., 2);
        smCosTheta->SetParName(0, "f_{0}");
        smCosTheta->SetParName(1, "f_{neg}");
        smCosTheta->SetParameters(6.64263e-01, 3.03734e-01);
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

        if(x[0]+x[1] > 1.0)
            return 10000000.0;
        
        double LL = 0.0;
        int nbins = data->GetXaxis()->GetNbins();
        for (int i = 0; i < nbins; i++) {
            vector<double> numbers = this->getNdataNmc(i + 1, x[0], x[1], x[2]);
            LL += this->Probability(numbers);
        }
        //        cout<<"LL: "<<LL<<endl;
        return LL;
    }
protected:
    string Name;
    TH1* bkg;
    TH1* data;
    TH1* signal;
    TF1 * smCosTheta;

    virtual vector<double> getNdataNmc(int bin, double f0 = 6.64263e-01, double f_ = 3.03734e-01, double rec_gen = 1.) = 0;

    double getWeight(double costheta, double f0 = 6.64263e-01, double f_ = 3.03734e-01) {
        //        cout<<smCosTheta<< " in getWeightFunc"<<endl;
        this->smCosTheta->SetParameter("f_{0}", 6.64263e-01);
        this->smCosTheta->SetParameter("f_{neg}", 3.03734e-01);
        double SM = smCosTheta->Eval(costheta);
        this->smCosTheta->SetParameter("f_{0}", f0);
        this->smCosTheta->SetParameter("f_{neg}", f_);
        double nonSM = smCosTheta->Eval(costheta);
        return (double) nonSM / (double) SM;
    }

    virtual double Probability(vector<double>) = 0;
};

class LikelihoodFunction : public LLBase {
public:

    LikelihoodFunction(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum) : LLBase(name, nonWtbSum, hData, WtbSum) {

    };

    ~LikelihoodFunction() {
    }

    static std::pair<TF3, LikelihoodFunction*> getLLFunction(string name, TH1* nonWtbSum, TH1* hData, TH1* WtbSum) {
        LikelihoodFunction * functor = new LikelihoodFunction(name, nonWtbSum, hData, WtbSum);
        TF3 ret(name.c_str(), functor, 0.0, 1.0, 0.0, 0.1, 0.0, 2.0, 0, "LikelihoodFunction");
        ret.SetRange(0.0, 0.0, 0.000001, 1.0, 1.0, 2.0);
        return make_pair(ret, functor);
    }
protected:

    virtual vector<double> getNdataNmc(int bin, double f0 = 6.64263e-01, double f_ = 3.03734e-01, double rec_gen = 1.) {
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
        double weight = getWeight(costheta, f0, f_) * rec_gen;
        double nSignal = weight * signal->GetBinContent(bin);
        double nMC = bkg->GetBinContent(bin) + nSignal;
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

    virtual vector<double> getNdataNmc(int bin, double f0 = 6.64263e-01, double f_ = 3.03734e-01, double rec_gen = 1.) {
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

void GetMinimum(TF3 F, double * x, double * xerr, bool CalcError = true) {
    //    based on the documentation of TF3::GetMinimumXYZ from
    //    http://root.cern.ch/root/html532/src/TF3.cxx.html#QUjxjE
    //    F.Print("all");
    F.GetMinimumXYZ(x[0], x[1], x[2]);
    //    cout<<x[0]<<"\t"<<x[1]<<"\t"<<x[2]<<endl;
    if (!CalcError)
        return;
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
    minuit->SetParameter(0, "x", x[0], 0.1, xl, xu);
    minuit->SetParameter(1, "y", x[1], 0.1, yl, yu);
    minuit->SetParameter(2, "z", x[2], 0.1, zl, zu);
    for (int i = 0; i < 10; i++)
        arg_list[i] = 1.;
    Int_t fitResult = minuit->ExecuteCommand("MIGRAD", arg_list, 0);
    if (fitResult != 0) {
        cout << "Abnormal termination of minimization" << endl;
        x[0] = -1.0;
        x[1] = -1.0;
        x[2] = -1.0;
        delete minuit;
        return;
    }

    x[0] = minuit->GetParameter(0);
    x[1] = minuit->GetParameter(1);
    x[2] = minuit->GetParameter(2);

    double globcc = 0.0;
    minuit->GetErrors(0, xu, xl, xerr[0], globcc);
    minuit->GetErrors(1, yu, yl, xerr[1], globcc);
    minuit->GetErrors(2, zu, zl, xerr[2], globcc);
    delete minuit;
}

class WeightFunctionCreator {
public:

    WeightFunctionCreator(double std_f0 = 6.64263e-01, double std_fneg = 3.03734e-01) :
    f0Std(std_f0), f_Std(std_fneg) {
        func = new TF1("WeightFunctionCreator", CosTheta, -1, 1, 2);
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

    static std::pair<TF1, WeightFunctionCreator*> getWeightFunction(string name, double std_f0 = 6.64263e-01, double std_fneg = 3.03734e-01) {
        WeightFunctionCreator * functor = new WeightFunctionCreator(6.64263e-01, 3.03734e-01);
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
#endif	/* LikelihoodFunction_H */

