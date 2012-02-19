/* 
 * File:   SameFlavourDiLeptonFakeEstimator.h
 * Author: hbakhshi
 *
 * Created on February 25, 2011, 7:58 PM
 */

#ifndef SAMEFLAVOURDILEPTONFAKEESTIMATOR_H
#define	SAMEFLAVOURDILEPTONFAKEESTIMATOR_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include <DataFormats/interface/TreeReaderEvent.h>

#include "BaseAnalysis/interface/EfficiencyHandlerReader.h"
#include "FPRatios.h"
#include "BaseAlgorithm.h"

class SameFlavourDiLeptonFakeEstimator : public Analyzer<TreeReaderEvent> {
public:
    typedef Analyzer<TreeReaderEvent> base;

    SameFlavourDiLeptonFakeEstimator(const edm::ParameterSet& ps);
    virtual ~SameFlavourDiLeptonFakeEstimator();

    virtual bool Run(TreeReaderEvent* ev);
    virtual void End();

    double TotalFR;
    double TotalFR_Err;
    FPRatios LucFakeWeightEstimator;
private:
    TH2D* hFakeRate;
    bool disabled;

    bool isElectron;
    string TightTag;
    string LooseTag;

    string FakeRateDirectoryName;
    string FakeRateFileName;

    bool binnedPt;
    bool AbsEta;
    bool EndcapBarrel;

    double TotalFP;
    double TotalFF;
    double TotalPP;

    double TotalT0;
    double TotalT1;
    double TotalT2;
};

#endif	/* SAMEFLAVOURDILEPTONFAKEESTIMATOR_H */

