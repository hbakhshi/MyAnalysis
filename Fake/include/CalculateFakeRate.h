/* 
 * File:   CalculateFakeRate.h
 * Author: hbakhshi
 *
 * Created on January 28, 2011, 6:34 PM
 */

#ifndef CALCULATEFAKERATE_H
#define	CALCULATEFAKERATE_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include <DataFormats/interface/TreeReaderEvent.h>
#include "BaseAlgorithm.h"

class CalculateFakeRate : public Analyzer<TreeReaderEvent> {
public:
    typedef Analyzer<TreeReaderEvent> base;

    CalculateFakeRate(const edm::ParameterSet& ps);
    ~CalculateFakeRate();

    virtual bool Run(TreeReaderEvent* ev);

    virtual void End();

    TH1F E_IsoL;
    TH1F E_IsoT;
    TH2F E_IsoID;

    edm::VParameterSet AllFRPS;
    vector<BaseAlgorithm*> allFR;
};

#endif	/* CALCULATEFAKERATE_H */

