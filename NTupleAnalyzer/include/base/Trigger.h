/* 
 * File:   Trigger.h
 * Author: hbakhshi
 *
 * Created on December 3, 2010, 1:41 PM
 */

#ifndef TRIGGER_H
#define	TRIGGER_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <exception>

#include "BaseAnalysis/interface/HistogramsManager.h"

using namespace std;

class TriggerException : public exception {
public:

    string __WHAT;

    TriggerException(string WHAT) : __WHAT(WHAT) {
    }

    virtual ~TriggerException() throw () {
    };

    virtual const char* what() const throw () {

        return __WHAT.c_str();
    }
};

class TriggerCutSingleRunRange {
public:
    int MinRun;
    int MaxRun;
    vector<string> HLTs;
    string Name;

    bool SkipRunsWithInconsistentMenu;

    void Print() const;
};

class TriggerCutFullRunRange {
public:
    vector<TriggerCutSingleRunRange*> Cuts;
    string Name;

    TriggerCutFullRunRange(const edm::VParameterSet& input, string name) ;

    void Print() const;

    const TriggerCutSingleRunRange* FindARun(int run) const;
};

class TriggerAnalyzer {
public:
    TriggerCutFullRunRange* TriggerInfo;
    int* CurrentRun;
    int* HLTResutls;

    string CurrentRunRangeName;
    bool CurrentRunRangeSkip;
    TH1I hAllDifferentRunRanges;
    TH1I hPassedDifferentRunRanges;

    TriggerAnalyzer(TriggerCutFullRunRange* trgInf);

    void SetPointers(int* currentRun, int* HLTRes);

    std::vector<int> CurrentBits;

    void OnRunChange(int run, std::map<string, int>* hltNames);

    bool Analyze(int run);

    void Write(TDirectory* dir);
};

class VTriggerCuts : public std::vector<TriggerCutFullRunRange> {
public:
    typedef std::vector<TriggerCutFullRunRange> BASE;
    std::vector<TriggerAnalyzer*> Analyzers;
    HistogramsManager<int> AllHistograms;

    VTriggerCuts();

    void SetHistosEnable(bool enable);

    void AddHistogram(ObjectProperty<int>* histo);

    void CreateAnalyzers();

    virtual void End(TDirectory* dir);

    void SetAnalyzerPointers(int* currentRun, int* HLTRes);

    void RunChanged(int run, std::map<string, int>* hltNames);

    map<string, bool> LastResults;

    void FillAllValue(Histograms<int>* histos);

    void RunAllAnalyzers(int run);

    void Print() const;
};

#endif	/* TRIGGER_H */

