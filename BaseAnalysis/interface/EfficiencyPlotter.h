/* 
 * File:   EfficiencyPlotter.h
 * Author: ajafari
 *
 * Created on February 3, 2009, 6:47 PM
 */

#include "EfficiencyHandler.h"
#include "EfficiencyH1.h"
#include "TString.h"
#include "TCanvas.h"
#include <TSystem.h>
#ifndef _EFFICIENCYPLOTTER_H
#define	_EFFICIENCYPLOTTER_H
template <class G>
class AllSort{
public:
    bool operator()(const EfficiencyH1<G> *lhs ,const EfficiencyH1<G> *rhs ){
        return (lhs->GetHAll()->GetMaximum()>rhs->GetHAll()->GetMaximum());
    }
};
template <class L>
class PassedSort{
public:
    bool operator()(const EfficiencyH1<L> *lhs ,const EfficiencyH1<L> *rhs ){
        return (lhs->GetMaximum()>rhs->GetMaximum());
    }
};

template <class T>
class EfficiencyPlotter{
public:
    EfficiencyPlotter(std::string name):Name(name){
        
    }
    ~EfficiencyPlotter(){};
    void AddHandler(EfficiencyHandler<T> tmp){
        allHandlers.push_back(tmp);
    }
    void Draw(){
        
        gSystem->mkdir(Name.c_str());
        for(uint i = 0; i<allHandlers.at(0).GetProperties(); i++){
            EfficiencyH1<T>* EH1 = allHandlers.at(0).GetEffH1(i);
            std::vector<EfficiencyH1<T>*> SamePropEH1Vec;
            EH1->GetHAll()->SetLineColor(1);
            EH1->SetLineColor(1);
            EH1->GetPoissonianEff()->SetLineColor(1);
            EH1->GetBayesianEff()->SetLineColor(1);
            std::cout<<EH1->GetPropertyName()<<std::endl;
            SamePropEH1Vec.push_back(EH1);
            for(uint j = 1; j<allHandlers.size(); j++){
                for(uint l = 0; l<allHandlers.at(j).GetProperties(); l++){
                    if (EH1->GetPropertyID() == allHandlers.at(j).GetEffH1(l)->GetPropertyID()){
                        ((allHandlers.at(j)).GetEffH1(l))->GetHAll()->SetLineColor(j+1);
                        ((allHandlers.at(j)).GetEffH1(l))->SetLineColor(j+1);
                        ((allHandlers.at(j)).GetEffH1(l))->GetPoissonianEff()->SetLineColor(j+1);
                        ((allHandlers.at(j)).GetEffH1(l))->GetBayesianEff()->SetLineColor(j+1);
                        SamePropEH1Vec.push_back(allHandlers.at(j).GetEffH1(l));
                    }
                    
                }
            }
            std::cout<<SamePropEH1Vec.size()<<std::endl;
            //==========All
            TCanvas All;
            All.cd();
            TString opt = "";
            TH1D * h;
            sort(SamePropEH1Vec.begin(),SamePropEH1Vec.end(),AllSort<T>());
//            std::cout<<"All in "<<EH1->GetPropertyName()<<" is sorted"<<std::endl;
            for(uint j = 0; j<SamePropEH1Vec.size(); j++){
//                std::cout<<"-------------"<<std::endl;
                h = SamePropEH1Vec.at(j)->GetHAll();
                h->SetStats(kFALSE);
                std::cout<<"-------------"<<std::endl;
                h->SetLineWidth(SamePropEH1Vec.size()-j);
                if (j!=0) opt = "sames";
//                std::cout<<"-------------"<<std::endl;
                h->Draw(opt);
//                std::cout<<"-------------"<<std::endl;
            }
            std::string Cname = Name + "/" + (SamePropEH1Vec.at(0))->GetPropertyName() +"_All.C";
            All.Update();
//            std::cout<<"All is finished"<<std::endl;
            All.SaveAs(Cname.c_str());
//            std::cout<<"All is finished"<<std::endl;
            //===========Passed
            //sort for maxima
            sort(SamePropEH1Vec.begin(),SamePropEH1Vec.end(),PassedSort<T>());
//            std::cout<<"Passed in "<<EH1->GetPropertyName()<<" is sorted"<<std::endl;
            TCanvas Passed;
            Passed.cd();
            opt = "";
            for(uint j = 0; j<SamePropEH1Vec.size(); j++){
                (SamePropEH1Vec.at(j))->SetStats(kFALSE);
                (SamePropEH1Vec.at(j))->SetLineWidth(SamePropEH1Vec.size()-j);
                if (j!=0) opt="sames";
                (SamePropEH1Vec.at(j))->Draw(opt);
            }
            Cname = Name + "/" + (SamePropEH1Vec.at(0))->GetPropertyName() +"_Passed.C";
            Passed.Update();
            Passed.SaveAs(Cname.c_str());
            
            //===========PEff
            TCanvas Poisson;
            Poisson.cd();
            opt = "";
            for(uint j = 0; j<SamePropEH1Vec.size(); j++){
                h = (SamePropEH1Vec.at(j))->GetPoissonianEff();
                h->SetStats(kFALSE);
                //h->GetYaxis()->SetRangeUser(-0.01,1.01);
                h->SetLineWidth(SamePropEH1Vec.size()-j);
                if (j!=0) opt="sames";
                h->Draw(opt);
            }
            
            Cname =  Name + "/" + (SamePropEH1Vec.at(0))->GetPropertyName() +"_PEff.C";
            Poisson.Update();
            Poisson.SaveAs(Cname.c_str());
            
            //============BEff
            TCanvas Bayes;
            Bayes.cd();
            opt = "AP";
            for(uint j = 0; j<SamePropEH1Vec.size(); j++){
                TGraphAsymmErrors *tga = (SamePropEH1Vec.at(j))->GetBayesianEff();
                //tga->GetYaxis()->SetRangeUser(0,1.01);
                if (j!=0) opt="P";
                tga->Draw(opt);
            }
            Cname =  Name + "/" + (SamePropEH1Vec.at(0))->GetPropertyName() +"_BEff.C";
            Bayes.Update();
            Bayes.SaveAs(Cname.c_str()); 
        }
        
    }
private:
    std::string Name;
    std::vector<EfficiencyHandler<T> > allHandlers;
};

#endif	/* _EFFICIENCYPLOTTER_H */

