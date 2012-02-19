/* 
 * File:   EfficiencyExt.h
 * Author: ajafari
 * Each instance of this class is to fill the bins of passed and all 
 * Histograms (which are to used for efficiency) by some integral on 
 * another property of ....
 * Created on January 30, 2009, 6:28 PM
 */

#ifndef _EFFICIENCYEXT_H
#define	_EFFICIENCYEXT_H

#include "EfficiencyH1.h"
#include "ObjectProperty.h"
#include "string"
#include "vector"
#include "TH1D.h"
#include <sstream>
#include <cmath>
#include "TF1.h"
#include "TStyle.h"
#include "TPaveText.h"

#include "Math/Functor.h"
#include "Math/WrappedFunction.h"
#include "Math/IFunction.h"
#include "Math/Integrator.h"

#ifndef uint
#define uint unsigned int
#endif

using namespace std;

class TF1Wrapper {
public:
    static TF1* F;
    static double Get(double x);
};

template<class T>
class EfficiencyExt : public EfficiencyH1<T> {
public:
    typedef EfficiencyH1<T> BASE;

    EfficiencyExt(ObjectProperty<T>* main, ObjectProperty<T>* inside, TF1* _fitFunc, double xmin, double xmax, string name) : BASE(main, name),
    InsideProp(inside),
    fitFunc(_fitFunc),
    xMin(xmin),
    xMax(xmax) {
        binSize = (this->theProperty->MaxValue - this->theProperty->MinValue) / this->theProperty->NumberOfBins;
        for (int i = 0; i < main->NumberOfBins; i++) {
            TH1D ip;

            ostringstream ss_name;
            ostringstream ss_title;
            ss_name << inside->PropertyName << "_" << i << "_P_main_" << main->PropertyName;
            ss_title << inside->PropertyTitle << " (" << i << "th bin of " << main->PropertyName << ") passed";
            ip.SetNameTitle(ss_name.str().c_str(), ss_title.str().c_str());
            ip.SetBins(inside->NumberOfBins, inside->MinValue, inside->MaxValue);
            innerHPassed.push_back(ip);

            ostringstream ss_name2;
            ostringstream ss_title2;
            ss_name2 << inside->PropertyName << "_" << i << "_A_main_" << main->PropertyName;
            ss_title2 << inside->PropertyTitle << " (" << i << "th bin of " << main->PropertyName << ") All";
            TH1D* ia = (TH1D*) (ip.Clone(ss_name2.str().c_str()));
            ia->SetTitle(ss_title2.str().c_str());
            innerHAll.push_back(*ia);
        }
    };

    virtual ~EfficiencyExt() {
    };

    virtual int Fill(const T* theObject, double weight = 1) {
        if (this->theProperty == 0) {
            cout << "NULL !!!";
            throw std::exception();
        } else {
            bool allPassed = true;
            if (this->AllCondition != 0)
                allPassed = this->AllCondition(theObject);

            if (allPassed) {
                double main_val = this->theProperty->ReadValue(theObject);

                if (main_val > this->theProperty->MinValue && main_val < this->theProperty->MaxValue) {
                    int bin_number(floor((main_val - this->theProperty->MinValue) / binSize));

                    double inside_val = this->InsideProp->ReadValue(theObject);
                    innerHAll.at(bin_number).Fill(inside_val, weight);

                    if (this->Condition(theObject))
                        return innerHPassed.at(bin_number).Fill(inside_val, weight);
                }
            }
        }
        return -10;

    };

    virtual void WriteAll(TDirectory* dir, bool mkdir = true) {

        TDirectory* my_dir = dir->mkdir(this->theProperty->PropertyName.c_str());
        TDirectory* in_dir = my_dir->mkdir(InsideProp->PropertyName.c_str());
        in_dir->cd();

#ifdef DEBUG__
        cout << this->Name << " is being started" << endl;
#endif


        int old_fit = gStyle->GetOptFit();
        gStyle->SetOptFit(1111);
        for (uint i = 0; i < innerHAll.size(); i++) {
            ostringstream ss;
            ss << "bin_" << i;
            in_dir->mkdir(ss.str().c_str())->cd();

            double bin_i_all = 0;
            TH1D hAllI = innerHAll[i];
            TF1* f_ret = GetIntegral(&hAllI, bin_i_all);

            TCanvas cA;
            cA.cd();
            hAllI.Draw();
            f_ret->Draw("same");

            TPaveText * t = new TPaveText(0.05 , 0.85 , 0.25 , 0.95 , "brNDC");
            ostringstream int_A;
            int_A << "Integral = " << bin_i_all;
            t->AddText(int_A.str().c_str());
            t->Draw();

            cA.Write(hAllI.GetName());

#ifdef DEBUG__
            cout << "\tAll " << i << " has been integrated" << endl;
#endif

            double bin_i_pss = 0;
            TH1D hPssI = innerHPassed[i];
            f_ret = GetIntegral(&hPssI, bin_i_pss);

            TCanvas cP;
            cP.cd();
            hPssI.Draw();
            f_ret->Draw("same");            

            t = new TPaveText(0.05 , 0.85 , 0.25 , 0.95 , "brNDC");
            ostringstream int_P;
            int_P << "Integral = " << bin_i_pss;
            t->AddText(int_P.str().c_str());
            t->Draw();

            cP.Write(hPssI.GetName());
#ifdef DEBUG__
            cout << "\tPass " << i << " has been integrated" << endl;
#endif

            this->hAll->SetBinContent(i + 1, bin_i_all);
            TH1D::SetBinContent(i + 1, bin_i_pss);
        }
        gStyle->SetOptFit(old_fit);

        BASE::WriteAll(my_dir, false);

        dir->cd();
    };

    TF1* GetIntegral(TH1D* h, double& res) {

        Double_t * all = h->GetIntegral(); //Integral();
        double par0 = fitFunc->GetParameter(0);
        par0 = (*all)*par0;

        fitFunc->SetParameter(0,par0);
        TF1* fAfterFit =  (TF1*) (fitFunc->Clone());
#ifdef DEBUG__
        cout << "\tFit Started ";
#endif
        TCanvas* ccc = new TCanvas();
        fAfterFit->Draw();
        ccc->Write("funcBefore");
        
        h->Fit(fAfterFit, "QRMELO");

        TCanvas* ccc2 = new TCanvas();
        fAfterFit->Draw();
        ccc2->Write("funcAfter");
        
#ifdef DEBUG__
        cout << "\tFit Finished ";
#endif

        TF1Wrapper::F = fAfterFit;
        ROOT::Math::WrappedFunction<> f1(TF1Wrapper::Get);

#ifdef DEBUG__
        cout << "\tIntegral Started ";
#endif
        ROOT::Math::Integrator ig(f1);

        res = ig.Integral(xMin, xMax);
        //res = fAfterFit->Integral(xMin, xMax);
        return fAfterFit;
    };
protected:
    vector<TH1D> innerHPassed;
    vector<TH1D> innerHAll;
    ObjectProperty<T>* InsideProp;
    TF1* fitFunc;
    double xMin, xMax;
    double binSize;
};

#endif	/* _EFFICIENCYEXT_H */

