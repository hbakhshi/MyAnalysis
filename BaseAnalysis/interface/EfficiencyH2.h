/* 
 * File:   EfficiencyH1.h
 * Author: ajafari
 *
 * Created on January 30, 2009, 3:40 PM
 */

#ifndef _EFFICIENCY_H2
#define	_EFFICIENCY_H2

#include "ObjectProperty.h"
#include "TH2Ext.h"
#include <string>
#include <TCanvas.h>
#include "TDirectory.h"

template<class T>
class EfficiencyH2 : public TH2Ext<T> {
public:

    typedef TH2Ext<T> BASE;

    EfficiencyH2(ObjectProperty<T>* theProp, ObjectProperty<T>* theProp2, std::string name) :
    BASE(theProp, theProp2),
    Name(name) {
        hAll = theProp->GetH2(theProp2, name + "_ALL");

        std::string title = name;
        if (name != "")
            name += ("_PASSED_" + theProp->ObjectName + "_" + BASE::GetMixedName());
        else
            name = theProp->ObjectName + "_" + BASE::GetMixedName();

        BASE::SetNameTitle(name.c_str(), title.c_str());

        int nBins = hAll->GetNbinsX();
        double bins[1000];
        for (int kkk = 0; kkk < nBins; kkk++)
            bins[kkk] = hAll->GetXaxis()->GetBinLowEdge(kkk + 1);
        bins[nBins] = bins[nBins - 1] + hAll->GetXaxis()->GetBinWidth(nBins + 1);

        int nBinsY = hAll->GetNbinsY();
        double binsY[1000];
        for (int kkk = 0; kkk < nBinsY; kkk++)
            binsY[kkk] = hAll->GetYaxis()->GetBinLowEdge(kkk + 1);
        binsY[nBinsY] = binsY[nBinsY - 1] + hAll->GetYaxis()->GetBinWidth(nBinsY + 1);

        BASE::SetBins(nBins,bins, nBinsY,binsY);

        BASE::SetXTitle(theProp->PropertyTitle.c_str());
        BASE::SetYTitle(theProp2->PropertyTitle.c_str());

        AllCondition = 0;

        this->EffPoisson = theProp->GetH2(theProp2, "EffPoissonErr");
//        string poison_name("EffPoissonErr");
//        poison_name = Name + "_" + poison_name + "_" + BASE::GetMixedName();
//
//        EffPoisson->SetNameTitle(poison_name.c_str(), "");
//        EffPoisson->SetBins(theProp->NumberOfBins, theProp->MinValue, theProp->MaxValue, theProp2->NumberOfBins, theProp2->MinValue, theProp2->MaxValue);
//        EffPoisson->SetXTitle(theProp->PropertyTitle.c_str());
//        EffPoisson->SetYTitle(theProp2->PropertyTitle.c_str());
    };

    virtual ~EfficiencyH2() {
    };

    bool (*Condition)(const T*);
    bool (*AllCondition)(const T*);

    virtual int Fill(const T* theObject, double weight = 1) {
        if (this->the1stProperty == 0) {
            cout << "NULL !!!";
            throw std::exception();
        } else {
            bool allPassed = true;
            if (AllCondition != 0)
                allPassed = AllCondition(theObject);

            if (allPassed) {
                double val = BASE::the1stProperty->ReadValue(theObject);
                double val2 = BASE::the2ndProperty->ReadValue(theObject);
                hAll->Fill(val, val2, weight);
                if (Condition(theObject))
                    return TH2D::Fill(val, val2, weight);
            }
        }
        return -10;
    };

    virtual void WriteAll(TDirectory* dir, bool mkdir = true) {
        if (dir == 0)
            return;
        TDirectory* dir2;
        if (mkdir) {
            dir2 = dir->mkdir(BASE::GetMixedName().c_str());
            dir2->cd();
        } else {
            dir->cd();
        }


        hAll->Write();
        BASE::Write();


        EffPoissonErr();
        EffPoisson->Write();
        dir->cd();
    };

    virtual void EffPoissonErr() {
        for (int i = 0; i < BASE::the1stProperty->NumberOfBins; i++) {
            for (int j = 0; j < BASE::the2ndProperty->NumberOfBins; j++) {

                double passed = TH2D::GetBinContent(i + 1, j + 1);
                double all = hAll->GetBinContent(i + 1, j + 1);

                if (all != 0) {
                    double eff = 0;
                    double Err = 0;

                    eff = passed / all;
                    Err = sqrt(passed * (all - passed) / all) / all;
                    EffPoisson->SetBinContent(i + 1, j + 1, eff);
                    EffPoisson->SetBinError(i + 1, j + 1, Err);
                }
            }
        }
    };

    TH2D* GetHAll()const {
        return hAll;
    };

    TH2D* GetPoissonianEff() {
        return EffPoisson;
    };
protected:
    TH2D* hAll;
    TH2D* EffPoisson;
    std::string Name;
};

#endif
