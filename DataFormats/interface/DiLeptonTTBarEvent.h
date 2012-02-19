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
            Others = 16
        };

        TopDecays GenDecayMode;
        TopDecays RecDecayMode;

        class TopDecayChain {
        public:
            TLorentzVector b;

            struct WBoson {
                TLorentzVector lepton;
                TLorentzVector neutrino;

            } W;

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

            SolverResults(vector<double> pnux, vector<double> pnuy, vector<double> pnuz, vector<double> pnubx, vector<double> pnuby, vector<double> pnubz);

            SolverResults(string name, TLorentzVector the_b, TLorentzVector the_bbar, TLorentzVector the_lminus, TLorentzVector the_lplus);

            std::map< double, int > SolEffMass;
            std::map< double, int > SolTopDR;
            std::map< double, int > SolTopBarDR;
            std::map< double, int > SolTTBarDR;
            std::map< double, int > SolNuChi2;
            std::map< double, int > SolNuBarChi2;
            std::map< double, int > SolNuNuBarChi2;

            void AnalyzeSolutions();

            enum solutions{
                MinEffMass,
                MinTopDR,
                MinTopbarDR,
                MinTTbarDR,
                MinNuChi2,
                MinNubarChi2,
                MinNuNubarChi2
            };
        
            TopDecayChain Top_Rec;
            TopDecayChain TopBar_Rec;
            
            void GetTops(solutions solution,TopDecayChain** top ,TopDecayChain** topbar  );
        };

        std::map<string, SolverResults> AllSolutions;
        SolverResults* AddSolverResults(string name, TLorentzVector the_b, TLorentzVector the_bbar);
        bool SelectASolution(string name, SolverResults::solutions solution);

        //

        TTBarDileptonicEvent();

        void SetLeptons(Lepton* lep1, Lepton* lep2);

        bool hasBeenSolved;
        bool hasGenInfo;
    };
}
#endif	/* DILEPTONTTBAREVENT_H */

