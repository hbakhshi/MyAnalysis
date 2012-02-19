/* 
 * File:   NeutrinoSolver.h
 * Author: hbakhshi
 *
 * Created on January 31, 2012, 11:01 AM
 */

#ifndef NEUTRINOSOLVER_H
#define	NEUTRINOSOLVER_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include "DataFormats/interface/DiElectronEventProperties.h"
#include "TRandom.h"
#include "BJetAssigner.h"
#include "DileptonAnalyticalSolver.h"
#include "DataFormats/interface/DiLeptonEventProperties.h"
#include "TH1.h"

class NeutrinoSolver : public Analyzer<TopAnalysis::TTBarDileptonicEvent> {
public:
    typedef Analyzer<TopAnalysis::TTBarDileptonicEvent> BASE;

    BJetAssigner bjetAssigner;
    llsolver::DileptonAnalyticalSolver theSolver;

    TRandom random_seed;
    TRandom random_w1;
    TRandom random_w2;
    TRandom random_t1;
    TRandom random_t2;
    bool SwapBJetsIfNoSolution;
    int MaxLoopNumberToSolve;

    double top_mass, top_width, w_mass, w_width;
    bool FixMasses;


    TH1* hNSolutions;
    TH1* hNLoopsToSolve;

    TH1* hW1Mass_Selected;
    TH1* hW2Mass_Selected;
    TH1* hT1Mass_Selected;
    TH1* hT2Mass_Selected;
    TH1* hEffMass_Selected;
    
    TH1* hW1Mass_Wrongs;
    TH1* hW2Mass_Wrongs;
    TH1* hT1Mass_Wrongs;
    TH1* hT2Mass_Wrongs;
    TH1* hEffMass_Wrongs;

    TopAnalysis::DiLeptonTTBarEventProperties::EventType EventTypeReader;
    std::vector<double> AcceptedEventTypes;

    NeutrinoSolver(const edm::ParameterSet& ps);

    virtual ~NeutrinoSolver();

    bool Run(TopAnalysis::TTBarDileptonicEvent* ev);

    virtual void End();
private:
    double ETMiss[2];
    double BPt[4];
    double BBarPt[4];
    double LPPt[4];
    double LMPt[4];

    vector<double> pnux;
    vector<double> pnuy;
    vector<double> pnuz;
    vector<double> pnubx;
    vector<double> pnuby;
    vector<double> pnubz;
    vector<double> cd_diff;
    int cubic_single_root_cmplx;
};

#endif	/* NEUTRINOSOLVER_H */

