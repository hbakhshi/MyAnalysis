/* 
 * File:   BaseAnalysis.h
 * Author: hbakhshi
 *
 * Created on November 25, 2010, 1:05 PM
 */

#ifndef BASEANALYSIS_H
#define	BASEANALYSIS_H

#include "base/Configuration.h"

template<class EVENTTYPE>
class Analyzer {
public:

    Analyzer(const edm::ParameterSet& ps) : Name(ps.getParameter<string>("Name")) {
    };

    virtual void Load() {
    };

    virtual void OnChangeFile(FileInfo* fi, DataSet* ds) {
    };

    ~Analyzer() {
        if (info::TheInfo->Verbosity > 100)
            cout << Name << " Ended" << endl;
    }

    virtual void OnChangeRun(std::map<string, int>* hltTable) {
        if (info::TheInfo->PrintTriggerTable) {
            cout << "HLT Table :" << endl;
            for (std::map<string, int>::const_iterator itr = hltTable->begin();
                    itr != hltTable->end(); itr++)
                cout << itr->first << "\t:\t" << itr->second << endl;
            cout << "----------------------------------" << endl;
        }
    };

    virtual void OnChangeDS(DataSet* ds) {
    };
    virtual bool Run(EVENTTYPE* ev) = 0;

    virtual void End() {
    };

    string Name;
};

#endif	/* BASEANALYSIS_H */

