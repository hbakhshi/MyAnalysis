/* 
 * File:   SameFlavourDiLeptonFakeEstimator.cc
 * Author: hbakhshi
 * 
 * Created on February 25, 2011, 7:58 PM
 */

#include <Reflex/Base.h>

#include "Fake/include/SameFlavourDiLeptonFakeEstimator.h"
#include "boost/detail/container_fwd.hpp"

SameFlavourDiLeptonFakeEstimator::SameFlavourDiLeptonFakeEstimator(const edm::ParameterSet& ps) : base(ps) {

    TotalFP = 0.0;
    TotalFF = 0.0;
    TotalPP = 0.0;

    TotalT0 = 0.0;
    TotalT1 = 0.0;
    TotalT2 = 0.0;

    isElectron = ps.getParameter<bool>("isElectron");
    TightTag = ps.getParameter<string > ("TightTag");
    LooseTag = ps.getParameter<string > ("LooseTag");

    FakeRateDirectoryName = ps.getParameter<string > ("FakeRateDirectoryName");
    FakeRateFileName = ps.getParameter<string > ("FakeRateFileName");

    binnedPt = ps.getParameter<bool>("binnedPt");
    AbsEta = ps.getParameter<bool>("AbsEta");
    EndcapBarrel = ps.getParameter<bool>("EndcapBarrel");

    string objName = "Mu";
    if (isElectron)
        objName = "El";

    BaseAlgorithm::FakeInfo::PT_Property* pt_prop;
    if (this->binnedPt) {
        std::vector<double> doubles;
        doubles.push_back(1.0);
        pt_prop = new BaseAlgorithm::FakeInfo::PT_Property(objName, &doubles);
    } else
        pt_prop = new BaseAlgorithm::FakeInfo::PT_Property(objName);

    BaseAlgorithm::FakeInfo::Eta_Property* eta_prop = new BaseAlgorithm::FakeInfo::Eta_Property(objName, EndcapBarrel, AbsEta);

    BaseAlgorithm::FakeInfo::All_Property* all_prop = new BaseAlgorithm::FakeInfo::All_Property(objName);

    try {
        TFile f(FakeRateFileName.c_str(), "READ");
        EfficiencyHandlerReader<BaseAlgorithm::FakeInfo> DirectoryReader(&f, FakeRateDirectoryName);
        Efficiency2DObjects objs(DirectoryReader.Get(pt_prop, eta_prop));
        TH2D htmp(*((TH2D*) objs.hEff->Clone("hFakeRate")));

        Efficiency1DObjects obj_total(DirectoryReader.Get(all_prop));
        TotalFR = obj_total.hEff->GetBinContent(1);
        TotalFR_Err = obj_total.hEff->GetBinError(1);
        cout << "TotalFR : " << TotalFR << "+-" << TotalFR_Err << endl;
        f.Close();

        disabled = false;

        hFakeRate = new TH2D(htmp);

        //        LucFakeWeightEstimator.SetPratios(1.0, 0.0, 1.0, 0.0);
        //        LucFakeWeightEstimator.SetFratios(0.0, 0.0, 0.0, 0.0);

        if (isElectron) {
            //cout << "All set to 0 and 1" << endl;
            this->LucFakeWeightEstimator.SetElFratios(hFakeRate);
            //hFakeRate->Print("all");
            this->LucFakeWeightEstimator.SetElPratios(0.94, 0.0);
        } else {
            this->LucFakeWeightEstimator.SetMuFratios(hFakeRate);
            this->LucFakeWeightEstimator.SetMuPratios(0.98, 0.0);
        }


        vector<double> doubles_tmp(3, 0.0);

        this->LucFakeWeightEstimator.NevtTopol((isElectron ? 2 : 0), (isElectron ? 0 : 2), doubles_tmp);
    } catch (EfficiencyHandlerReaderException exp) {
        cout << exp.what() << endl;
        disabled = true;
    }
}

SameFlavourDiLeptonFakeEstimator::~SameFlavourDiLeptonFakeEstimator() {

}

bool SameFlavourDiLeptonFakeEstimator::Run(TreeReaderEvent* ev) {
    float* PTS;
    float* ETAS;
    int* CHARGES;
    vector<int>* LooseIDS;
    vector<int>* TightIDS;

    if (isElectron) {
        PTS = ev->ElPt;
        ETAS = ev->ElEta;
        CHARGES = ev->ElCharge;
        LooseIDS = &(ev->ElectronsToAnalyze[LooseTag]);
        TightIDS = &(ev->ElectronsToAnalyze[TightTag]);
    } else {
        PTS = ev->MuPt;
        ETAS = ev->MuEta;
        CHARGES = ev->MuCharge;
        LooseIDS = &(ev->MuonsToAnalyze[LooseTag]);
        TightIDS = &(ev->MuonsToAnalyze[TightTag]);
    }

    vector<double> pts;
    vector<double> etas;
    vector<int> pass;

    if (LooseIDS->size() >= 2) {

        int charge(1);
        int nTights = 0;

        for (vector<int>::const_iterator loose_idx = LooseIDS->begin(); loose_idx != LooseIDS->end(); loose_idx++) {
            pts.push_back(*(PTS + *loose_idx));

            if (this->AbsEta)
                etas.push_back(fabs(*(ETAS + *loose_idx)));
            else
                etas.push_back(*(ETAS + *loose_idx));

            bool isTight = (std::find(TightIDS->begin(), TightIDS->end(), *loose_idx) != TightIDS->end());
            pass.push_back((isTight ? 1 : 0));

            charge *= (*(CHARGES + *loose_idx));

            nTights += (isTight ? 1 : 0);
        }


        if( (LooseIDS->size() == 2 && charge < 0 ) || LooseIDS->size() > 2 || ev->PFMET < 20.0)
            return false;

        double WFP(0.0);
        double WFF(0.0);
        double WPP(0.0);

        int base_sign(1);
        if (nTights == 1) //Formula in the page 12 of cms an-2010/261
            base_sign = (-1);

        if (isElectron) {
            WFP = base_sign * -1 * LucFakeWeightEstimator.FPWeight(1, 1, 0, 0, pts, etas, pass);

            //            if (nTights == 1)
            //                this->LucFakeWeightEstimator.SetVerbose(10);
            //
            WFF = base_sign * 1 * LucFakeWeightEstimator.FPWeight(0, 2, 0, 0, pts, etas, pass);

            //            this->LucFakeWeightEstimator.SetVerbose(0);

            WPP = base_sign * 1 * LucFakeWeightEstimator.FPWeight(2, 0, 0, 0, pts, etas, pass);
        } else {
            WFP = base_sign * -1 * LucFakeWeightEstimator.FPWeight(0, 0, 1, 1, pts, etas, pass);
            WFF = base_sign * 1 * LucFakeWeightEstimator.FPWeight(0, 0, 0, 2, pts, etas, pass);
            WPP = base_sign * 1 * LucFakeWeightEstimator.FPWeight(0, 0, 2, 0, pts, etas, pass);
        }

        TotalFF += WFF;
        TotalFP += WFP;
        TotalPP += WPP;

        switch (nTights) {
            case 0:
                TotalT0++;
                break;
            case 1:
                TotalT1++;
                break;
            case 2:
                TotalT2++;
                cout << endl << ev->Run <<  " -- " << ev->Event << endl;
                break;
        }
    }
}

void SameFlavourDiLeptonFakeEstimator::End() {

    cout << this->Name << ":" << endl;
    cout << "----------------------------From Pt Eta :" << endl;
    cout << "TotalPP = " << TotalPP << endl;
    cout << "TotalFP = " << TotalFP << endl;
    cout << "TotalFF = " << TotalFF << endl;

    cout << "----------------------------From Total :" << endl;

    int ne = (isElectron ? 2 : 0), nm = (isElectron ? 0 : 2);
    FPRatios fprTotal;
    vector<double> vnpass;
    //    this->TotalT0 = 275.00;
    //    this->TotalT1 = 86.00 ;
    //    this->TotalT2 = 40.00 ;
    vnpass.push_back(this->TotalT0);
    vnpass.push_back(this->TotalT1);
    vnpass.push_back(this->TotalT2);
    
    if (isElectron) {
        fprTotal.SetElFratios(TotalFR, TotalFR_Err);
        fprTotal.SetElPratios(0.95, 0.05);
    } else {
        fprTotal.SetElFratios(TotalFR, TotalFR_Err);
        fprTotal.SetMuPratios(0.98, 0.02);
    }
    fprTotal.NevtTopol(ne, nm, vnpass);


    // compute all the N_nfe,npe,nfm,npm and their errors
    vector<double> nevFP = fprTotal.NevtPass();
    vector<double> dNtotstat = fprTotal.NevtPassErrStat();
    vector<double> dNtotsyst = fprTotal.NevtPassErrSyst();

    // loop over all FPconfigs and print the values
    int it = -1;
    cout << endl;
    for (int ie = 0; ie < ne + 1; ++ie) {
        for (int im = 0; im < nm + 1; ++im) {
            it++;
            string NNN;
            switch(it){
                case 0:
                    NNN="TotalPP";
                    break;
                case 1:
                    NNN="TotalFP";
                    break;
                case 2:
                    NNN="TotalFF";
                    break;
            }
            double ntot = nevFP[it];
            double dntotstat = dNtotstat[it];
            double dntotsyst = dNtotsyst[it];
            cout <<  NNN << " = " << ntot << " +- " << dntotstat << "_stat +- " << dntotsyst << "_syst" << endl;
        }
    }

    cout << "----------------------------" << endl;

    cout << "TotalT0 = " << TotalT0 << endl;
    cout << "TotalT1 = " << TotalT1 << endl;
    cout << "TotalT2 = " << TotalT2 << endl;

    cout << "----------------------------" << endl;


}
