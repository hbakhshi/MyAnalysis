/* 
 * File:   EfficiencyH1.h
 * Author: ajafari
 *
 * Created on January 30, 2009, 3:40 PM
 */

#ifndef _EFFICIENCY_H
#define	_EFFICIENCY_H

#include "ObjectProperty.h"
#include "TH1Ext.h"
#include <string>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include "TDirectory.h"

template<class T>
class EfficiencyH1 : public TH1Ext<T> {
public:

    EfficiencyH1(ObjectProperty<T>* theProp, std::string name) : TH1Ext<T>(theProp),
    Name(name) {
        hAll = theProp->GetH1(name + "_ALL");

        std::string title = name;
        if (name != "")
            name += ("_PASSED_" + theProp->ObjectName + "_" + theProp->PropertyName);
        else
            name = theProp->ObjectName + "_" + theProp->PropertyName;

        TH1Ext<T>::SetNameTitle(name.c_str(), title.c_str());

        int nBins = hAll->GetNbinsX();
        double bins[1000];
        for (int kkk = 0; kkk < nBins; kkk++)
            bins[kkk] = hAll->GetBinLowEdge(kkk + 1);
        bins[nBins] = bins[nBins - 1] + hAll->GetBinWidth(nBins + 1);
        TH1Ext<T>::SetBins(nBins, bins);
        //        TH1Ext<T>::SetBins(theProp->NumberOfBins, theProp->MinValue, theProp->MaxValue);
        TH1Ext<T>::SetXTitle(theProp->PropertyTitle.c_str());

        AllCondition = 0;
    };

    virtual ~EfficiencyH1() {
    };

    bool (*Condition)(const T*);
    bool (*AllCondition)(const T*);

    virtual int Fill(const T* theObject, double weight = 1) {
        if (this->theProperty == 0) {
            cout << "NULL !!!";
            throw std::exception();
        } else {
            bool allPassed = true;
            if (AllCondition != 0)
                allPassed = AllCondition(theObject);

            if (allPassed) {
                double val = this->theProperty->ReadValue(theObject);
                hAll->Fill(val, weight);
                if (Condition(theObject))
                    return TH1D::Fill(val, weight);
            }
        }
        return -10;
    };

    virtual void WriteAll(TDirectory* dir, bool mkdir = true) {
        if (dir == 0)
            return;
        TDirectory* dir2;
        if (mkdir) {
            dir2 = dir->mkdir(this->theProperty->PropertyName.c_str());
            dir2->cd();
        } else {
            dir->cd();
        }


        hAll->Write();
        TH1Ext<T>::Write();

        eff_bay = new TGraphAsymmErrors((TH1*)this, hAll);
        eff_bay->GetXaxis()->SetTitle(hAll->GetXaxis()->GetTitle());
        string asym_name("EffBayesianAsymmErr");
        asym_name = Name + "_" + asym_name + "_" + this->theProperty->PropertyName;
        eff_bay->SetName(asym_name.c_str());
        eff_bay->Write();

        TCanvas c;
        c.cd();
        eff_bay->Draw("AP");
        c.Write("Drown_Bay");

        EffPoissonErr();
        EffPoisson->Write();
        (dir2->mkdir("Integrated"))->cd();

        IntegratedAll = new TH1D("IntegratedAll_0I", "IntegratedAll_0I", this->theProperty->NumberOfBins, this->theProperty->MinValue, this->theProperty->MaxValue);
        IntegratedPass = new TH1D("IntegratedPass_0I", "IntegratedPass_0I", this->theProperty->NumberOfBins, this->theProperty->MinValue, this->theProperty->MaxValue);
        for (int i = 0; i<this->theProperty->NumberOfBins; i++) {
            double passed = this->Integral(1, i + 1);
            double all = hAll->Integral(1, i + 1);
            IntegratedAll->SetBinContent(i + 1, all);
            IntegratedPass->SetBinContent(i + 1, passed);
        }
        IntegratedEff_bay = new TGraphAsymmErrors(IntegratedPass, IntegratedAll);
        IntegratedAll->Write();
        IntegratedPass->Write();
        std::string IEname("Integrated_");
        IEname += this->theProperty->PropertyName + "_0I";
        IntegratedEff_bay->SetName(IEname.c_str());
        IntegratedEff_bay->Write();

        IntegratedAll = new TH1D("IntegratedAll_Iinf", "IntegratedAll_Iinf", this->theProperty->NumberOfBins, this->theProperty->MinValue, this->theProperty->MaxValue);
        IntegratedPass = new TH1D("IntegratedPass_Iinf", "IntegratedPass_Iinf", this->theProperty->NumberOfBins, this->theProperty->MinValue, this->theProperty->MaxValue);
        for (int i = 0; i<this->theProperty->NumberOfBins; i++) {
            double passed = this->Integral(i + 1, this->theProperty->MaxValue);
            double all = hAll->Integral(i + 1, this->theProperty->MaxValue);
            IntegratedAll->SetBinContent(i + 1, all);
            IntegratedPass->SetBinContent(i + 1, passed);
        }
        IntegratedEff_bay = new TGraphAsymmErrors(IntegratedPass, IntegratedAll);
        IntegratedAll->Write();
        IntegratedPass->Write();
        std::string IEname_inf("Integrated_");
        IEname_inf += this->theProperty->PropertyName + "_Iinf";
        IntegratedEff_bay->SetName(IEname_inf.c_str());
        IntegratedEff_bay->Write();

        //        std::cout<<"\t"<<this->theProperty->PropertyName<<" is beung written"<<std::endl;
        dir->cd();
    };

    virtual void EffPoissonErr() {
        TH1D * tmp = new TH1D();
        string poison_name("EffPoissonErr");
        poison_name = Name + "_" + poison_name + "_" + this->theProperty->PropertyName;

        tmp->SetNameTitle(poison_name.c_str(), "");

        int nBins = this->GetNbinsX();
        double bins[1000];
        for (int kkk = 0; kkk < nBins; kkk++)
            bins[kkk] = this->GetBinLowEdge(kkk + 1);
        bins[nBins] = bins[nBins - 1] + this->GetBinWidth(nBins + 1);
        tmp->SetBins(nBins, bins);
        
        tmp->SetXTitle(this->hAll->GetXaxis()->GetTitle());

        for (int i = 0; i<this->theProperty->NumberOfBins; i++) {
            double passed = this->GetBinContent(i + 1);
            double all = hAll->GetBinContent(i + 1);

            if (all != 0) {
                double eff = 0;
                double Err = 0;

                eff = passed / all;
                Err = sqrt(passed * (all - passed) / all) / all;
                tmp->SetBinContent(i + 1, eff);
                tmp->SetBinError(i + 1, Err);
            }
        }
        EffPoisson = tmp;
    };

    TH1D* GetHAll()const {
        return hAll;
    };

    TH1D* GetPoissonianEff() {
        return EffPoisson;
    };

    TGraphAsymmErrors * GetBayesianEff() {
        return eff_bay;
    };

    template<class toCheck>
    bool CheckPropType() {
        const toCheck* obj = dynamic_cast<const toCheck*> (this->theProperty);
        return ((obj != NULL) && (obj != 0));
    }
protected:
    TH1D* hAll;
    TH1D* EffPoisson;
    TH1D* IntegratedAll;
    TH1D* IntegratedPass;
    TGraphAsymmErrors *IntegratedEff_bay;
    TGraphAsymmErrors *eff_bay;
    std::string Name;
};

#endif	/* _EfficiencyH1HANDLER_H */

