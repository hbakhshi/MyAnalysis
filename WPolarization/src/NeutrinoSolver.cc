/* 
 * File:   NeutrinoSolver.cc
 * Author: hbakhshi
 * 
 * Created on January 31, 2012, 11:01 AM
 */

#include "WPolarization/interface/NeutrinoSolver.h"

NeutrinoSolver::NeutrinoSolver(const edm::ParameterSet& ps) : BASE(ps),
random_seed(ps.getUntrackedParameter<int>("RandomSeed", 885147)),
random_w1(random_seed.Integer(150000000)),
random_w2(random_seed.Integer(150000000)),
random_t1(random_seed.Integer(150000000)),
random_t2(random_seed.Integer(150000000)),
bjetAssigner(ps.getParameterSet("bJetAssigner")),
top_mass(172.9), top_width(1.5), w_mass(80.398), w_width(2.08),
FixMasses(ps.getUntrackedParameter<bool>("FixMasses", false)),
SwapBJetsIfNoSolution(ps.getUntrackedParameter<bool>("SwapBJetsIfNoSolution", true)),
MaxLoopNumberToSolve(ps.getUntrackedParameter<int>("MaxLoopNumberToSolve", 1000)),
AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")),
allHistograms("MinEffectiveMassResults") {

    if (FixMasses)
        MaxLoopNumberToSolve = 0;

    int maxLoop = (SwapBJetsIfNoSolution ? 2 : 1) * (MaxLoopNumberToSolve + 1);
    this->hNLoopsToSolve = new TH1D("hNLoopsToSolve", "Number of loops to solve", maxLoop, 0, maxLoop);

    this->hNSolutions = new TH1D("hNSolutions", "Number of solutions", 10, 0, 10);

    this->hW1Mass_Selected = new TH1D("hW1Mass_Selected", "W1Mass for selected answer", 250, 0., 250.);
    this->hW2Mass_Selected = new TH1D("hW2Mass_Selected", "W2Mass for selected answer", 250, 0., 250.);
    this->hT1Mass_Selected = new TH1D("hT1Mass_Selected", "T1Mass for selected answer", 500, 0., 500.);
    this->hT2Mass_Selected = new TH1D("hT2Mass_Selected", "T2Mass for selected answer", 500, 0., 500.);
    this->hEffMass_Selected = new TH1D("hEffMass_Selected", "Effective mass for the selected answer", 1000, 0., 1000.);

    this->hW1Mass_Wrongs = new TH1D("hW1Mass_Wrong", "W1Mass for Wrong answers", 250, 0., 250.);
    this->hW2Mass_Wrongs = new TH1D("hW2Mass_Wrong", "W2Mass for Wrong answers", 250, 0., 250.);
    this->hT1Mass_Wrongs = new TH1D("hT1Mass_Wrong", "T1Mass for Wrong answers", 500, 0., 500.);
    this->hT2Mass_Wrongs = new TH1D("hT2Mass_Wrong", "T2Mass for Wrong answers", 500, 0., 500.);
    this->hEffMass_Wrongs = new TH1D("hEffMass_Wrong", "Effective mass for the Wrong answers", 1000, 0., 1000.);

    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 1, 1, 1 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 1, 1, 2 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 1, 2, 1 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 1, 2, 2 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 1, 3, 1 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 1, 3, 2 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 2, 1, 1 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 2, 1, 2 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 2, 2, 1 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 2, 2, 2 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 2, 3, 1 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::TopRecRestuls < 2, 3, 2 > >();
    allHistograms.AddHisto1< TopAnalysis::DiLeptonTTBarEventProperties::EffectiveMass<false> >();
}

NeutrinoSolver::~NeutrinoSolver() {
}

bool NeutrinoSolver::Run(TopAnalysis::TTBarDileptonicEvent* ev) {
    if (!bjetAssigner.Run(ev)) {
        this->hNLoopsToSolve->Fill(-1);
        this->hNSolutions->Fill(-1);
        //cout << "it is running" << endl;

        return false;
    }

    double evtType = EventTypeReader.ReadValue(ev);
    if (std::find(AcceptedEventTypes.begin(), AcceptedEventTypes.end(), evtType) == AcceptedEventTypes.end()) {
        this->hNLoopsToSolve->Fill(-2);
        this->hNSolutions->Fill(-2);
        //cout << "it is running" << endl;

        return false;
    }
    //cout << "it is running" << endl;

    TopAnalysis::TTBarDileptonicEvent::SolverResults* results = ev->AddSolverResults(this->Name, bjetAssigner.the_b_index, bjetAssigner.the_bbar_index);


    this->pnux.clear();
    this->pnuy.clear();
    this->pnuz.clear();
    this->pnubx.clear();
    this->pnuby.clear();
    this->pnubz.clear();
    this->cd_diff.clear();
    cubic_single_root_cmplx = -1;

    int counter = 0;
    if (this->FixMasses) {
        this->theSolver.solve(results->ETMiss, results->BPt, results->BBarPt, results->LPPt, results->LMPt, this->w_mass, this->w_mass, this->top_mass, this->top_mass, 0.0, 0.0,
                &this->pnux, &this->pnuy, &this->pnuz, &this->pnubx, &this->pnuby, &this->pnubz, &cd_diff, cubic_single_root_cmplx);
    } else {
        double W1Mass, W2Mass, T1Mass, T2Mass;
        bool Solved(false);
        for (; counter < MaxLoopNumberToSolve; counter++) {
            W1Mass = random_w1.BreitWigner(w_mass, w_width);
            W2Mass = random_w2.BreitWigner(w_mass, w_width);
            T1Mass = random_t1.BreitWigner(top_mass, top_width);
            T2Mass = random_t2.BreitWigner(top_mass, top_width);

            this->theSolver.solve(results->ETMiss, results->BPt, results->BBarPt, results->LPPt, results->LMPt, W1Mass, W2Mass, T1Mass, T2Mass, 0.0, 0.0,
                    &this->pnux, &this->pnuy, &this->pnuz, &this->pnubx, &this->pnuby, &this->pnubz, &cd_diff, cubic_single_root_cmplx);

            if (pnux.size() > 0) {
                Solved = true;
                break;
            }
        }
        if (!Solved && SwapBJetsIfNoSolution) {
            for (; counter < 2 * MaxLoopNumberToSolve; counter++) {
                W1Mass = random_w1.BreitWigner(w_mass, w_width);
                W2Mass = random_w2.BreitWigner(w_mass, w_width);
                T1Mass = random_t1.BreitWigner(top_mass, top_width);
                T2Mass = random_t2.BreitWigner(top_mass, top_width);

                this->theSolver.solve(results->ETMiss, results->BBarPt, results->BPt, results->LPPt, results->LMPt, W1Mass, W2Mass, T1Mass, T2Mass, 0.0, 0.0,
                        &this->pnux, &this->pnuy, &this->pnuz, &this->pnubx, &this->pnuby, &this->pnubz, &cd_diff, cubic_single_root_cmplx);

                if (pnux.size() > 0) {
                    Solved = true;
                    results->SwapBs();
                    break;
                }
            }
        }
    }

    this->hNLoopsToSolve->Fill(counter);
    this->hNSolutions->Fill(pnux.size());

    results->SetSolverResultsAndAnalyze(&pnux, &pnuy, & pnuz, &pnubx, &pnuby, &pnubz);
    //find the best answer with minimizing the effective mass of ttbar
    if (results->NumberOfSolutions > 0) {

        ev->SelectASolution(this->Name, TopAnalysis::TTBarDileptonicEvent::SolverResults::MinEffMass);
        this->allHistograms.Fill(ev ,ev->Weight );

        bool isFirst(true);
        for (std::map<double, int>::const_iterator itr = results->SolEffMass.begin(); itr != results->SolEffMass.end(); itr++) {
            if (!results->GetTops((TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions) (100 + itr->second)))
                throw ("wrong solution number");

            if (isFirst) {
                this->hW1Mass_Selected->Fill(results->TopBar_Rec.getW().M());
                this->hW2Mass_Selected->Fill(results->Top_Rec.getW().M());
                this->hT1Mass_Selected->Fill(results->TopBar_Rec.getTop().M());
                this->hT2Mass_Selected->Fill(results->Top_Rec.getTop().M());
                this->hEffMass_Selected->Fill(itr->first);

                isFirst = false;
            } else {
                this->hW1Mass_Wrongs->Fill(results->TopBar_Rec.getW().M());
                this->hW2Mass_Wrongs->Fill(results->Top_Rec.getW().M());
                this->hT1Mass_Wrongs->Fill(results->TopBar_Rec.getTop().M());
                this->hT2Mass_Wrongs->Fill(results->Top_Rec.getTop().M());
                this->hEffMass_Wrongs->Fill(itr->first);
            }
        }
        return true;

    } else
        return false;
    //
}

void NeutrinoSolver::End() {
    TFile* file = info::TheInfo->OutFile;
    file->cd();
    TDirectory* dir = file->mkdir(this->Name.c_str());
    dir->cd();

    this->hNLoopsToSolve->Write();
    this->hNSolutions->Write();

    this->hW1Mass_Selected->Write();
    this->hW2Mass_Selected->Write();
    this->hT1Mass_Selected->Write();
    this->hT2Mass_Selected->Write();
    this->hEffMass_Selected->Write();

    this->hW1Mass_Wrongs->Write();
    this->hW2Mass_Wrongs->Write();
    this->hT1Mass_Wrongs->Write();
    this->hT2Mass_Wrongs->Write();
    this->hEffMass_Wrongs->Write();

    this->allHistograms.Write(dir);
    //cout << this->Name << " ended" << endl;
    file->cd();
}
