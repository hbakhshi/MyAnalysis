/* 
 * File:   CosThetaAnalysis.h
 * Author: hbakhshi
 *
 * Created on February 1, 2012, 3:31 PM
 */

#ifndef COSTHETAANALYSIS_H
#define	COSTHETAANALYSIS_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "TH1.h"

class CosThetaAnalysis : public Analyzer<TopAnalysis::TTBarDileptonicEvent> {
public:
    typedef Analyzer<TopAnalysis::TTBarDileptonicEvent> BASE;

    TopAnalysis::DiLeptonTTBarEventProperties::EventType EventTypeReader;
    std::vector<double> AcceptedEventTypes;
    
    string solverName;
    TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions solverSolution;
    
    TH1* hCosThetaPosLepton;
    TH1* hCosThetaNegLepton;
    TH1* hCosTheta1stLepton;
    TH1* hCosTheta2ndLepton;
    
    CosThetaAnalysis(const edm::ParameterSet& ps);
    virtual ~CosThetaAnalysis();
    
    bool Run(TopAnalysis::TTBarDileptonicEvent* ev);

    virtual void End();
private:

};

#endif	/* COSTHETAANALYSIS_H */

