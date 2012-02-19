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
AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")) {

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

    this->ETMiss[0] = ev->PFMET.X();
    this->ETMiss[1] = ev->PFMET.Y();

    this->BPt[1] = ev->Top_Rec.b.X();
    this->BPt[2] = ev->Top_Rec.b.Y();
    this->BPt[3] = ev->Top_Rec.b.Z();
    this->BPt[0] = ev->Top_Rec.b.T();

    this->BBarPt[1] = ev->TopBar_Rec.b.X();
    this->BBarPt[2] = ev->TopBar_Rec.b.Y();
    this->BBarPt[3] = ev->TopBar_Rec.b.Z();
    this->BBarPt[0] = ev->TopBar_Rec.b.T();

    this->LPPt[1] = ev->Top_Rec.W.lepton.X();
    this->LPPt[2] = ev->Top_Rec.W.lepton.Y();
    this->LPPt[3] = ev->Top_Rec.W.lepton.Z();
    this->LPPt[0] = ev->Top_Rec.W.lepton.T();

    this->LMPt[1] = ev->TopBar_Rec.W.lepton.X();
    this->LMPt[2] = ev->TopBar_Rec.W.lepton.Y();
    this->LMPt[3] = ev->TopBar_Rec.W.lepton.Z();
    this->LMPt[0] = ev->TopBar_Rec.W.lepton.T();

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
        this->theSolver.solve(this->ETMiss, this->BPt, this->BBarPt, this->LPPt, this->LMPt, this->w_mass, this->w_mass, this->top_mass, this->top_mass, 0.0, 0.0,
                &this->pnux, &this->pnuy, &this->pnuz, &this->pnubx, &this->pnuby, &this->pnubz, &cd_diff, cubic_single_root_cmplx);
    } else {
        double W1Mass, W2Mass, T1Mass, T2Mass;
        bool Solved(false);
        for (; counter < MaxLoopNumberToSolve; counter++) {
            W1Mass = random_w1.BreitWigner(w_mass, w_width);
            W2Mass = random_w2.BreitWigner(w_mass, w_width);
            T1Mass = random_t1.BreitWigner(top_mass, top_width);
            T2Mass = random_t2.BreitWigner(top_mass, top_width);

            this->theSolver.solve(this->ETMiss, this->BPt, this->BBarPt, this->LPPt, this->LMPt, W1Mass, W2Mass, T1Mass, T2Mass, 0.0, 0.0,
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

                this->theSolver.solve(this->ETMiss, this->BBarPt, this->BPt, this->LPPt, this->LMPt, W1Mass, W2Mass, T1Mass, T2Mass, 0.0, 0.0,
                        &this->pnux, &this->pnuy, &this->pnuz, &this->pnubx, &this->pnuby, &this->pnubz, &cd_diff, cubic_single_root_cmplx);

                if (pnux.size() > 0) {
                    Solved = true;
                    TLorentzVector tmp(ev->Top_Rec.b);
                    ev->Top_Rec.b = ev->TopBar_Rec.b;
                    ev->TopBar_Rec.b = tmp;
                    break;
                }
            }
        }
    }

    this->hNLoopsToSolve->Fill(counter);
    this->hNSolutions->Fill(pnux.size());

    //find the best answer with minimizing the effective mass of ttbar
    double wmass1[10], wmass2[10], tmass1[10], tmass2[10], effmas[10];
    int minEffMassIdx = -1;
    double tmpEffMass = 10000000.0;
    for (int i = 0; i < pnux.size(); i++) {
        double nuE = sqrt(pnux[i] * pnux[i] + pnuy[i] * pnuy[i] + pnuz[i] * pnuz[i]);
        double nubE = sqrt(pnubx[i] * pnubx[i] + pnuby[i] * pnuby[i] + pnubz[i] * pnubz[i]);

        ev->Top_Rec.W.neutrino.SetPxPyPzE(pnux[i], pnuy[i], pnuz[i], nuE);
        ev->TopBar_Rec.W.neutrino.SetPxPyPzE(pnubx[i], pnuby[i], pnubz[i], nubE);

        wmass1[i] = ev->Top_Rec.getW().M();
        wmass2[i] = ev->TopBar_Rec.getW().M();
        tmass1[i] = ev->Top_Rec.getTop().M();
        tmass2[i] = ev->TopBar_Rec.getTop().M();
        effmas[i] = (ev->Top_Rec.getTop() + ev->TopBar_Rec.getTop()).M();

        if (effmas[i] < tmpEffMass) {
            minEffMassIdx = i;
            tmpEffMass = effmas[i];
        }
    }

    if (minEffMassIdx > -1) {
        double nuE = sqrt(pnux[minEffMassIdx] * pnux[minEffMassIdx] + pnuy[minEffMassIdx] * pnuy[minEffMassIdx] + pnuz[minEffMassIdx] * pnuz[minEffMassIdx]);
        double nubE = sqrt(pnubx[minEffMassIdx] * pnubx[minEffMassIdx] + pnuby[minEffMassIdx] * pnuby[minEffMassIdx] + pnubz[minEffMassIdx] * pnubz[minEffMassIdx]);
        
        ev->Top_Rec.W.neutrino.SetPxPyPzE(pnux[minEffMassIdx], pnuy[minEffMassIdx], pnuz[minEffMassIdx], nuE);
        ev->TopBar_Rec.W.neutrino.SetPxPyPzE(pnubx[minEffMassIdx], pnuby[minEffMassIdx], pnubz[minEffMassIdx], nubE);

        this->hW1Mass_Selected->Fill(wmass1[minEffMassIdx]);
        this->hW2Mass_Selected->Fill(wmass2[minEffMassIdx]);
        this->hT1Mass_Selected->Fill(tmass1[minEffMassIdx]);
        this->hT2Mass_Selected->Fill(tmass2[minEffMassIdx]);
        this->hEffMass_Selected->Fill(effmas[minEffMassIdx]);

        for (int kk = 0; kk < pnux.size() && kk != minEffMassIdx; kk++) {
            this->hW1Mass_Wrongs->Fill(wmass1[kk]);
            this->hW2Mass_Wrongs->Fill(wmass2[kk]);
            this->hT1Mass_Wrongs->Fill(tmass1[kk]);
            this->hT2Mass_Wrongs->Fill(tmass2[kk]);
            this->hEffMass_Wrongs->Fill(effmas[kk]);
        }

        ev->hasBeenSolved = true;
        return true;
    } else
        return false;
    //
}

void NeutrinoSolver::End() {
    TFile* file = info::TheInfo->OutFile;
    file->cd();
    file->mkdir(this->Name.c_str())->cd();

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

    //cout << this->Name << " ended" << endl;
    file->cd();
}
