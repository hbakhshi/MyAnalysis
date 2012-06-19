/* 
 * File:   DiLeptonTTBarEvent.h
 * Author: hbakhshi
 *
 * Created on February 13, 2012, 8:02 PM
 */

#ifndef DILEPTONTTBAREVENT_H
#define	DILEPTONTTBAREVENT_H

#include "DiElectronEvent.h"
#include <map>

namespace TopAnalysis {

    class TTBarDileptonicEvent : public ElectronAnalysis::DiLepton {
    public:
        int PUnumInteractions;
        int NPrimaryVertices;
        //PER CHANNEL TRIGGER INFO
        bool TRG_DiMuon;
        bool TRG_DiEle;
        bool TRG_EleMuon;        
        //
        
        enum TopDecays {
            NOT_SET = 0,
            ElP_MuM = 1,
            ElM_MuP = 2,
            DiEle = 3,
            DiMu = 4,
            DiTau = 5,
            MuTau = 6,
            ElTau = 7,
            ElJets = 8,
            MuJets = 9,
            TauJets = 10,
            FullHadronic = 11,
            //for diagnostics purposes
            Just1B = 12,
            ZeroBs = 13,
            ExtraBs = 14,
            BadNu = 15,
            Others = 16,
            
            EE_Inconsistent_With_Trigger = 17,
            MM_Inconsistent_With_Trigger = 18,
            EM_Inconsistent_With_Trigger = 19
        };

        TopDecays GenDecayMode;
        TopDecays RecDecayMode;

        class TopDecayChain {
        public:
            TLorentzVector b;

            struct WBoson {
                TLorentzVector lepton;
                TLorentzVector neutrino;

                double leptonIsolationValue;
            } W;

            double LeptonBDeltaR() const{
                return ROOT::Math::VectorUtil::DeltaR<TLorentzVector, TLorentzVector >(this->W.lepton , this->b) ;
            };
            
            inline TLorentzVector getW() const {
                return W.lepton + W.neutrino;
            };

            inline TLorentzVector getTop() const {
                return getW() + b;
            };

            int Charge;

            TopDecayChain(int charge) : Charge(charge) {
            };

            double CosTheta() const;

            void Clear();
        };

        TopDecayChain TOP_Gen;
        TopDecayChain TOPBar_Gen;

        TopDecayChain* Top_Rec;
        TopDecayChain* TopBar_Rec;

        long int CurrentEntryInTree;


        //SolverResults

        class SolverResults {
        public:
            string Name;

            int NumberOfSolutions;
            std::vector< TLorentzVector > NeutrinoSolutions;
            std::vector< TLorentzVector > NeutrinoBarSolutions;

            //SolverResults(vector<double> pnux, vector<double> pnuy, vector<double> pnuz, vector<double> pnubx, vector<double> pnuby, vector<double> pnubz);

            SolverResults(string name,  math::XYZTLorentzVector the_b,  math::XYZTLorentzVector the_bbar,  math::XYZTLorentzVector the_lminus,  math::XYZTLorentzVector the_lplus, double met_x, double met_y, TopDecayChain* tgen, TopDecayChain* tbargen );
            void SwapBs();
            //to be able to put in a map
            SolverResults();

            double MWP;
            double MWM;
            double MTop;
            double MTBar;

            std::map< double, int > SolEffMass;
            std::map< double, int > SolTopDR;
            std::map< double, int > SolTopBarDR;
            std::map< double, int > SolTTBarDR;
            std::map< double, int > SolNuChi2;
            std::map< double, int > SolNuBarChi2;
            std::map< double, int > SolNuNuBarChi2;

            void SetSolverResultsAndAnalyze(vector<double>* pnux, vector<double>* pnuy, vector<double>* pnuz, vector<double>* pnubx, vector<double>* pnuby, vector<double>* pnubz);

            enum solutions {
                MinEffMass = 0,
                MinTopDR,
                MinTopbarDR,
                MinTTbarDR,
                MinNuChi2,
                MinNubarChi2,
                MinNuNubarChi2,
                Solution0 = 100,
                Solution1 = 101,
                Solution2 = 102,
                Solution3 = 103,
                Solution4 = 104,
                Solution5 = 105,
                Solution6 = 106,
                Solution7 = 107
            };

            TopDecayChain Top_Rec;
            TopDecayChain TopBar_Rec;

            TopDecayChain* Top_Gen;
            TopDecayChain* TopBar_Gen;

            TVector2 MET;

            bool GetTops(solutions solution, TopDecayChain** top = NULL, TopDecayChain** topbar = NULL);

            //information needed for solver
            double ETMiss[2];
            double BPt[4];
            double BBarPt[4];
            double LPPt[4];
            double LMPt[4];
        };

        std::map<string, SolverResults> AllSolutions;
        SolverResults* AddSolverResults(string name, int the_b_index, int the_bbar_index);
        bool SelectASolution(string name, SolverResults::solutions solution);

        //

        TTBarDileptonicEvent();

        void SetLeptons(Lepton* lep1, Lepton* lep2);

        bool hasBeenSolved;
        bool hasGenInfo;

        Lepton* GetLepton(int charge) {
            if (charge == 0)
                return NULL;
            else if (FirstElectron->Charge * charge > 0)
                return FirstElectron;
            else
                return SecondElectron;
        }
    };
}
#endif	/* DILEPTONTTBAREVENT_H */

