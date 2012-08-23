/* 
 * File:   CosThetaAnalysis.h
 * Author: hbakhshi
 *
 * Created on February 1, 2012, 3:31 PM
 */

#ifndef COSTHETAANALYSIS_H
#define	COSTHETAANALYSIS_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "TH1.h"
#include "TH3.h"
#include "TF1.h"
#include "TFitResult.h"
#include <boost/lexical_cast.hpp>

class CosThetaAnalysis : public Analyzer<TopAnalysis::TTBarDileptonicEvent> {
public:
    typedef Analyzer<TopAnalysis::TTBarDileptonicEvent> BASE;

    class dGammaFunctin {
    public:

        dGammaFunctin() {

        }

        double operator() (double *x, double *p) {
            double FNeg = *p;
            double F0   = *(p+1);
            double FPos = (1.0 - FNeg - F0);
            
            double norm_factor = *(p+2);
            
            double costheta = *x ;
            
            double aa = (1.0 - costheta )*(1.0 - costheta );
            double bb = (1.0 - costheta*costheta);
            double cc = (1.0 + costheta )*(1.0 + costheta );
            
            double ret(0.0);
            ret = (FNeg*3.0*aa / 8.0) ;
            ret += (F0*3.0*bb / 4.0 ) ;
            ret += (FPos*3.0*cc / 8.0);
            
            return norm_factor*ret;
        };
        
        static TF1* GetFunction(string name){
            dGammaFunctin* functor = new dGammaFunctin();
            
            TF1* ret = new TF1(name.c_str() , functor , -1.0 , 1.0 , 3);
            
            ret->SetParName(0 , "FNeg" );
            ret->SetParName(1 , "F0" );
            ret->SetParName(2 , "NormaFactor" );
            
            ret->SetParLimits(0 , 0.0 , 1.0);
            ret->SetParLimits(1 , 0.0 , 1.0);
            
            ret->SetParameters( 0.4 , 0.6 , 1.0 );
            
            return ret;
        };
        
        static TH1* Fit(TH1* hToFit , int nbins = -1,bool normalize = true, bool fixNormFactor = false){
            string newName(hToFit->GetName());
            newName += "_Fitted";
            TH1* hret;
            
            if(nbins > 0){
                int ngroups = (hToFit->GetNbinsX()/nbins);
                newName += "_Nins" + boost::lexical_cast<string>(nbins);
                
                newName += ( fixNormFactor ? "_FixNormFactor" :  "");                    
                
                hret = hToFit->Rebin(ngroups , newName.c_str());
            }else
                hret = (TH1*)hToFit->Clone( newName.c_str() );
            
            if(normalize)
                hret->Scale( 1./hToFit->Integral() );
                            
            TF1* f = GetFunction( string(hToFit->GetName()) );
                
            if(fixNormFactor)
                f->FixParameter(2 , 1.0);
            
            TFitResultPtr fitres(hret->Fit(f , "BQRS"));
            cout << hret->GetName() ;
            fitres->Print("ALL");
            
            double f_int=f->Integral(-1.0 , 1.0);
            double h_int = hret->Integral();
            //hret->Scale( f_int / h_int );
            //cout << h_int << "  " << hret->Integral() << "   " << f_int << endl;
            //f->Print("ALL");
            
            return hret;
        }
    };

    TopAnalysis::DiLeptonTTBarEventProperties::EventType EventTypeReader;
    std::vector<double> AcceptedEventTypes;

    string solverName;
    TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions solverSolution;

    bool FillGen;
    bool FillRec;
    bool FillTree;
    vector<TopAnalysis::TTBarDileptonicEvent::TopDecays> GenDecayModes;
    
    TH1* hCosThetaPosLepton;
    TH1* hCosThetaNegLepton;

    TH1* hCosThetaPosLepton_Gen;
    TH1* hCosThetaNegLepton_Gen;
    TH2* hCosThetaPtLep_Gen;

    TH1* hCosTheta1stLepton;
    TH1* hCosTheta2ndLepton;

    TH1* hCosThetaAllLeptonsUnWeighted;
        
    TH2* hCosThetaAllLeptonsVsLeptonIsolation;
    TH2* hCosThetaAllLeptonsVsLeptonJetDR;
    TH2* hCosThetaAllLeptonsVsLeptonPt;
    TH2* hCosThetaAllGenVsREC;
    TH3* hCosThetaAllGenVsRECVsDR;
    std::map<int , TTree*> allTrees;
    double genCosThetaValueHolder;
    double recCosThetaValueHolder;
    double eventWeight;
    bool   isElectron;
    double LeptonPt;
    double LeptonEta;
    int nPU;
    int nPV;
    int eventType;
    TFile* fileTree ;
    
    CosThetaAnalysis(const edm::ParameterSet& ps);
    virtual ~CosThetaAnalysis();

    bool Run(TopAnalysis::TTBarDileptonicEvent* ev);

    virtual void End();
private:

};

#endif	/* COSTHETAANALYSIS_H */

