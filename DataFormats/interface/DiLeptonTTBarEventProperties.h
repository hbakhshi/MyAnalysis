/* 
 * File:   DiLeptonTTBarEventProperties.h
 * Author: hbakhshi
 *
 * Created on February 8, 2012, 4:04 PM
 */

#ifndef DILEPTONTTBAREVENTPROPERTIES_H
#define	DILEPTONTTBAREVENTPROPERTIES_H

#include "BaseAnalysis/interface/ObjectProperty.h"
#include "DiElectronEvent.h"
#include "DiElectronEventProperties.h"
#include "DiLeptonEventProperties.h"
#include "Electron.h"

#include "DiLeptonTTBarEvent.h"
#include <string>

#include <TH1D.h>

using namespace std;

namespace TopAnalysis {
    namespace DiLeptonTTBarEventProperties {

        class Exception : public exception {
        public:

            string __WHAT;

            Exception(string WHAT) : __WHAT(WHAT) {
            }

            virtual ~Exception() throw () {
            };

            virtual const char* what() const throw () {

                return __WHAT.c_str();
            }

        };

        typedef ElectronAnalysis::DiLeptonEventProperties::EventType<TopAnalysis::TTBarDileptonicEvent> EventType;
        typedef ElectronAnalysis::DiLeptonEventProperties::NumberOfJets<TopAnalysis::TTBarDileptonicEvent> NumberOfJets;

        class NumberOfBJets : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            NumberOfBJets() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("NumberOfBJets", "Number of b-jets", 0, 6, 6, "DiLeptonEvent", 1505) {
            };

            virtual ~NumberOfBJets() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                return t->BJets.size();
            };
        };

        typedef ElectronAnalysis::DiLeptonEventProperties::PFMet<TopAnalysis::TTBarDileptonicEvent> PFMet;

        template<int iii>
        struct HT {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::HT<iii, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<int iii>
        struct MHT {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::MHT<iii, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<ElectronAnalysis::Electron::Objects what = ElectronAnalysis::Electron::_Momentum>
                struct InvariantMass {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::InvariantMass<what, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<int JetNumber, int btag_alg>
        struct JetBTag {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::JetBTag<JetNumber, btag_alg, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<int JetNumber >
        class BJetPt : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            BJetPt() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("BJetPt", "BJet P_t", 20, 220, 50, "DiLeptonEvent", 10) {
                string s = "";
                string s2 = "";
                if (JetNumber == 1) {
                    s = "First ";
                    s2 = "First";
                } else if (JetNumber == 2) {
                    s = "Second ";
                    s2 = "Second";
                } else if (JetNumber == 3) {
                    s = "Third ";
                    s2 = "Third";
                } else
                    throw Exception(this->PropertyTitle);

                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;

                this->PropertyID += (JetNumber - 1);
            };

            virtual ~BJetPt() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                if (JetNumber < t->BJets.size())
                    return t->Jets[ t->BJets[JetNumber] ].Pt();
                else
                    return -100.0;
            };
        };

        template<int JetNumber>
        struct JetPt {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::JetPt<JetNumber, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<int JetNumber>
        struct JetEta {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::JetEta<JetNumber, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<int LeptonNumber>
        struct LeptonEta {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::LeptonEta<LeptonNumber, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<int LeptonNumber>
        struct LeptonPt {
        public:
            typedef ElectronAnalysis::DiLeptonEventProperties::LeptonPt<LeptonNumber, TopAnalysis::TTBarDileptonicEvent> type;
        };

        template<bool Gen>
        class EffectiveMass : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            EffectiveMass() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("TTBarEffectiveMass", "TTBar Effective Mass", 330, 1330, 1000, "TTBarDileptonicEvent", 1) {
                if (Gen) {
                    this->PropertyName = ("Gen" + this->PropertyName);
                    this->PropertyTitle = ("Gen " + this->PropertyTitle);
                    this->PropertyID = 2;
                }
            };

            ~EffectiveMass() {
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                if (Gen)
                    return (t->TOP_Gen.getTop() + t->TOPBar_Gen.getTop()).M();
                else {
                    return (t->Top_Rec->getTop() + t->TopBar_Rec->getTop()).M();
                }
            };
        };

        class NPrimaryVertices : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            NPrimaryVertices() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("NPrVtx", "#Primary vertices", 0, 50, 50, "TTBarDileptonicEvent", 45522) {
            };

            ~NPrimaryVertices() {
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                return t->NPrimaryVertices;
            };
        };

        template<bool Gen>
        class TTBarDecayMode : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            TTBarDecayMode() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("TTBarDecayMode", "TTBar Decay Mode", 0, 17, 17, "TTBarDileptonicEvent", 3) {
                if (Gen) {
                    this->PropertyName = ("Gen" + this->PropertyName);
                    this->PropertyTitle = ("Gen " + this->PropertyTitle);
                    this->PropertyID = 4;
                }
            };

            virtual ~TTBarDecayMode() {
            };

            virtual void SetAxis(TAxis* axis) {
                axis->SetBinLabel(1, "Not Set");
                axis->SetBinLabel(2, "e+ #mu-");
                axis->SetBinLabel(3, "e- #mu+");
                axis->SetBinLabel(4, "DiEle");
                axis->SetBinLabel(5, "DiMuo");
                axis->SetBinLabel(6, "DiTau");
                axis->SetBinLabel(7, "MuTau");
                axis->SetBinLabel(8, "ElTau");
                axis->SetBinLabel(9, "ElJets");
                axis->SetBinLabel(10, "Mu jets");
                axis->SetBinLabel(11, "TauJet");
                axis->SetBinLabel(12, "Full Hadronic");
                axis->SetBinLabel(13, "Just1B");
                axis->SetBinLabel(14, "ZeroBs");
                axis->SetBinLabel(15, "ExtraBs");
                axis->SetBinLabel(16, "BadNu");
                axis->SetBinLabel(17, "Others");
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                if (Gen)
                    return double(t->GenDecayMode);
                else
                    return double(t->RecDecayMode);
            };
        };

        //TopAntiTop==1 means top 2 antitop
        //obj : 1 Top
        //      2 W
        //      3 b
        //      4 lepton
        //      5 neutrino
        //what: 1 DR
        //      2 DPhi
        //      3 DEta
        //      4 DPt/PtGen
        //      5 (pgen-prec)^2
        //      6 |pgen-prec|/|pgen|

        template<int TopAntiTop, int obj, int what >
        class GenRecComp : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            GenRecComp() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("", "", 0, 6.0, 120, "TTBarDileptonicEvent", 100) {
                string part1 = TopAntiTop == 1 ? "Top" : "Antitop";
                string part2 = "";
                string part2_title = "";
                switch (obj) {
                    case 1:
                        part2 = part2_title = "Main";
                        break;
                    case 2:
                        part2 = part2_title = "W";
                        break;
                    case 3:
                        part2 = part2_title = "b";
                        break;
                    case 4:
                        part2 = part2_title = "lepton";
                        break;
                    case 5:
                        part2 = "neutrinos";
                        part2_title = "#nu";
                        break;
                }
                string part3 = "";
                string part3_title = "";
                switch (what) {
                    case 1:
                        part3 = "DR";
                        part3_title = "#Delta R";
                        this->MinValue = 0.0;
                        this->MaxValue = 6.0;
                        this->NumberOfBins = 120;
                        break;
                    case 2:
                        part3 = "DPhi";
                        part3_title = "#Delta #phi";
                        this->MinValue = -ROOT::Math::Pi();
                        this->MaxValue = ROOT::Math::Pi();
                        this->NumberOfBins = 80;
                        break;
                    case 3:
                        part3 = "DEta";
                        part3_title = "#Delta #eta";
                        this->MinValue = 0.0;
                        this->MaxValue = 10.0;
                        this->NumberOfBins = 100;
                        break;
                    case 4:
                        part3 = "DPtOverPtGen";
                        part3_title = "#Delta p_t / p_t^gen ";
                        this->MinValue = -10.0;
                        this->MaxValue = 10.0;
                        this->NumberOfBins = 200;
                        break;
                    case 5:
                        part3 = "pgen_prec";
                        part3_title = "(P_{gen} - P_{rec})";
                        this->MinValue = 0.0;
                        this->MaxValue = 2000.0;
                        this->NumberOfBins = 2000;
                        break;
                    case 6:
                        part3 = "pgen_prec_over_pgen";
                        part3_title = "|P_{gen} - P_{rec}| / |P_{gen}| ";
                        this->MinValue = -10.0;
                        this->MaxValue = 10.0;
                        this->NumberOfBins = 200;
                        break;
                    case 7:
                        part3 = "gen_pt_value";
                        part3_title = "p_{t} ";
                        this->MinValue = 0.0;
                        this->MaxValue = 200;
                        this->NumberOfBins = 20;
                        break;
                    case 8:
                        part3 = "gen_eta_value";
                        part3_title = "#eta ";
                        this->MinValue = -10.0;
                        this->MaxValue = 10.0;
                        this->NumberOfBins = 20;
                        break;
                }

                this->PropertyName = part1 + part2 + part3;
                this->PropertyTitle = part3_title + " for " + part2_title + "'s in " + part1 + "s";

                this->PropertyID = 100 * TopAntiTop + 10 * obj + what;
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                TLorentzVector gen;
                TLorentzVector rec;

                const TopAnalysis::TTBarDileptonicEvent::TopDecayChain* t_gen = (TopAntiTop == 1 ? &(t->TOP_Gen) : &(t->TOPBar_Gen));
                const TopAnalysis::TTBarDileptonicEvent::TopDecayChain* t_rec = (TopAntiTop == 1 ? (t->Top_Rec) : (t->TopBar_Rec));

                switch (obj) {
                    case 1:
                        gen = t_gen->getTop();
                        rec = t_rec->getTop();
                        break;
                    case 2:
                        gen = t_gen->getW();
                        rec = t_rec->getW();
                        break;
                    case 3:
                        gen = t_gen->b;
                        rec = t_rec->b;
                        break;
                    case 4:
                        gen = t_gen->W.lepton;
                        rec = t_rec->W.lepton;
                        break;
                    case 5:
                        gen = t_gen->W.neutrino;
                        rec = t_rec->W.neutrino;
                        break;
                }

                double ret = -100000.0;
                double dpt;
                switch (what) {
                    case 1:
                        ret = ROOT::Math::VectorUtil::DeltaR<TLorentzVector, TLorentzVector > (gen, rec);
                        break;
                    case 2:
                        ret = ROOT::Math::VectorUtil::DeltaPhi<TLorentzVector, TLorentzVector > (gen, rec);
                        break;
                    case 3:
                        ret = fabs(gen.Eta() - rec.Eta());
                        break;
                    case 4:
                        dpt = gen.Pt() - rec.Pt();
                        ret = (dpt / gen.Pt());
                        break;
                    case 5:
                        ret = (gen.Vect() - rec.Vect()).Mag();
                        break;
                    case 6:
                        ret = (gen.Vect() - rec.Vect()).Mag() / gen.Vect().Mag();
                        break;
                    case 7:
                        ret = gen.Pt() ;
                        break;
                    case 8:
                        ret = gen.Eta() ;
                        break;
                }
                return ret;
            }
        };

      class LeptonsDPhi : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
      public:
	LeptonsDPhi() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("DPhi", "#Delta#Phi", -3.15 , 3.15 , 126 , "TTBarDileptonicEvent", 100778473) {
	}

	virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
	  math::XYZTLorentzVector rec =t->FirstElectron->get4Vector(0);
	  math::XYZTLorentzVector rec2=t->SecondElectron->get4Vector(0);

	  double ret = -100000.0;
	  ret = ROOT::Math::VectorUtil::DeltaPhi<math::XYZTLorentzVector, math::XYZTLorentzVector > (rec2, rec);
	  
	  return ret;
	}
      };
        
        //TopAntiTop==1 means top 2 antitop 3 difference between them
        //obj : 1 Top
        //      2 W
        //      3 neutrino
        //      4 lepton
        //what: 1 pt
        //      2 eta
        //      3 phi
        template<int TopAntiTop, int obj, int what >
        class TopRecRestuls : public ObjectProperty<TopAnalysis::TTBarDileptonicEvent> {
        public:

            TopRecRestuls() : ObjectProperty<TopAnalysis::TTBarDileptonicEvent>("", "", 0, 6.0, 120, "TTBarDileptonicEvent", 100) {
	      string part1 = TopAntiTop == 1 ? "Top" : ( TopAntiTop == 2 ? "Antitop" : ("TTBar") );
                string part2 = "";
                string part2_title = "";
                switch (obj) {
                    case 1:
                        part2 = part2_title = part1;
                        break;
                    case 2:
                        part2 = part2_title = "W";
                        break;
                    case 3:
                        part2 = "neutrinos";
                        part2_title = "#nu";
                        break;
                    case 4:
                        part2 = "leptons";
                        part2_title = "leptons";
                        break;
                }
                string part3 = "";
                string part3_title = "";
                switch (what) {
                    case 1:
                        part3 = "pt";
                        part3_title = "p_{t}";
                        this->MinValue = 0.0;
                        this->MaxValue = 500.0;
                        this->NumberOfBins = 50;
                        break;
                    case 2:
                        part3 = "Eta";
                        part3_title = "#eta";
                        this->MinValue = -5.0;
                        this->MaxValue = 5.0;
                        this->NumberOfBins = 50;
                        break;
                    case 3:
                        part3 = "Phi";
                        part3_title = "#phi";
                        this->MinValue = ROOT::Math::Pi();
                        this->MaxValue = ROOT::Math::Pi();
                        this->NumberOfBins = 126;
                        break;
                }

                this->PropertyName =  (TopAntiTop == 3 ? "dleta" : "") + part1 + part2 + part3;
                this->PropertyTitle = (TopAntiTop == 3 ? "#Delta" : "") + part3_title + " of " + part2_title + "'s in " + part1 + " reconstruction";

                this->PropertyID = 100 * TopAntiTop + 10 * obj + what;
            };

            virtual double ReadValue(const TopAnalysis::TTBarDileptonicEvent* t) const {
                TLorentzVector rec;
                TLorentzVector rec2;

                const TopAnalysis::TTBarDileptonicEvent::TopDecayChain* t_rec = (TopAntiTop == 1 ? (t->Top_Rec) : (t->TopBar_Rec));
                const TopAnalysis::TTBarDileptonicEvent::TopDecayChain* t2_rec = (TopAntiTop == 1 ? (t->TopBar_Rec) : (t->Top_Rec));

                switch (obj) {
                    case 1:
                        rec = t_rec->getTop();
                        rec2 = t2_rec->getTop();
                        break;
                    case 2:
                        rec = t_rec->getW();
                        rec2 = t2_rec->getW();
                        break;
                    case 3:
                        rec = t_rec->W.neutrino;
                        rec2 = t2_rec->W.neutrino;
                        break;
                }

                double ret = -100000.0;
                switch (what) {
                    case 1:
                        ret = rec.Pt();
			if(TopAntiTop == 3)
			  ret -= rec2.Pt() ;
                        break;
                    case 2:
                        ret = rec.Eta();
			if(TopAntiTop == 3)
			  ret -= rec2.Pt();
                        break;
		case 3:
		  ret = rec.Phi();
		  if(TopAntiTop == 3)
		    ret = ROOT::Math::VectorUtil::DeltaPhi<TLorentzVector, TLorentzVector > (rec2, rec);
		  break;
                }
                return ret;
            }
        };

    }
}

#endif	/* DILEPTONTTBAREVENTPROPERTIES_H */

