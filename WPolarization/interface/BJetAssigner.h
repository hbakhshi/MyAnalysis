/* 
 * File:   BJetAssigner.h
 * Author: hbakhshi
 *
 * Created on January 30, 2012, 7:34 PM
 */

#ifndef BJETASSIGNER_H
#define	BJETASSIGNER_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include "DataFormats/interface/DiElectronEventProperties.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "TRandom.h"

class BJetAssigner : public Analyzer<TopAnalysis::TTBarDileptonicEvent> {
public:
    enum methods{
        method_random,
        method_genmatch,
        method_mlb,
        method_deltaR,
        method_deltaPhi,
        method_deltaEta
    } Method;
    string method_name;
    //random tools and configs
    TRandom random_generator;
    //
    int the_b_index;
    int the_bbar_index;
            
    typedef Analyzer<TopAnalysis::TTBarDileptonicEvent> BASE;

    BJetAssigner(const edm::ParameterSet& ps);
    virtual ~BJetAssigner();

    bool Run(TopAnalysis::TTBarDileptonicEvent* ev);
private:

};

#endif	/* BJETASSIGNER_H */

