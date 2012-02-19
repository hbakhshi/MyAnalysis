#include <TH2.h>

#include "helper/Utilities.hh"
//#include "UserAnalysis.hh"


//#include "FakeRate/Factory.h"
//#include "FakeRate/ExtendedFakeEstimator.h"

//#include "FakeRate/FakeOrigin.h"

using namespace std;


#include <sched.h>
cpu_set_t mask_t;

UserAnalysis::UserAnalysis(TreeReader *tr) : UserAnalysisBase(tr) {
    Util::SetStyle();

    __CPU_SET(1, &mask_t); //Run over CPU #1
    if (sched_setaffinity(0, sizeof (cpu_set_t), &mask_t) < 0)
        perror("sched_setaffinity");

    for (vector<BaseAlgorithm*>::iterator ba = Factory::allFR.begin(); ba != Factory::allFR.end(); ba++)
        (*ba)->SetTree(tr);
}

UserAnalysis::~UserAnalysis() {
}

vector<double> counts_pt;
vector<double> counts_eta;
vector<double> counts_phi;

map<BaseAlgorithm*, double> counts_extended;
map<BaseAlgorithm*, double> counts_extended_gamma;
map<BaseAlgorithm*, double> counts_passing;
map<BaseAlgorithm*, double> counts_passing_gamma;

void UserAnalysis::Begin() {
    for (uint i = 0; i < Factory::allFR.size(); i++) {
        counts_pt.push_back(0.0);
        counts_eta.push_back(0.0);
        counts_phi.push_back(0.0);
    }

    cout << "-a-b-c-d-e-f-g-------" << endl;
    ExtendedFakeEstimator* extended = new ExtendedFakeEstimator(ExtendedFakeEstimator::GetHisto(Factory::theFile, "BremIsoBarrel"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "BremIDBarrel"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "OIsoBarrel"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "OIDBarrel"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "OConvBarrel"),
            "Pt(15,*)&RelIso04(*,0.7)&IDRobustLoose(1,1)",
            "Pt(15,*)&RelIso04(*,0.3)&IDRobustLoose(1,1)",
            "Pt(15,*)&RelIso04(*,0.3)&IDRobustLoose(1,1)&IDRobustTight(1,1)",
            "estimate_barrel", this->fTR
            );
    extended->PARAM_ConvRejAll = true;
    extended->PARAM_EtaRegion = 1;

    Factory::allFR.push_back(extended);
    counts_extended[extended] = 0.0;
    counts_extended_gamma[extended] = 0.0;
    counts_passing[extended] = 0.0;
    counts_passing_gamma[extended] = 0.0;

    ExtendedFakeEstimator* extended_endcap = new ExtendedFakeEstimator(ExtendedFakeEstimator::GetHisto(Factory::theFile, "BremIsoEnd"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "BremIDEnd"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "OIsoEnd"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "OIDEnd"),
            ExtendedFakeEstimator::GetHisto(Factory::theFile, "OConvEnd"),
            "Pt(15,*)&RelIso04(*,0.7)&IDRobustLoose(1,1)",
            "Pt(15,*)&RelIso04(*,0.3)&IDRobustLoose(1,1)",
            "Pt(15,*)&RelIso04(*,0.3)&IDRobustLoose(1,1)&IDRobustTight(1,1)",
            "estimate_end" , this->fTR
            );
    extended_endcap->PARAM_ConvRejAll = true;
    extended_endcap->PARAM_EtaRegion = 2;

    Factory::allFR.push_back(extended_endcap);
    counts_extended[extended_endcap] = (0.0);
    counts_extended_gamma[extended_endcap] = (0.0);
    counts_passing[extended_endcap] = (0.0);
    counts_passing_gamma[extended_endcap] = (0.0);
}

void UserAnalysis::Analyze() {

    for (int alg_i = 0; alg_i < Factory::allFR.size(); alg_i++) {
        BaseAlgorithm* algo = Factory::allFR[alg_i];

        vector<BaseAlgorithm::FakeInfo> fis = algo->GetAll();

        if (algo->MakeString() == "EXTENDED") {
            for (uint fi_i = 0; fi_i < fis.size(); fi_i++) {
                counts_extended[algo] += fis[fi_i].coeff1;
                
                double tmp_gamma = counts_extended_gamma[algo] + fis[fi_i].coeff2;
                if(tmp_gamma != NAN && tmp_gamma != nan("nan"))
                    counts_extended_gamma[algo] = tmp_gamma;
                
                counts_passing[algo] += fis[fi_i].coeff3;

                if(this->fTR->ElGenID[fis[fi_i].FakableObject.index] == 22)
                    counts_passing_gamma[algo] += fis[fi_i].coeff3;                
            }
        } else {
            for (uint fi_i = 0; fi_i < fis.size(); fi_i++) {
                counts_eta[alg_i] += fis[fi_i].Probabilities.eta;
                counts_pt[alg_i] += fis[fi_i].Probabilities.pt;
                counts_phi[alg_i] += fis[fi_i].Probabilities.phi;
            }
        }
    }
}

void UserAnalysis::End() {
    //    const char* filename = "estimates.root";
    //    fHistFile = new TFile(fOutputDir + TString(filename), "RECREATE");
    //
    //    fHistFile->cd();

    cout << "-----------------" << endl;
    for (int alg_i = 0; alg_i < Factory::allFR.size(); alg_i++) {
        BaseAlgorithm* algo = Factory::allFR[alg_i];

        if(algo->MakeString() == "EXTENDED"){
            cout << algo->algoName << " : " << counts_extended[algo] << "," << counts_passing[algo] << "(photons=" << counts_extended_gamma[algo] << "," << counts_passing_gamma[algo] << ")" << endl;

            algo->WriteAll(0);
        }
        else
            cout << algo->algoName << " : " << counts_pt[alg_i] << "," << counts_eta[alg_i] << "," << counts_phi[alg_i] << endl;

        delete algo;
    }
    cout << "-----------------" << endl;



    //    fHistFile->Close();
}
