/* 
 * File:   ExtendedFakeEstimator.h
 * Author: hbakhshi
 *
 * Created on August 4, 2010, 6:17 PM
 */

#ifndef _EXTENDEDFAKEESTIMATOR_H
#define	_EXTENDEDFAKEESTIMATOR_H

#include "BaseAlgorithm.h"

using namespace std;

class ExtendedFakeEstimator : public BaseAlgorithm {
public:
    typedef BaseAlgorithm base;
    ExtendedFakeEstimator(TH1F* dirISOConv, TH1F* dirIDConv, TH1F* dirISOO, TH1F* dirIDO, TH1F* dirConvO,
            string LooseCuts, string IsoCuts, string TightCuts , string name , TreeReader* tree);

    virtual ~ExtendedFakeEstimator();

    virtual string MakeString();
    virtual vector<FakeInfo> GetAll();
    virtual void WriteAll(TDirectory* dir = 0);
    
    static TH1F* GetHisto(TDirectory* dir , string DirName){
        dir->cd(DirName.c_str());

        return ( (TH1F*) (gDirectory->Get("hFR_Pt")) );
    };
    int PARAM_EtaRegion;
    int PARAM_ConvRejAll;
private:
    TH1F hFIsoGamma;
    TH1F hFtGamma;
    TH1F hFIsoOthers;
    TH1F hFtOthers;

    TH1F hIsoNotTight;
    TH1F hLooseNotTight;
    TH1F hTight;
    TH1F hTightConv;
    TH1F hTightOthers;

    TH1F hCoefIsoNotTight;
    TH1F hCoefLooseNotTight;
    TH1F hCoefDenominator;

    string isoParams;
    vector<Cut> ISO_Cuts;

    void ClearHistogram(TH1* h);
    inline double ReadHisto(TH1* h, double value){
        return h->GetBinContent( h->GetXaxis()->FindBin(value) ) ;
    };

};

#endif	/* _EXTENDEDFAKEESTIMATOR_H */

