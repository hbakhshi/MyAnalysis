/* 
 * File:   GenRecComparison.h
 * Author: hbakhshi
 *
 * Created on February 8, 2012, 3:44 PM
 */

#ifndef GENRECCOMPARISON_H
#define	GENRECCOMPARISON_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "TRandom.h"
#include "BaseAnalysis/interface/HistogramsManager.h"

class GenRecComparison : public Analyzer<TopAnalysis::TTBarDileptonicEvent> {
public:
    typedef Analyzer<TopAnalysis::TTBarDileptonicEvent> BASE;

    Histograms< TopAnalysis::TTBarDileptonicEvent > top_histos;
    Histograms< TopAnalysis::TTBarDileptonicEvent > antitop_histos;
    Histograms< TopAnalysis::TTBarDileptonicEvent > event_histos;

    Histograms< TopAnalysis::TTBarDileptonicEvent > gen_info;
    
    std::vector<double> AcceptedEventTypes;
    TopAnalysis::DiLeptonTTBarEventProperties::EventType EventTypeReader;

    GenRecComparison(const edm::ParameterSet& ps);
    virtual void End();

    virtual ~GenRecComparison();

    bool Run(TopAnalysis::TTBarDileptonicEvent* ev);

    string solverName ;
    TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions solverSolution;
private:

};

#endif	/* GENRECCOMPARISON_H */

