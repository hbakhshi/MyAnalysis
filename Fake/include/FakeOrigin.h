/* 
 * File:   FakeOrigin.h
 * Author: hbakhshi
 *
 * Created on July 12, 2010, 3:23 PM
 */

#ifndef _FAKEORIGIN_H
#define	_FAKEORIGIN_H

#include "base/TreeReader.hh"
#include <map>
#include <iostream>
#include <sstream>
#include <limits>
#include "helper/pdgparticle.hh"
#include "TH2.h"
using namespace std;

inline vector<string> SplitString(string Str, const char* delim) {
    vector<string> ret;
    char str[Str.length()];
    //std::cout << "  " ;//Str << "  " << string(delim) << std::endl;
    strcpy(str, Str.c_str());
    char * pch = strtok(str, delim);
    while (pch != NULL) {
        ret.push_back(string(pch));
        pch = strtok(NULL, delim);
    }
    return ret;
}

class Cut {
public:
    string objectType;
    string propertyName;

    double minVal;
    double maxVal;

    TLeaf* leaf;
    string type;

    string LeafName() {
        return objectType + propertyName;
    }

    Cut() {
        leaf = NULL;

        maxVal = numeric_limits<int>::max();
        minVal = numeric_limits<int>::min();
    }
};

class CutInfo {
public:
    vector<Cut> Cuts;
    string Name;

    int NumberOfPassed;
    int NumberOfPassedBarrel;
    map<int, int> GMIDs;

    TreeReader* TheTree;

    CutInfo(string name, string cuts, TreeReader* theTree, string decay_name);
    void AddEle(int eleID);

    bool Pass(int id);

    void WriteAndPrint(TDirectory*);

    TH1I* hDRJet;
    TH1I* hJERatio;
    TH2I* hDRJet_E;
    TH1I* hJet_EMF;
    TH1I* hJet_NAssTrk;
    TH1I* hJet_NConsts;
    TH1I* hJet_ChrgFrc;
    TH1I* hJet_HCALSze;
    TH1I* hJet_ECALSze;

    TH1I* hDRGamma;
    TH2I* hDRGamma_E;
    TH1I* hGamma_HOE;

    TH1I* hEIso_Rel04;
    TH1I* hEIso_Trk04;
    TH1I* hEIso_ECl04;
    TH1I* hEIso_HCl04;
    TH1I* hEIso_D0;
    TH1I* hEIso_D0Res;

    TH1I* hEConv_Dis;
    TH1I* hEConv_DCot;
    TH1I* hEConv_NMisHit;
    TH2I* hEConv_DistDCot;

    TH1I* hEID_DPhiClus;
    TH1I* hEID_DEtaClus;
    TH1I* hEID_DPhiSuper;
    TH1I* hEID_DEtaSuper;
    TH1I* hEID_SIeIe;
    TH1I* hEID_EoverP;
    TH1I* hEID_E5E25;

    TH1I* hERatio_Gen;

    vector<TH1*> allHs;
};

class DecayChainInfo {
public:
    int PID;
    int MID;
    std::map<int, int> GMIDs;

    int TotalNumber;
    int TotalNumberBarrel;

    DecayChainInfo(int pid, int mid, TreeReader* theTree);
    void AddCut(string name, string cuts);
    bool AddEle(int eleID);

    void WriteAndPrint(TDirectory*);

    std::vector<CutInfo> Cuts;
    TreeReader* TheTree;


    static map<int, pdgparticle> fPDGMap;
    static void ReadPDGTable(const char* filename = "pdgtable.txt");
};

class FakeAnalyzer {
public:
    vector<DecayChainInfo> AllDecays;
    string Name;
    TreeReader* TheTree;

    enum EC_BAREL {
        EndCap,
        Barel,
        All
    };
    EC_BAREL EtaStatus;
    bool ConversionRejection;

    FakeAnalyzer(TreeReader*, string, double pt = -1, EC_BAREL etaStatus = All, bool ConvRej = false);
    void AddCut(string cut, string name);
    void Analyze();

    void End(TDirectory*);

    vector<int> OtherDecays;
    vector<int> OtherDecaysBarrel;
};
#endif	/* _FAKEORIGIN_H */
