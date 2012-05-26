#ifndef DILEPTONEVENTPROPERTIES_H
#define	DILEPTONEVENTPROPERTIES_H

#include "BaseAnalysis/interface/ObjectProperty.h"
#include "DiElectronEvent.h"
#include "Electron.h"
#include <math.h>
#include <limits>

#include <string>
#include <TSystem.h>
#include <TH1D.h>
#include <map>

using namespace std;

namespace ElectronAnalysis {
    namespace DiLeptonEventProperties {

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

        template<Electron::Objects what = Electron::_Momentum, class TheEventType = ElectronAnalysis::DiLepton>
                class InvariantMass : public ObjectProperty<TheEventType> {
        public:

            InvariantMass() : ObjectProperty<TheEventType>("InvariantMass", "Invariant Mass", 0, 500, 50, "DiLeptonEvent", 500 + int(what)) {
                string s = "";
                string s2 = "";
                switch (what) {
                    case Electron::_Momentum:
                        s = "Dilepton ";
                        s2 = "Dilepton_";
                        break;
                    case Electron::_BestMatch1:
                        s = "BM1 ";
                        s2 = "BM1_";
                        break;
                    case Electron::_BestMatch3:
                        s = "BM3  ";
                        s2 = "BM3_";
                        break;
                        //	case Electron::_GSFTrack :
                        //	  s = "GSFTracks ";
                        //	  s2 = "GSFTracks_";
                        //	  break;
                        //	case Electron::_GSFTrack_OUT :
                        //	  s = "GSFTracks_OUT ";
                        //	  s2 = "GSFTracks_OUT_";
                        //	  break;
                    case Electron::_NearestTrack:
                        s = "Non GSF Tracks ";
                        s2 = "NonGSFTracks_";
                        break;
                        //	case Electron::_Seed :
                        //	  s = "Seeds ";
                        //	  s2 = "Seeds_";
                        //	  break;
                    case Electron::_SuperCluster:
                        s = "Super Clusters ";
                        s2 = "SuperClusters_";
                        break;
                }
                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;
            };

            virtual ~InvariantMass() {
            };

            virtual double ReadValue(const TheEventType* t) const {
                return t->GetInvMass(what);
            };
        };

        template<class TheEventType = ElectronAnalysis::DiLepton>
        class EventType : public ObjectProperty<TheEventType> {
        public:

            EventType() : ObjectProperty<TheEventType>("EventType", "Event Type", 0, 4, 8, "DiLeptonEvent", 1) {
            };

            virtual ~EventType() {
            };

            virtual void SetAxis(TAxis* axis) {
                axis->SetBinLabel(1, "ee os");
                axis->SetBinLabel(2, "ee ss");
                axis->SetBinLabel(3, "#mu#mu os");
                axis->SetBinLabel(4, "#mu#mu ss");
                axis->SetBinLabel(5, "e#mu os");
                axis->SetBinLabel(6, "e#mu ss");
                axis->SetBinLabel(7, "#mu e os");
                axis->SetBinLabel(8, "#mu e ss");
            };

            virtual double ReadValue(const TheEventType* t) const {

                double SS = (t->FirstElectron->Charge == t->SecondElectron->Charge) ? 0.25 : -0.25;

                if (t->FirstElectron->isElectron() && t->SecondElectron->isElectron())
                    return 0.5 + SS;

                if (t->FirstElectron->isMuon() && t->SecondElectron->isMuon())
                    return 1.5 + SS;

                if (t->FirstElectron->isElectron() && t->SecondElectron->isMuon())
                    return 2.5 + SS;

                if (t->FirstElectron->isMuon() && t->SecondElectron->isElectron())
                    return 3.5 + SS;
            };
        };

        template<int LeptonNumber, class TheEventType = ElectronAnalysis::DiLepton>
        class LeptonPt : public ObjectProperty<TheEventType> {
        public:

            LeptonPt() : ObjectProperty<TheEventType>("LeptonPt", "Lepton P_t", 5, 205, 50, "DiLeptonEvent", 2) {
                string s = "";
                string s2 = "";
                if (LeptonNumber == 1) {
                    s = "First ";
                    s2 = "First";
                } else if (LeptonNumber == 2) {
                    s = "Second ";
                    s2 = "Second";
                } else if (LeptonNumber == 3) {
                    s = "Positive ";
                    s2 = "Positive";
                } else if (LeptonNumber == 4) {
                    s = "Negative ";
                    s2 = "Negative";
                } else
                    throw Exception(this->PropertyTitle);

                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;

                this->PropertyID += (LeptonNumber - 1);
            };

            virtual ~LeptonPt() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TheEventType* t) const {
                if (LeptonNumber == 1) {
                    return t->FirstElectron->Pt;
                } else if (LeptonNumber == 2) {
                    return t->SecondElectron->Pt;
                } else if (LeptonNumber == 3) {
                    if (t->SecondElectron->Charge > 0)
                        return t->SecondElectron->Pt;
                    else
                        return t->FirstElectron->Pt;
                } else if (LeptonNumber == 4) {
                    if (t->SecondElectron->Charge < 0)
                        return t->SecondElectron->Pt;
                    else
                        return t->FirstElectron->Pt;
                }
            };
        };

        template<int LeptonNumber, class TheEventType = ElectronAnalysis::DiLepton>
        class LeptonEta : public ObjectProperty<TheEventType> {
        public:

            LeptonEta() : ObjectProperty<TheEventType>("LeptonEta", "Lepton #Eta", -3, 3, 60, "DiLeptonEvent", 4) {
                string s = "";
                string s2 = "";
                if (LeptonNumber == 1) {
                    s = "First ";
                    s2 = "First";
                } else if (LeptonNumber == 2) {
                    s = "Second ";
                    s2 = "Second";
                } else if (LeptonNumber == 3) {
                    s = "Positive ";
                    s2 = "Positive";
                } else if (LeptonNumber == 4) {
                    s = "Negative ";
                    s2 = "Negative";
                }
                else
                    throw Exception(this->PropertyTitle);

                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;

                this->PropertyID += (LeptonNumber - 1);
            };

            virtual ~LeptonEta() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TheEventType* t) const {
                if (LeptonNumber == 1) {
                    return t->FirstElectron->Eta;
                } else if (LeptonNumber == 2) {
                    return t->SecondElectron->Eta;
                } else if (LeptonNumber == 3) {
                    if (t->SecondElectron->Charge > 0)
                        return t->SecondElectron->Eta;
                    else
                        return t->FirstElectron->Eta;
                } else if (LeptonNumber == 4) {
                    if (t->SecondElectron->Charge < 0)
                        return t->SecondElectron->Eta;
                    else
                        return t->FirstElectron->Eta;
                }
            };
        };

        template<int JetNumber, class TheEventType = ElectronAnalysis::DiLepton>
        class JetEta : public ObjectProperty<TheEventType> {
        public:

            JetEta() : ObjectProperty<TheEventType>("JetEta", "Jet #Eta", -3, 3, 60, "DiLeptonEvent", 6) {
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

            virtual ~JetEta() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TheEventType* t) const {
                if (JetNumber < t->Jets.size())
                    return t->Jets[JetNumber].Eta();
                else
                    return -100.0;
            };
        };

        template<int JetNumber, int btag_alg, class TheEventType = ElectronAnalysis::DiLepton>
        class JetBTag : public ObjectProperty<TheEventType> {
        public:

            JetBTag() : ObjectProperty<TheEventType>("JetBTag", "Jet btag value", -100, 100, 200, "DiLeptonEvent", 1052) {
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
                } else {
                    throw Exception(this->PropertyTitle);
                }

                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;

                if (btag_alg == 1) {
                    this->PropertyTitle = this->PropertyTitle + " (TrackCountingHighEff)";
                    this->PropertyName = s2 + this->PropertyName + "_TrackCountingHighEff";
                } else if (btag_alg == 2) {
                    this->PropertyTitle = this->PropertyTitle + " (simpleSecondaryVertexHighEff)";
                    this->PropertyName = s2 + this->PropertyName + "_simpleSecondaryVertexHighEff";
                } else
                    throw Exception(this->PropertyTitle);

                this->PropertyID += (btag_alg)*(JetNumber - 1);
            };

            virtual ~JetBTag() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TheEventType* t) const {
                if (t == NULL || !t)
                    return 102.0;

                if (JetNumber < t->Jets.size()) {
                    if (btag_alg == 1)
                        return t->Jets_TrackCountingHighEff[JetNumber];
                    else if (btag_alg == 2)
                        return t->Jets_simpleSecondaryVertexHighEff[JetNumber];
                } else
                    return -101.0;
            };
        };

        template<int JetNumber, class TheEventType = ElectronAnalysis::DiLepton>
        class JetPt : public ObjectProperty<TheEventType> {
        public:

            JetPt() : ObjectProperty<TheEventType>("JetPt", "Jet P_t", 20, 220, 50, "DiLeptonEvent", 10) {
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

            virtual ~JetPt() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TheEventType* t) const {
                if (JetNumber < t->Jets.size())
                    return t->Jets[JetNumber].Pt();
                else
                    return -100.0;
            };
        };

        template<class TheEventType = ElectronAnalysis::DiLepton>
        class NumberOfJets : public ObjectProperty<TheEventType> {
        public:

            NumberOfJets() : ObjectProperty<TheEventType>("NumberOfJets", "# Jets", 0, 15, 15, "DiLeptonEvent", 15) {
            };

            virtual ~NumberOfJets() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const TheEventType* t) const {
                return t->Jets.size();
            };
        };

        template<int iii, class TheEventType = ElectronAnalysis::DiLepton> // 0 : just jets. 1 : +leptnos. 2 : +leptons +PFMet
        class HT : public ObjectProperty<TheEventType> {
        public:

            HT() : ObjectProperty<TheEventType>("HT", "HT", 20, 620, 60, "DiLeptonEvent", 16) {
                string s = "";
                string s2 = "";
                if (iii == 0) {
                    s = "Jets ";
                    s2 = "Jets";
                } else if (iii == 1) {
                    s = "Jets + Leptons ";
                    s2 = "JetsLeptons";
                } else if (iii == 2) {
                    s = "Jets + Leptons + PFMET ";
                    s2 = "JetsLeptonsPFMET";
                } else
                    throw Exception(this->PropertyTitle);

                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;

                this->PropertyID += (iii);
            };

            virtual ~HT() {
            };

            virtual double ReadValue(const TheEventType* t) const {
                double ret = 0;
                for (uint i = 0; i < t->Jets.size(); i++)
                    ret += t->Jets[i].Pt();

                if (iii > 0) {
                    ret += t->FirstElectron->Pt;
                    ret += t->SecondElectron->Pt;
                }

                if (iii == 2)
                    ret += t->PFMET.Rho();

                return ret;
            };
        };

        template<int iii, class TheEventType = ElectronAnalysis::DiLepton> // 0 : just jets. 1 : +leptnos. 2 : +leptons +PFMet
        class MHT : public ObjectProperty<TheEventType> {
        public:

            MHT() : ObjectProperty<TheEventType>("MHT", "MHT", 20, 620, 60, "DiLeptonEvent", 20) {
                string s = "";
                string s2 = "";
                if (iii == 0) {
                    s = "Jets ";
                    s2 = "Jets";
                } else if (iii == 1) {
                    s = "Jets + Leptons ";
                    s2 = "JetsLeptons";
                } else if (iii == 2) {
                    s = "Jets + Leptons + PFMET ";
                    s2 = "JetsLeptonsPFMET";
                } else
                    throw Exception(this->PropertyTitle);

                this->PropertyTitle = s + this->PropertyTitle;
                this->PropertyName = s2 + this->PropertyName;

                this->PropertyID += (iii);
            };

            virtual ~MHT() {
            };

            virtual double ReadValue(const TheEventType* t) const {
                double retX = 0;
                double retY = 0;
                for (uint i = 0; i < t->Jets.size(); i++) {
                    retX += t->Jets[i].Px();
                    retY += t->Jets[i].Py();
                }

                if (iii > 0) {
                    retX += t->FirstElectron->Px;
                    retY += t->FirstElectron->Py;

                    retX += t->SecondElectron->Px;
                    retY += t->SecondElectron->Py;
                }

                if (iii == 2) {
                    retX += t->PFMET.x();
                    retY += t->PFMET.y();
                }

                return sqrt(pow(retX, 2) + pow(retY, 2));
            };
        };

        template<class TheEventType = ElectronAnalysis::DiLepton>
        class PFMet : public ObjectProperty<TheEventType> {
        public:

            PFMet() : ObjectProperty<TheEventType>("PFMET", "PFMET", 20, 220, 50, "DiLeptonEvent", 25) {

            };

            ~PFMet() {
            };

            virtual double ReadValue(const TheEventType* t) const {
                return t->PFMET.rho();
            }
        };
    }
}
#endif	/* DILEPTONEVENTPROPERTIES_H */

