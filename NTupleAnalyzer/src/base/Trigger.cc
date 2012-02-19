#include "NTupleAnalyzer/include/base/Trigger.h"

void TriggerCutSingleRunRange::Print() const {
    cout << "|" << Name << "|" << MinRun << "|" << MaxRun << "|";
    for (uint i = 0; i < HLTs.size(); i++)
        cout << HLTs[i] << ",";

    cout << "|" << endl;
}

TriggerCutFullRunRange::TriggerCutFullRunRange(const edm::VParameterSet& input, string name) : Name(name) {
    for (edm::VParameterSet::const_iterator itr = input.begin();
            itr != input.end(); itr++) {

        TriggerCutSingleRunRange* trg = new TriggerCutSingleRunRange();

        trg->MinRun = itr->getParameter<int > ("MinRun");
        trg->MaxRun = itr->getParameter<int > ("MaxRun");
        trg->HLTs = itr->getParameter< vector<string> > ("HLTNames");
        trg->Name = itr->getParameter< string > ("Name");
        trg->SkipRunsWithInconsistentMenu = itr->getUntrackedParameter< bool > ("SkipRunsWithInconsistentMenu", false);

        //to remove the duplicated names of HLT's
        std::vector<string>::iterator it = unique(trg->HLTs.begin(), trg->HLTs.end());
        trg->HLTs.resize(it - trg->HLTs.begin());

        Cuts.push_back(trg);
    }

}

void TriggerCutFullRunRange::Print() const {
    std::cout << Name << endl;
    std::cout << "-----" << endl;
    for (uint i = 0; i < Cuts.size(); i++) {
        Cuts[i]->Print();
    }
}

const TriggerCutSingleRunRange* TriggerCutFullRunRange::FindARun(int run) const {
    for (uint i = 0; i < Cuts.size(); i++) {
        if (run >= Cuts[i]->MinRun)
            if (run <= Cuts[i]->MaxRun)
                return Cuts[i];
    }

    return NULL;
}

TriggerAnalyzer::TriggerAnalyzer(TriggerCutFullRunRange* trgInf) {
    TriggerInfo = trgInf;

    string hName = this->TriggerInfo->Name + "_" + "hAllDifferentRunRanges";
    hAllDifferentRunRanges.SetNameTitle(hName.c_str(), hName.c_str());
    hAllDifferentRunRanges.SetBins(this->TriggerInfo->Cuts.size(), 0, this->TriggerInfo->Cuts.size());
    for (uint i = 0; i<this->TriggerInfo->Cuts.size(); i++)
        this->hAllDifferentRunRanges.GetXaxis()->SetBinLabel(i + 1, this->TriggerInfo->Cuts[i]->Name.c_str());

    hName = this->TriggerInfo->Name + "_" + "hPassedDifferentRunRanges";
    hPassedDifferentRunRanges.SetNameTitle(hName.c_str(), hName.c_str());
    hPassedDifferentRunRanges.SetBins(this->TriggerInfo->Cuts.size(), 0, this->TriggerInfo->Cuts.size());
    for (uint i = 0; i<this->TriggerInfo->Cuts.size(); i++)
        this->hPassedDifferentRunRanges.GetXaxis()->SetBinLabel(i + 1, this->TriggerInfo->Cuts[i]->Name.c_str());
    
    CurrentRunRangeSkip = false;
}

void TriggerAnalyzer::SetPointers(int* currentRun, int* HLTRes) {
    CurrentRun = currentRun;
    HLTResutls = HLTRes;

}

void TriggerAnalyzer::Write(TDirectory* dir) {
    dir->cd(this->TriggerInfo->Name.c_str());

    hAllDifferentRunRanges.Write();
    hPassedDifferentRunRanges.Write();

    string hName = this->TriggerInfo->Name + "_" + "hPassedEfficiency";
    TH1D hPassEfficiency;
    hPassEfficiency.SetNameTitle(hName.c_str(), hName.c_str());
    hPassEfficiency.SetBins(this->TriggerInfo->Cuts.size(), 0, this->TriggerInfo->Cuts.size());
    for (uint i = 0; i<this->TriggerInfo->Cuts.size(); i++){
        hPassEfficiency.GetXaxis()->SetBinLabel(i + 1, this->TriggerInfo->Cuts[i]->Name.c_str());
        
        hPassEfficiency.SetBinContent(i+1,hPassedDifferentRunRanges.GetBinContent(i+1)/hAllDifferentRunRanges.GetBinContent(i+1));
    }
    hPassEfficiency.Write();
    
    dir->cd();
}

void TriggerAnalyzer::OnRunChange(int run, std::map<string, int>* hltNames) {
    if (*CurrentRun != run)
        throw TriggerException("TriggerAnalyzer::OnRunChange ==> run number doesn't match");

    const TriggerCutSingleRunRange* CurrentRunRange = TriggerInfo->FindARun(run);

    this->CurrentRunRangeName = CurrentRunRange->Name;
    const vector<string>* CurrentRunHLTs = &(CurrentRunRange->HLTs);

    if (CurrentRunHLTs == NULL) {
        stringstream str;
        str << "In trigger menu " << TriggerInfo->Name << " run " << run << " is not specified.";

        throw TriggerException(str.str());
    }

    CurrentBits.clear();
    if (CurrentBits.size() > 0)
        throw TriggerException("TriggerAnalyzer::OnRunChange ==> CurrentBits isn't clean");

    std::map<string, int>::iterator itr;
    for (vector<string>::const_iterator hltName = CurrentRunHLTs->begin(); hltName != CurrentRunHLTs->end(); hltName++) {
        itr = hltNames->find(*hltName);
        if (itr == hltNames->end()) {
            stringstream sss;
            sss << "TriggerAnalyzer::OnRunChange ==> " + TriggerInfo->Name << " run " << run << " - Bit with name " + *hltName + " not found!";
            if (CurrentRunRange->SkipRunsWithInconsistentMenu)
                cout << sss.str() << endl;
            else
                throw TriggerException(sss.str());
        } else
            CurrentBits.push_back(itr->second);
    }
    
    this->CurrentRunRangeSkip = CurrentRunRange->SkipRunsWithInconsistentMenu;
}

bool TriggerAnalyzer::Analyze(int run) {
    if (run != *CurrentRun)
        throw TriggerException("TriggerAnalyzer::Analyze ==> Please first call the OnRunChange method");

    bool ret = false;
    for (std::vector<int>::iterator hltbit = CurrentBits.begin(); hltbit != CurrentBits.end(); hltbit++)
        ret = (ret || (*(HLTResutls + *hltbit) != 0));

    if(CurrentBits.size() == 0 && this->CurrentRunRangeSkip)
        ret = true;
    
    hAllDifferentRunRanges.Fill(CurrentRunRangeName.c_str(), 1);
    if (ret) hPassedDifferentRunRanges.Fill(CurrentRunRangeName.c_str(), 1);

    return ret;
}

VTriggerCuts::VTriggerCuts() : BASE(),
AllHistograms("TriggerResults", true, true, false, true) {
    AllHistograms.CreateHistos("AllEvents");
};

void VTriggerCuts::SetHistosEnable(bool enable) {
    AllHistograms.Enabled = enable;
};

void VTriggerCuts::AddHistogram(ObjectProperty<int>* histo) {
    AllHistograms.AddHisto1ToAll(histo);
};

void VTriggerCuts::CreateAnalyzers() {
    for (BASE::iterator itr = this->begin(); itr != this->end(); itr++) {
        TriggerAnalyzer* trganal = new TriggerAnalyzer(&(*itr));
        Analyzers.push_back(trganal);

        AllHistograms.CreateHistos(itr->Name);
    }
};

void VTriggerCuts::End(TDirectory* dir) {
    AllHistograms.WriteAll(dir);

    if (AllHistograms.Enabled) {
        dir->cd("TriggerResults");
        for (std::vector<TriggerAnalyzer*>::iterator analyzer = Analyzers.begin();
                analyzer != Analyzers.end(); analyzer++)
            (*analyzer)->Write(gDirectory);
    }
}

void VTriggerCuts::SetAnalyzerPointers(int* currentRun, int* HLTRes) {
    for (std::vector<TriggerAnalyzer*>::iterator analyzer = Analyzers.begin();
            analyzer != Analyzers.end(); analyzer++) {
        (*analyzer)->SetPointers(currentRun, HLTRes);
    }
}

void VTriggerCuts::RunChanged(int run, std::map<string, int>* hltNames) {
    for (std::vector<TriggerAnalyzer*>::iterator analyzer = Analyzers.begin();
            analyzer != Analyzers.end(); analyzer++) {
        (*analyzer)->OnRunChange(run, hltNames);
    }
}

void VTriggerCuts::RunAllAnalyzers(int run) {
    HistogramsManager<int>::iterator Histos = AllHistograms.begin();
    FillAllValue(*Histos);
    for (std::vector<TriggerAnalyzer*>::iterator analyzer = Analyzers.begin();
            analyzer != Analyzers.end(); analyzer++) {
        bool res = (*analyzer)->Analyze(run);

        Histos++;
        if (res)
            FillAllValue(*Histos);

        LastResults[(*analyzer)->TriggerInfo->Name] = res;
    }
};

void VTriggerCuts::Print() const {
    for (BASE::const_iterator itr = BASE::begin(); itr != BASE::end(); itr++)
        itr->Print();
}

void VTriggerCuts::FillAllValue(Histograms<int>* histos) {
    if (!AllHistograms.Enabled)
        return;

    for (uint i = 0; i < histos->GetSizeH1(); i++) {
        TH1Ext<int>* hhh = histos->GetHistogram1(i);
        int maxVal = -1;
        const ObjectPropertyFromIntArray* objInt = dynamic_cast<const ObjectPropertyFromIntArray*> (hhh->theProperty);
        const ObjectPropertyFromDblArray* objDbl = dynamic_cast<const ObjectPropertyFromDblArray*> (hhh->theProperty);
        if (objInt != 0) {
            if (objInt->MaxPossibleIndex != NULL)
                maxVal = *(objInt->MaxPossibleIndex);
        } else if (objDbl != 0) {
            if (objDbl->MaxPossibleIndex != NULL)
                maxVal = *(objDbl->MaxPossibleIndex);
        }

        if (maxVal == -1) {
            int objId = 0;
            hhh->Fill(&objId);
        } else {
            for (int objId = 0; objId < maxVal; objId++)
                hhh->Fill(&objId);
        }
    }
}
