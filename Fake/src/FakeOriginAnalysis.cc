//#include "UserAnalysis.hh"
//#include "FakeRate/FakeOrigin.h"

#include <sched.h>
cpu_set_t mask_t;

vector<FakeAnalyzer*> analyzers;
int allElecs(0);

UserAnalysis::UserAnalysis(TreeReader *tr) : UserAnalysisBase(tr) {
    Util::SetStyle();

    __CPU_SET(1, &mask_t); //Run over CPU #1
    if (sched_setaffinity(0, sizeof (cpu_set_t), &mask_t) < 0)
        perror("sched_setaffinity");


    DecayChainInfo::ReadPDGTable();
    FakeAnalyzer* pt_10 = new FakeAnalyzer(tr, "pt10", 10);
    analyzers.push_back(pt_10);

    FakeAnalyzer* pt_15 = new FakeAnalyzer(tr, "pt15", 15);
    analyzers.push_back(pt_15);

    FakeAnalyzer* pt_15_conv = new FakeAnalyzer(tr, "pt15Conv", 15, FakeAnalyzer::All, true);
    analyzers.push_back(pt_15_conv);

    FakeAnalyzer* pt_20 = new FakeAnalyzer(tr, "pt20", 20);
    analyzers.push_back(pt_20);
    //
    //    FakeAnalyzer* pt_20_conv = new FakeAnalyzer(tr, "pt20Conv",20 , FakeAnalyzer::All , true);
    //    analyzers.push_back(pt_20_conv);

}

UserAnalysis::~UserAnalysis() {
    //delete pt_10;
}

void UserAnalysis::Begin() {
    // Define the output file of histograms            

    // Define the histograms and add them to histoContainer   
}

void UserAnalysis::Analyze() {
    for (int i = 0; i < analyzers.size(); i++)
        analyzers.at(i)->Analyze();
}

void UserAnalysis::End() {
    TFile* fHistFile = new TFile(fOutputDir + TString("fakes.root"), "RECREATE");

    fHistFile->cd();

    for (int i = 0; i < analyzers.size(); i++)
        analyzers.at(i)->End(fHistFile);

    fHistFile->Close();
}
