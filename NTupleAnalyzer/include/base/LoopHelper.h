/* 
 * File:   LoopHelper.h
 * Author: hbakhshi
 *
 * Created on January 1, 2009, 4:43 PM
 */



#ifndef _LOOPHELPER_H
#define	_LOOPHELPER_H


//System
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sys/stat.h> //to check local files


#include <TTree.h>
#include <TFile.h>
#include "base/TreeReader.hh"
#include "../helper/Utilities.hh"
#include <string>

#include "Configuration.h"
#include "Analyzer.h"

using namespace std;

class LoopExceptions : public exception {
public:

    string __WHAT;

    LoopExceptions(string WHAT) : __WHAT(WHAT) {
    }

    virtual ~LoopExceptions() throw () {
    };

    virtual const char* what() const throw () {

        return __WHAT.c_str();
    }
};

class LoopHelper {
public:

    LoopHelper() : AllEvents(-1), fTR(NULL), CurrentRunNumber(-1000) {
        Util::SetStyle();
    };

    ~LoopHelper() {
        for(analyzers::reverse_iterator analyzer = Analyzers.rbegin() ; analyzer != Analyzers.rend() ; analyzer++){
            cout << (*analyzer)->Name << " is being killed .... " ;
            delete *(analyzer);
        }
        if(info::TheInfo->Verbosity > 100)
            cout << "LoopHelper Ended" << endl;
    };

    void Start(RunInfo) throw (LoopExceptions);

    typedef std::vector<Analyzer<EventSelector::EVENTTYPE>*> analyzers;
    analyzers Analyzers;
    //a place for the number of events which are going to be analyzed from the current file.
    int AllEvents;
    //the total number of analyzed events so far
    int Total;
    TreeReader *fTR;
    EventSelector* DiElectronEventReader;

    int CurrentRunNumber;
    std::map<string, int> CurrentRunHLTLabelMap;

    void GetHLTNames() {
        Int_t run = fTR->Run;
        if (CurrentRunNumber != run) {
            CurrentRunNumber = run;

            TFile *f = fTR->fChain->GetCurrentFile();
            TTree* runTree = (TTree*) f->Get("analyze/RunInfo");
            std::vector<std::string>* HLTNames = new std::vector<std::string>();
            int RunHolder(-100);
            if (!runTree) {
                std::cerr << "!!! LoopHelper::GetHLTNames "
                        << "Coudln't get analyze/RunInfo tree" << std::endl;
                return;
            }

            if (info::TheInfo->Verbosity > 0) std::cout << "Retrieving HLTNames for run " << run << std::endl;
            runTree->SetBranchAddress("HLTNames", (&HLTNames));
            runTree->SetBranchAddress("Run", &RunHolder);
            //runTree->Print("ALL");
            for (int entry = 0; entry < runTree->GetEntries(); entry++) {
                //cout << entry << "  " << runTree->GetEntries() << endl;
                runTree->GetEntry(entry);
                if (RunHolder == run)
                    break;
            }

            for (int i = 0; i < HLTNames->size(); i++) {
                CurrentRunHLTLabelMap[(*HLTNames)[i]] = i;

                if (info::TheInfo->Verbosity > 1) cout << (*HLTNames)[i] << " - " << i << endl;
            }
        }
    }

    //    int GetHLTBit(string theHltName) {
    //        if (CurrentRunHLTLabelMap.empty()) return -1;
    //        else {
    //            map<string, int>::iterator it = CurrentRunHLTLabelMap.find(theHltName);
    //            if (it == CurrentRunHLTLabelMap.end()) {
    //                if (info::TheInfo->Verbosity > 1) cout << "UserAnalysisBase::GetHLTBit ==> Bit with name " << theHltName << " not found!" << endl;
    //                return -1;
    //            } else {
    //                return it->second;
    //            }
    //        }
    //    }
    //
    //    bool GetHLTResult(string theHltName) {
    //        if (CurrentRunHLTLabelMap.empty()) return false;
    //        else {
    //            int bit = GetHLTBit(theHltName);
    //            if (bit == -1) {
    //                if (info::TheInfo->Verbosity > 1) cout << "UserAnalysisBase::GetHLTResult ==> Bit with name " << theHltName << " not found!" << endl;
    //                return false;
    //            } else return (bool)fTR->HLTResults[bit];
    //        }
    //    }

private:

    void SetNTuple2Event();
};

void LoopHelper::SetNTuple2Event() {
    SelectorConfig* config = new SelectorConfig(info::TheInfo->SelectorConfigs);
    DiElectronEventReader = new EventSelector(NULL, config, &(info::TheInfo->triggerCuts.LastResults));
};

void LoopHelper::Start(RunInfo info) throw (LoopExceptions) {

    //TFile* f;

    using namespace std;



    Total = 0;

    TFile* file_tree = NULL;
    TTree* clone_tree = NULL;
    TList clone_run_trees;
    SetNTuple2Event();

    for (VDataSets::size_type ds_idx = 0; ds_idx < info::TheInfo->dataSets.size(); ds_idx++) {
        DataSet* ds_itr = &(info::TheInfo->dataSets.at(ds_idx));

        for (LoopHelper::analyzers::iterator analze = this->Analyzers.begin();
                analze != this->Analyzers.end(); analze++) {
            (*analze)->OnChangeDS(ds_itr);
        }
        for (std::vector<FileInfo>::size_type file_idx = 0; file_idx < ds_itr->size(); file_idx++) {

            FileInfo* file_itr = (ds_itr->at(file_idx));

            int TotalEventsInFile = 0;
            double weight = file_itr->GetWeight(info::TheInfo->IntLumi);

            long int nEntries = file_itr->NEvents();
            this->AllEvents = file_itr->MaxInput == -1 ? nEntries : file_itr->MaxInput;

            cout << file_itr->FileName << " is started : " << AllEvents << "(From:" << nEntries << ")" << " are going to be analyzed" << ". The weight is " << weight << endl;

            bool isFirstTimeThatReaderCrated = false;
            if (fTR == NULL) {
                fTR = new TreeReader(file_itr->GetTree());
                isFirstTimeThatReaderCrated = true;
                info::TheInfo->triggerCuts.SetAnalyzerPointers(&(fTR->Run), fTR->HLTResults);
                DiElectronEventReader->SetTree(fTR);
            } else {
                fTR->Init(file_itr->GetTree());
                cout << "The tree is initialized again" << endl;
            }

            if (info::TheInfo->CopyTree && clone_tree == NULL) {
                string file_tree_name = info::TheInfo->CopyTreeFileName;
                file_tree = new TFile(file_tree_name.c_str(), "RECREATE");
                file_tree->mkdir("analyze")->cd();
                clone_tree = fTR->fChain->CloneTree(0);
            }
            if (info::TheInfo->CopyTree) {
                TTree* runTree = file_itr->GetRunTree();
                file_tree->GetDirectory("analyze")->cd();
                clone_run_trees.Add(runTree->CloneTree(-1, "fast"));
            }



            if (isFirstTimeThatReaderCrated)
                DiElectronEventReader->AddTriggerHistos();

            DiElectronEventReader->SetEventProperties(weight, ds_itr->ProcessName == "Z");
            DiElectronEventReader->OnChangeFile(&(*file_itr), &(*ds_itr));

            for (LoopHelper::analyzers::iterator analze = this->Analyzers.begin();
                    analze != this->Analyzers.end(); analze++) {
                (*analze)->OnChangeFile(&(*file_itr), &(*ds_itr));
            }

            long int counter = 0; //to control the display output
            for (long int index = 0;
                    index < nEntries && (file_itr->MaxInput == -1 ? true : TotalEventsInFile < file_itr->MaxInput) && (info::TheInfo->NumberOfEvents == -1 ? true : Total < info::TheInfo->NumberOfEvents);
                    index++, TotalEventsInFile++, Total++, counter++) {

                fTR->GetEntry(index);

                if (CurrentRunNumber != fTR->Run) {
                    if (info::TheInfo->triggerCuts.size() > 0) {
                        GetHLTNames();
                        info::TheInfo->triggerCuts.RunChanged(CurrentRunNumber, &CurrentRunHLTLabelMap);
                    }

                    for (LoopHelper::analyzers::iterator analze = this->Analyzers.begin();
                            analze != this->Analyzers.end(); analze++) {
                        (*analze)->OnChangeRun(& (this->CurrentRunHLTLabelMap));
                    }
                }

                bool trigger = true;
                info::TheInfo->triggerCuts.RunAllAnalyzers(CurrentRunNumber);
                for (map < string, bool>::const_iterator trg_name = info::TheInfo->triggerCuts.LastResults.begin();
                        trg_name != info::TheInfo->triggerCuts.LastResults.end(); trg_name++) {

                    bool pass = trg_name->second;

                    if (!pass) {
                        trigger = false;
                        map<string, double>::iterator trg_stat = file_itr->NTriggerRejections.find(trg_name->first);
                        if (trg_stat != file_itr->NTriggerRejections.end())
                            trg_stat->second += weight;
                        else
                            file_itr->NTriggerRejections[trg_name->first] = weight;
                    }
                }

                if (trigger || true) {
                    int stat = 0;
                    EventSelector::EVENTTYPE* evt = DiElectronEventReader->Read(stat);
                    if (evt != NULL) {
                        file_itr->NAnalyzed += weight;

                        if (info::TheInfo->Verbosity == 100)
                            cout << index << " : " << fTR->Event << endl;

                        for (LoopHelper::analyzers::iterator analze = this->Analyzers.begin();
                                analze != this->Analyzers.end(); analze++) {
                            file_itr->AddAnalyzeResult((*analze)->Name, weight, (*analze)->Run(evt));
                        }
                        if (clone_tree)
                            clone_tree->Fill();

                        file_itr->AddRejected(stat, weight);
                    } else
                        file_itr->AddRejected(stat, weight);

                }
                if ((index == nEntries - 1) || counter == info::TheInfo->NumberOfEventsPerPrintLoop) {

                    counter = 0;

                    std::cout << "\r\tEvent : " << index + 1 << " (In the file :" << file_itr->FileName << ")";
                    std::cout.flush();

                    if (clone_tree) {
                        clone_tree->AutoSave();
                    }
                }
            }

            std::cout << endl;

            //            cout << "ended" << endl;
            //file_itr->KillFile();
            //            cout << "file killed" << endl;
            if (fTR != NULL) {
                //                cout << "it's not null" << endl;
                //                delete fTR;
            }
        }
    }

    //    if (clone_tree) {
    //        file_tree->Close();
    //    }

    info::TheInfo->OutFile = TFile::Open(info::TheInfo->outfilename.c_str(), "RECREATE");
    DiElectronEventReader->End(info::TheInfo->OutFile);
    info::TheInfo->triggerCuts.End(info::TheInfo->OutFile);

    for (LoopHelper::analyzers::iterator analze = this->Analyzers.begin();
            analze != this->Analyzers.end(); analze++) {
        (*analze)->End();
    }
    info::TheInfo->OutFile->Close();
    if (info::TheInfo->CopyTree) {
        file_tree->GetDirectory("analyze")->cd();
        TTree* tAllRuns = (TTree*) clone_run_trees.At(0); // TTree::MergeTrees(&clone_run_trees);
        tAllRuns->AutoSave();
    }

    if(file_tree != NULL)
        file_tree->Close();
    
    cout << "\nNumber of total events anlyzed : " << Total << endl;
}

#endif	/* _LOOPHELPER_H */
