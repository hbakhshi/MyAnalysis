#include <TH2.h>

#include "helper/Utilities.hh"

#include "../include/EleIsoTemplate.h"
#include "../include/EleJetFR.hh"
#include "../include/CalculateFakeRate.h"
#include "Fake/include/MuonIsoTemplate.h"

using namespace std;

CalculateFakeRate::CalculateFakeRate(const edm::ParameterSet& ps) : CalculateFakeRate::base(ps),
AllFRPS(ps.getParameterSetVector("AllFR")),
E_IsoL("E_IsoL", "iso of electrons", 50, 0, 2.5),
E_IsoT("E_IsoT", "iso of electrons", 50, 0, 2.5),
E_IsoID("E_IsoID", "id vs iso", 7, 0, 0.7, 2, 0, 2) {
    for (edm::VParameterSet::const_iterator frCalc = AllFRPS.begin(); frCalc != AllFRPS.end(); frCalc++) {
        BaseAlgorithm* FRAlg = NULL;

        string params = frCalc->getParameter<string > ("AlgParams") + "_" +
                frCalc->getParameter<string > ("FOParams") + "_" + frCalc->getParameter<string > ("EleParams");
        switch (frCalc->getParameter<int>("Type")) {
            case 0: //EleIsoTemplate
                FRAlg = new EleIsoTemplate(frCalc->getParameter<string > ("Name"), params, NULL);
                if(frCalc->existsAs<string>("FakableObjectTag"))
                    ((EleIsoTemplate*)FRAlg)->FakableElectronTag = frCalc->getParameter<string>("FakableObjectTag");
                else
                    ((EleIsoTemplate*)FRAlg)->FakableElectronTag = "";

                if(frCalc->existsAs<string>("ElectronTag"))
                    ((EleIsoTemplate*)FRAlg)->ElectronTag = frCalc->getParameter<string>("ElectronTag");
                else
                    ((EleIsoTemplate*)FRAlg)->ElectronTag = "";

                break;
            case 1: //EleJetFR
                FRAlg = new EleJetFR(frCalc->getParameter<string > ("Name"), params, NULL);
                break;
            case 2: //MuonIsoTemplate
                FRAlg = new MuonIsoTemplate(frCalc->getParameter<string>("Name"), params, NULL);
                if(frCalc->existsAs<string>("FakableObjectTag"))
                    ((MuonIsoTemplate*)FRAlg)->FakableMuonTag = frCalc->getParameter<string>("FakableObjectTag");
                else
                    ((MuonIsoTemplate*)FRAlg)->FakableMuonTag = "";

                if(frCalc->existsAs<string>("MuonTag"))
                    ((MuonIsoTemplate*)FRAlg)->MuonTag = frCalc->getParameter<string>("MuonTag");
                else
                    ((MuonIsoTemplate*)FRAlg)->MuonTag = "";

                break;
        }

        allFR.push_back(FRAlg);
    }
}

CalculateFakeRate::~CalculateFakeRate() {

}

bool CalculateFakeRate::Run(TreeReaderEvent* ev) {
    for (int alg_i = 0; alg_i < allFR.size(); alg_i++) {
        BaseAlgorithm* algo = allFR[alg_i];
        algo->SetTree(ev);
        //cout << algo->algoName << endl;
        algo->GetAll();
        //cout << algo->algoName << endl;
    }

    for (int ei = 0; ei < ev->NEles; ei++) {
        bool loose = ev->ElIDRobustLoose[ei];
        bool tight = ev->ElIDRobustTight[ei];
        double iso = ev->ElRelIso04[ei];

        if (loose)
            E_IsoL.Fill(iso);

        if (tight)
            E_IsoT.Fill(iso);

        E_IsoID.Fill(iso, tight ? 1.5 : (loose ? 0.5 : -0.5));
    }
}

void CalculateFakeRate::End() {
    TFile* fHistFile = info::TheInfo->OutFile;
    fHistFile->cd();

    cout << "|Algo Name | #Tight / #Loose | 100 * (FR ± Error) |" << endl;
    for (int alg_i = 0; alg_i < allFR.size(); alg_i++) {
        BaseAlgorithm* algo = allFR[alg_i];

        algo->WriteAll(fHistFile);
    }

    fHistFile->cd();

    E_IsoL.Write();
    E_IsoT.Write();
    E_IsoID.Write();

    fHistFile->Close();
}
