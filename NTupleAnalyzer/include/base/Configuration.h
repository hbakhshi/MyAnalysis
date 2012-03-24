/* 
 * File:   Configuration.h
 * Author: hbakhshi
 *
 * Created on October 11, 2010, 6:31 PM
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/PythonProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Trigger.h"

using namespace std;

//usefull methods:
#include <string>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>

string GetStatusString(int stat);

std::vector <std::string>* read_directory(const std::string& path, std::vector <std::string>* result);

vector<string>* read_castor_directory(string castor_dir, vector<string>* ret);

vector<string>* read_srm_directory(string castor_dir, vector<string>* ret, string lcg_ls);

class FileInfo {
public:
    string FileName;
    double XSec;
    string Name;
    int MaxInput;
    double PreSelEfficiency;

    string TreeName;

    double GetWeight(double IntLumi) {
        if (XSec < 0)
            return 1;
        else
            return PreSelEfficiency * (IntLumi * XSec) / NEventsToAnalyze();
    }

    TFile* __File;

    virtual TFile * GetFile() {
        if (__File == NULL)
            __File = TFile::Open(this->FileName.c_str());

        return __File;
    }

    virtual void KillFile();

    TTree* __Tree;
    int __NEvents;
    TTree* __RunTree;

    virtual TTree* GetRunTree() {
        return __RunTree;
    }

    virtual TTree * GetTree() {
        if (__Tree == NULL) {
            __Tree = (TTree*) (GetFile()->Get(TreeName.c_str()));
            __RunTree = (TTree*) GetFile()->Get("analyze/RunInfo");
            __NEvents = __Tree->GetEntries();
        }

        return __Tree;
    }

    int NEventsToAnalyze() {
        if (__NEvents == -1)
            GetTree();

        if (MaxInput == -1)
            return __NEvents;
        else
            MaxInput;
    }

    int NReadEvents() const {
        return NAnalyzed + NAllRejecteds;
    }

    long int NEvents() {
        if (__NEvents == -1)
            GetTree();

        return __NEvents;
    }

    FileInfo(string treeName = "analyze/Analysis") : __File(NULL), __Tree(NULL), TreeName(treeName),
    NAnalyzed(0), NAllRejecteds(0), __NEvents(-1) {
    }

    void Print(string procName = "") const {
        cout.precision(2);
        cout << fixed << "|" << procName << "|" << Name << "|" << XSec << "|" << NReadEvents() << "|" << NAnalyzed << "|" << NAllRejecteds << "|" << endl;
        for (std::map<string, double>::const_iterator trg_stat = NTriggerRejections.begin();
                trg_stat != NTriggerRejections.end(); trg_stat++) {
            cout << "||" << trg_stat->first << "|" << trg_stat->second << "|" << endl;
        }
        for (std::map<int, double>::const_iterator rej_itr = NRejecteds.begin();
                rej_itr != NRejecteds.end(); rej_itr++) {
            cout << "|" << rej_itr->first << "|" << GetStatusString(rej_itr->first) << "|" << rej_itr->second << "|" << endl;
        }
        for (std::map<string, double>::const_iterator anz_itr = NRejectedsAnalyzers.begin();
                anz_itr != NRejectedsAnalyzers.end(); anz_itr++) {
            cout << "||" << anz_itr->first << "|" << NAnalyzedAnalyzers.at(anz_itr->first) << "|" << anz_itr->second << "|" << endl;
        }
        cout << endl;
    }

    //variables to hold information
    int NAnalyzed;
    //Rejections due to standard event selectino cuts in NTuple2Event class:
    std::map<int, double> NRejecteds;
    int NAllRejecteds;

    double AddRejected(int reason, double w) {
        NAllRejecteds += w;

        std::map<int, double>::iterator itr = NRejecteds.find(reason);
        if (itr != NRejecteds.end())
            return (itr->second += w);
        else
            return (NRejecteds[reason] = w);
    };
    //Rejections in analyzers
    std::map<string, double> NRejectedsAnalyzers;
    std::map<string, double> NAnalyzedAnalyzers;

    void AddAnalyzeResult(string analyzerName, double w, bool runValue) {
        std::map<string, double>::iterator itr_rej = NRejectedsAnalyzers.find(analyzerName);
        std::map<string, double>::iterator itr_anz = NAnalyzedAnalyzers.find(analyzerName);

        if (runValue) {
            if (itr_anz != NAnalyzedAnalyzers.end())
                itr_anz->second += w;
            else
                NAnalyzedAnalyzers[analyzerName] = w;

            if (itr_rej == NRejectedsAnalyzers.end())
                NRejectedsAnalyzers[analyzerName] = 0;
        } else {
            if (itr_rej != NRejectedsAnalyzers.end())
                itr_rej->second += w;
            else
                NRejectedsAnalyzers[analyzerName] = w;

            if (itr_anz == NAnalyzedAnalyzers.end())
                NAnalyzedAnalyzers[analyzerName] = 0;
        }
    };
    std::map<string, double> NTriggerRejections;
};

#include <boost/regex.hpp>

class DirectoryInfo : public FileInfo {
public:
    bool isCastor;
    int TotalNumberOfEvents;
    std::vector< string > Files;

    boost::regex fileNamePattern;

    DirectoryInfo(string treeName = "analyze/Analysis") : FileInfo(treeName) {
    };

    virtual TFile * GetFile() {
        if (Files.size() == 0) {
            if (isCastor)
                read_castor_directory(this->FileName, &Files);
            else
                read_directory(this->FileName, &Files);
        }

        return NULL;
    }

    virtual TTree * GetTree() {
        if (__Tree == NULL) {
            GetFile();
            __Tree = new TChain(this->TreeName.c_str());
            __RunTree = new TChain("analyze/RunInfo");

            for (vector<string>::iterator file = Files.begin();
                    file != Files.end(); file++) {
                if(fileNamePattern.str() != "")
                    if( ! boost::regex_match( *file , fileNamePattern  ) )
                        continue;
                    else
                        cout << *file << endl;
                
                TFile* f_tmp = TFile::Open((*file).c_str(), "READ");
                if (f_tmp != NULL) {
                    if (!(f_tmp->IsZombie()) && !(f_tmp->TestBit(TFile::kRecovered))) {
                        ((TChain*) __Tree)->AddFile((*file).c_str(), TotalNumberOfEvents < 0 ? 0 : TChain::kBigNumber);
                        ((TChain*) __RunTree)->AddFile((*file).c_str(), TotalNumberOfEvents < 0 ? 0 : TChain::kBigNumber);
                    }

                    f_tmp->Close();
                    delete f_tmp;
                }
            }

            this->__NEvents = TotalNumberOfEvents < 0 ? __Tree->GetEntries() : TotalNumberOfEvents;
        }
        return __Tree;
    }

    virtual void KillFile() {
    }
};

class DCapDir : public DirectoryInfo {
public:
    string Server;
    string LCG_LS;
    int DCapPort;
    int SRMPort;

    typedef DirectoryInfo BASE_;

    DCapDir(string treeName = "analyze/Analysis") : BASE_(treeName) {
    };

    string GetSRMFullAddress(string fileName = "") {
        std::stringstream ret;
        ret << "srm://" << Server << ":" << SRMPort;
        ret << "/srm/managerv2?SFN=";
        ret << BASE_::FileName << "/" << fileName;

        return ret.str();
    }

    string GetDCapFullAddress(string fileName) {
        std::stringstream ret;
        ret << "dcap://" << Server << ":" << DCapPort;
        ret << fileName;

        return ret.str();
    }

    virtual TFile * GetFile() {
        if (BASE_::Files.size() == 0) {
            read_srm_directory(GetSRMFullAddress(), &Files, LCG_LS);
        }

        return NULL;
    }

    virtual TTree * GetTree() {
        if (__Tree == NULL) {
            GetFile();
            __Tree = new TChain(this->TreeName.c_str());
            __RunTree = new TChain("analyze/RunInfo");

            for (vector<string>::iterator file = Files.begin();
                    file != Files.end(); file++) {
                ((TChain*) __Tree)->AddFile(GetDCapFullAddress(*file).c_str(), TotalNumberOfEvents < 0 ? 0 : TChain::kBigNumber);
                ((TChain*) __RunTree)->AddFile(GetDCapFullAddress(*file).c_str(), TotalNumberOfEvents < 0 ? 0 : TChain::kBigNumber);
            }

            this->__NEvents = TotalNumberOfEvents < 0 ? __Tree->GetEntries() : TotalNumberOfEvents;
        }
        return __Tree;
    }

    virtual void KillFile() {
    }
};

class DataSet : public std::vector<FileInfo*> {
public:
    string ProcessName;

    typedef std::vector<FileInfo*> BASE;

    DataSet(const edm::VParameterSet& input, string name) : ProcessName(name) {
        for (edm::VParameterSet::const_iterator itr = input.begin();
                itr != input.end(); itr++) {

            FileInfo* fi;

            cout << itr->getParameter<string > ("Name") << endl;
            if (itr->getUntrackedParameter<bool>("IsDirectory", false)) {
                fi = new DirectoryInfo();
                ((DirectoryInfo*) fi)->isCastor = itr->getParameter<bool>("IsCastor");
                ((DirectoryInfo*) fi)->fileNamePattern.assign( itr->getUntrackedParameter<string>("fileNamePattern", "").c_str() );
                ((DirectoryInfo*) fi)->TotalNumberOfEvents = itr->getParameter<int>("TotalNumberOfEvents");
            } else if (itr->getUntrackedParameter<bool>("IsDCap", false)) {
                fi = new DCapDir();
                ((DCapDir*) fi)->DCapPort = itr->getParameter<int>("DCapPort");
                ((DCapDir*) fi)->SRMPort = itr->getParameter<int>("SRMPort");
                ((DCapDir*) fi)->Server = itr->getParameter<string > ("Server");
                ((DCapDir*) fi)->LCG_LS = itr->getParameter<string > ("LCG_LS");
                ((DCapDir*) fi)->TotalNumberOfEvents = itr->getParameter<int>("TotalNumberOfEvents");
            } else
                fi = new FileInfo();

            fi->FileName = itr->getParameter<string > ("File");
            fi->XSec = itr->getParameter<double > ("XSec");
            fi->Name = itr->getParameter<string > ("Name");
            fi->MaxInput = itr->getParameter<int > ("MaxInput");
            fi->PreSelEfficiency = itr->getParameter<double > ("PreSelEfficiency");

            BASE::push_back(fi);
        }
    }

    int Total() const {
        int ret = 0;
        for (int i = 0; i < BASE::size(); i++) {
            ret += BASE::at(i)->NReadEvents();
        }
        return ret;
    };

    int NAnal() const {
        int ret = 0;
        for (int i = 0; i < BASE::size(); i++) {
            ret += BASE::at(i)->NAnalyzed;
        }
        return ret;
    };

    int NRej() const {
        int ret = 0;
        for (int i = 0; i < BASE::size(); i++) {
            ret += BASE::at(i)->NAllRejecteds;
        }
        return ret;
    };

    double XSec() const {
        double ret = 0;
        for (int i = 0; i < BASE::size(); i++) {
            ret += BASE::at(i)->XSec;
        }
        return ret;
    };

    void Print() const {
        if (BASE::size() == 1) {
            const FileInfo* fi = (BASE::at(0));
            cout.precision(2);
            fi->Print(this->ProcessName);
        } else if (BASE::size() > 1) {
            cout.precision(2);
            cout << fixed << "|" << this->ProcessName << "||" << XSec() << "|" << Total() << "|" << NAnal() << "|" << NRej() << "|" << endl;
            for (int j = 0; j < BASE::size(); j++) {
                const FileInfo* fi = (BASE::at(j));
                fi->Print();
            }
        }
    }
};

class VDataSets : public vector<DataSet> {
public:
    typedef vector<DataSet> BASE;

    VDataSets() {

    }

    void Print() {
        cout << "|" << "Name" << "|_FileName_|" << "XSec" << "|" << "Total" << "|" << "NAnal" << "|" << "NRej" << "|" << endl;
        for (int j = 0; j < BASE::size(); j++) {
            DataSet* ds = &(BASE::at(j));
            ds->Print();
        }
    }
};

class RunInfo {
public:
    int Verbosity;
    string Name;
    double IntLumi;
    int NumberOfEvents;
    VDataSets dataSets;
    int NumberOfEventsPerPrintLoop;
    string outfilename;
    TFile* OutFile;

    bool CopyTree;
    string CopyTreeFileName;

    bool PrintTriggerTable;
    VTriggerCuts triggerCuts;

    edm::ParameterSet SelectorConfigs;
    //ElectronSelection ElectronSelectionCuts;

    //EventSelection EventSelectionCuts;

    edm::VParameterSet Analyzers;

    RunInfo() {

        NumberOfEvents = -1;
        Name = "";
        IntLumi = 0.0;
        outfilename = "";
        Verbosity = 0;
    }

    virtual ~RunInfo() {
        if (Verbosity > 100)
            cout << "RunInfo Ended" << endl;
    };

    void Print() {
        cout << Name << "\t" << IntLumi << "\t" << NumberOfEvents << "\t" << outfilename << endl;

        dataSets.Print();
        //triggerCuts.Print();
        //ElectronSelectionCuts.Print();
        //EventSelectionCuts.Print();
    }
};

class info {
public:
    static RunInfo* TheInfo;
    static int CPU_Number;
    static void AnalyzeArgs(int argc, char** argv, std::string module_name = "ElectronCharge");
};
#endif	/* CONFIGURATION_H */

