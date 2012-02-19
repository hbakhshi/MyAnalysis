/* 
 * File:   ElectronProperties.h
 * Author: hbakhshi
 *
 * Created on December 3, 2008, 2:09 PM
 */

#ifndef _ELECTRONPROPERTIES_H
#define	_ELECTRONPROPERTIES_H

//#include "ECalRecHitInfo.h"
#include <BaseAnalysis/interface/ObjectProperty.h>
#include <BaseAnalysis/interface/Histograms.h>
#include "Electron.h"
#include <math.h>
#include <sstream>
#include <map>

#include <BaseAnalysis/interface/EfficiencyHandler.h>

#include "ElectronProperties.h"

namespace ElectronAnalysis {
    namespace ElectronProperties {

        template<int status = 1 > //
                class Gen_Charge : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            Gen_Charge() : ObjectProperty<ElectronAnalysis::Electron>("GenCharge", "e_{charge}", -1, 2, 3, "Electron", -1) {
                if (status == 1) {
                    PropertyName = "Gen1Charge";
                    PropertyTitle = "charge_{gen}^{stat=1}";
                    PropertyID = 151;
                } else if (status == 3) {
                    PropertyName = "Gen3Charge";
                    PropertyTitle = "charge_{gen}^{stat=3}";
                    PropertyID = 153;
                }
            };

            virtual ~Gen_Charge() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                if (status == 1 && t->GenID != 0)
                    return t->GenCharge;
                else if (t->GenID != 0)
                    return t->GenCharge;

                return 0;
            };
        };

        template<Electron::ChargeState i = Electron::ElectronCharge>
                class NewCharge : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            NewCharge() : ObjectProperty<ElectronAnalysis::Electron>("Charge", "e_{charge}", -1, 2, 3, "Electron", 0) {
            };

            virtual ~NewCharge() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->NewCharge(i);
            };
        };

        typedef NewCharge<Electron::ElectronCharge> Charge;

        class Pt : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            Pt() : ObjectProperty<ElectronAnalysis::Electron>("Pt", "Transverse Momentum of electron", 0, 500, 500, "Electron", 1) {
            };

            virtual ~Pt() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->Pt;
            };
        };

        class Eta : public ObjectProperty<ElectronAnalysis::Electron> {
        public:
            typedef ElectronAnalysis::Electron T;
            typedef ObjectProperty<ElectronAnalysis::Electron> BASE;

            bool EndcapBarrel;

            Eta(bool EndcapBarrel_ = false) : BASE("Eta", "#left|#eta#right|", 0, 2.5, 25, "Electron", 220),
            EndcapBarrel(EndcapBarrel_) {
            };

            virtual TH1Ext<T>* GetH1(string name, string title = "") {
                TH1Ext<T>* ret = BASE::GetH1(name, title);
                if (EndcapBarrel) {
                    double bins[] = {0.0, 1.4442, 1.566, 2.5};
                    ret->SetBins(3, bins);
                }
                return ret;
            }

            virtual TH2Ext<T>* GetH2(ObjectProperty<T>* otherProperty, string name, string title = "") {
                TH2Ext<T>* ret = BASE::GetH2(otherProperty, name, title);
                if (EndcapBarrel) {
                    cout << "H2 EndCapBarrel" << otherProperty->PropertyName << endl;
                    double bins_Y[100];
                    int binY_iii = 0;
                    for(double bin_low_edge = otherProperty->MinValue;
                            bin_low_edge <= otherProperty->MaxValue;
                            bin_low_edge += (otherProperty->MaxValue-otherProperty->MinValue)/otherProperty->NumberOfBins,binY_iii++)
                        bins_Y[binY_iii] = bin_low_edge;

                    double bins[] = {0.0, 1.4442, 1.566, 2.5};
                    ret->SetBins(3, bins , otherProperty->NumberOfBins, bins_Y);
                }
                return ret;
            }

            virtual ~Eta() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return fabs(t->Eta);
            };
        };

        class Energy : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            Energy() : ObjectProperty<ElectronAnalysis::Electron>("Energy", "Energy of electron", 0, 500, 500, "Electron", 2) {
            };

            virtual ~Energy() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->E;
            };
        };

        class Pz : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            Pz() : ObjectProperty<ElectronAnalysis::Electron>("Pz", "Pz", 0, 500, 500, "Electron", 3) {
            };

            virtual ~Pz() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->Pz;
            };
        };

        class CaloEnergyError : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            CaloEnergyError() : ObjectProperty<ElectronAnalysis::Electron>("CaloEnergyError", "Calo Energy Error", 0, 4, 200, "Electron", 4) {
            };

            virtual ~CaloEnergyError() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return -1;
            };
        };

        class DiffCaloEnergy_ElectronEnergy : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DiffCaloEnergy_ElectronEnergy() : ObjectProperty<ElectronAnalysis::Electron>("DiffCaloEnergy_ElectronEnergy", "(E_{e}-E_{sc})/E_{sc}", -1, 1, 100, "Electron", 5) {
            };

            virtual ~DiffCaloEnergy_ElectronEnergy() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->EError(Electron::_Momentum, Electron::_SuperCluster);
            };
        };

        class CovEtaEta : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            CovEtaEta() : ObjectProperty<ElectronAnalysis::Electron>("CovEtaEta", "#sigma_{I#etaI#eta}", 0.05, 0.05, 500, "Electron", 6) {
            };

            virtual ~CovEtaEta() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElSigmaIetaIeta;
            };
        };

        class HoverE : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            HoverE() : ObjectProperty<ElectronAnalysis::Electron>("HoverE", "E_{Hadronic}/E_{Electromagnetic}", 0, 0.08, 16, "Electron", 7) {
            };

            virtual ~HoverE() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElHcalOverEcal;
            };
        };

        class DPhiSeedSuperCluster : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DPhiSeedSuperCluster() : ObjectProperty<ElectronAnalysis::Electron>("DPhiSeedSuperCluster", "(#phi_{sc}-#phi_{seed})", -0.15, 0.15, 300, "Electron", 8) {
            };

            virtual ~DPhiSeedSuperCluster() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return -100; //t->DeltaPhi(Electron::_Seed, Electron::_SuperCluster);
            };
        };

        class DiffTrackMom_Electron : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DiffTrackMom_Electron() : ObjectProperty<ElectronAnalysis::Electron>("DiffTrackMom_Electron", "(P_{electron} - P_{track})/P_{track}", -1.5, 2, 350, "Electron", 9) {
            };

            virtual ~DiffTrackMom_Electron() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return 0; //t->PError(Electron::_Momentum, Electron::_GSFTrack);
            };
        };

        class DiffTrackPt_Electron : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DiffTrackPt_Electron() : ObjectProperty<ElectronAnalysis::Electron>("DiffTrackPt_Electron", "(P_{t}^{electron} - P_{t}^{track})/P_{t}^{track}", -1.5, 2, 350, "Electron", 10) {
            };

            virtual ~DiffTrackPt_Electron() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return 0; //t->PtError(Electron::_Momentum, Electron::_GSFTrack);
            };
        };

        class BremFraction : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            BremFraction() : ObjectProperty<ElectronAnalysis::Electron>("BremFraction", "BremFraction", 0, 1, 100, "Electron", 11) {
            };

            virtual ~BremFraction() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->Elfbrem;
            };
        };

        class BremFraction_Exp : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            double power;

            BremFraction_Exp(double _power = 1.0) : ObjectProperty<ElectronAnalysis::Electron>("BremFraction_Exp", "e^{-#frac{1}{brem}}", 0, exp(-_power), 100, "Electron", 110) {
                power = _power;
                PropertyID = 110 + int(10 * power);

                std::ostringstream ss_name;
                std::ostringstream ss_title;
                ss_name.precision(2);
                ss_title.precision(2);

                ss_name << "BremFraction_Exp" << power;
                PropertyName = ss_name.str();

                ss_title << "e^{-#frac{" << power << "}{brem}}";
                PropertyTitle = ss_title.str();
            };

            virtual ~BremFraction_Exp() {
            };

            BremFraction bremReader;

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                double brem = bremReader.ReadValue(t);
                if (brem == 0)
                    return 0;
                else
                    return exp(-(power) / brem);
            };
        };

        class NChi2_Trk : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            NChi2_Trk() : ObjectProperty<ElectronAnalysis::Electron>("NChi2_Trk", "#chi^{2}_{normilized}", 0, 10, 100, "Electron", 12) {
            };

            virtual ~NChi2_Trk() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElNChi2;
            };
        };

        template <int i = 0 > // in case of 0 uses dxy calculated by primary vertex, else 000
                class TIP : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            TIP() : ObjectProperty<ElectronAnalysis::Electron>("TIP", "TIP", -0.2, 0.2, 200, "Electron", 13) {
                if (i == 0) {
                    this->PropertyName += "_PV";
                    this->PropertyTitle += "_{PV}";
                } else {
                    this->PropertyName += "_000";
                    this->PropertyTitle += "_{000}";
                    this->PropertyID = 1123;
                }
            };

            virtual ~TIP() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                if (i == 0)
                    return t->D0PV;
                else
                    return t->D0BS;
            };
        };

        class NGoodHits : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            NGoodHits() : ObjectProperty<ElectronAnalysis::Electron>("NGoodHits", "#Good Hits", 0, 30, 30, "Electron", 14) {
            };

            virtual ~NGoodHits() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return 0; //t->gsfTrack()->hitPattern().numberOfValidHits();
            };
        };

        class NPixelHits : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            NPixelHits() : ObjectProperty<ElectronAnalysis::Electron>("NPixelHits", "# Pixel Hits", 0, 10, 10, "Electron", 15) {
            };

            virtual ~NPixelHits() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return 0; //t->gsfTrack()->hitPattern().numberOfValidPixelHits();
            };
        };

        class DPhiInnerTrack_Seed : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DPhiInnerTrack_Seed() : ObjectProperty<ElectronAnalysis::Electron>("DPhiInnerTrack_Seed", "(#phi_{innermost} - #phi_{seed})", -0.2, 0.2, 400, "Electron", 16) {
            };

            virtual ~DPhiInnerTrack_Seed() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                double dphi_InSeed = 0; //t->DeltaPhi(Electron::_GSFTrack, Electron::_Seed);
                return (dphi_InSeed);
            };
        };

        template <int i = 0 > // in case of 0 uses dxy calculated by primary vertex, else 000
                class TIP_Significance : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            TIP_Significance() : ObjectProperty<ElectronAnalysis::Electron>("TIP_Significance", "TIP Significance", -10, 10, 200, "Electron", 17) {
                if (i == 0) {
                    this->PropertyName += "_PV";
                    this->PropertyTitle += "_{PV}";
                } else {
                    this->PropertyName += "_000";
                    this->PropertyTitle += "_{000}";
                    this->PropertyID = 1723;
                }
            };

            virtual ~TIP_Significance() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                if (i == 0)
                    return t->D0PV / t->D0E;
                else
                    return t->D0BS / t->D0E;
            };
        };

        class Skewness_ECALHits : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            Skewness_ECALHits() : ObjectProperty<ElectronAnalysis::Electron>("Skewness_ECALHits", "<(#phi_{hit}-#phi_{seed})^{3}>", -10, 10, 400, "Electron", 800) {
                //this->PropertyTitle = "#frac{(#sqrt{#sum_{#in(hits)}WWW})(#sum_{i#in(hits)}(WWW#times#phi_{i}-AVG)^{3})}{(#sum_{i#in(hits)}(WWW#times#phi_{i}-AVG)^{2})^{3#/2}}";
            };

            virtual ~Skewness_ECALHits() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return -100; //t->SuperClusterInfo.Calculate(ECalRecHitInfo::phi, ECalRecHitInfo::_LogEnergy, SimpleSuperCluster::Third_Corrected, ECalRecHitInfo::_LogEnergy, SimpleSuperCluster::_Seed_P, false);
            };
        };

        class NumberOfECALHits : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            NumberOfECALHits() : ObjectProperty<ElectronAnalysis::Electron>("NumberOfECALHits", "#SuperClusterHits", 0, 50, 50, "Electron", 20) {
            };

            virtual ~NumberOfECALHits() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElBasicClustersSize;
            };
        };

        class SC_Energy : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            SC_Energy() : ObjectProperty<ElectronAnalysis::Electron>("SC_Energy", "Energy_{sc}", 0, 500, 500, "Electron", 19) {
            };

            virtual ~SC_Energy() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElCaloEnergy;
            };
        };

        class DEtaSeedTrack : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DEtaSeedTrack() : ObjectProperty<ElectronAnalysis::Electron>("DEtaSeedTrack", "#eta_{seed} - #eta_{track}", -0.2, 0.2, 40, "Electron", 22) {
            };

            virtual ~DEtaSeedTrack() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElDeltaEtaSeedClusterAtCalo;
            };
        };

        class DPhiSeedTrack : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            DPhiSeedTrack() : ObjectProperty<ElectronAnalysis::Electron>("DPhiSeedTrack", "#phi_{seed} - #phi_{track}", -0.2, 0.2, 40, "Electron", 23) {
            };

            virtual ~DPhiSeedTrack() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElDeltaPhiSeedClusterAtCalo;
            };
        };

        class MomError : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            MomError() : ObjectProperty<ElectronAnalysis::Electron>("trackMomentumError", "Treack Momentum Error", 0, 50, 150, "Electron", 29) {
            };

            virtual ~MomError() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                return -1; //t->trackMomentumError();
            };
        };

        class RelativeMomError : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            RelativeMomError() : ObjectProperty<ElectronAnalysis::Electron>("RelativeMomentumError", "Relative Momentum Error", 0, 10, 500, "Electron", 2905) {
            };

            virtual ~RelativeMomError() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                //                if (t->ElPt != 0)
                //                    return (t->trackMomentumError() / (t->pt() * t->pt()));
                return -1;
            };
        };

        class RelativeMomError2 : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            RelativeMomError2() : ObjectProperty<ElectronAnalysis::Electron>("RelativeMomentumError2", "Relative Momentum Error2", 0, 10, 500, "Electron", 2906) {
            };

            virtual ~RelativeMomError2() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                //                if (t->pt() != 0)
                //                    return (t->electronMomentumError() / (t->pt() * t->pt()));
                return -1;
            };
        };

        class Momentum : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            Momentum() : ObjectProperty<ElectronAnalysis::Electron>("Momentum", "#||{#vec{P}_{track}}", 10, 500, 500, "Electron", 30) {
            };

            virtual ~Momentum() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                return t->ElTrkMomAtVtx;
                ;
            };
        };

        class AllHistograms : public Histograms<ElectronAnalysis::Electron> {
        public:

            AllHistograms(string name) : Histograms<ElectronAnalysis::Electron>(name),
            brem_exp1(1.),
            brem_exp2(2.) {
                AddHisto1(charge * (new DPhiSeedSuperCluster()));

                AddHisto1<DiffTrackMom_Electron > ();
                AddHisto1<DiffTrackPt_Electron > ();
                AddHisto1<BremFraction > ();
                AddHisto1<NChi2_Trk > ();
                AddHisto1<HoverE > ();
                AddHisto1<Pz > ();
                AddHisto1<Pt > ();
                AddHisto1<Eta > ();
                AddHisto1<CovEtaEta > ();

                _tip = AddHisto1<TIP < 0 > > ();
                _tip_00 = AddHisto1<TIP < 1 > > ();
                AddHisto1(charge * _tip);
                AddHisto1(charge * _tip_00);

                _tip_sig = AddHisto1<TIP_Significance < 0 > >();
                _tip_sig_00 = AddHisto1<TIP_Significance < 1 > >();
                AddHisto1(charge * _tip_sig);
                AddHisto1(charge * _tip_sig_00);

                AddHisto1<NGoodHits > ();
                AddHisto1<NPixelHits > ();

                _skew_hits = AddHisto1<Skewness_ECALHits > ();
                AddHisto1(charge * _skew_hits);

                ObjectProperty<ElectronAnalysis::Electron>* brem_skew1 = brem_exp1 * _skew_hits;
                brem_skew1->NumberOfBins = _skew_hits->NumberOfBins;
                ObjectProperty<ElectronAnalysis::Electron>* brem_skew2 = brem_exp2 * _skew_hits;
                brem_skew2->NumberOfBins = _skew_hits->NumberOfBins;
                AddHisto1(brem_skew1);
                AddHisto1(brem_skew2);
                AddHisto1(charge * brem_skew1);
                AddHisto1(charge * brem_skew2);

                AddHisto1(charge * (AddHisto1<DPhiSeedTrack > ()));
                AddHisto1(charge * (AddHisto1<DPhiInnerTrack_Seed > ()));

                MomError* momErr = AddHisto1<MomError > ();
                Momentum* mom = AddHisto1<Momentum > ();
                ObjectProperty<ElectronAnalysis::Electron> * relErrorP = *momErr / mom;
                relErrorP->MinValue = 0;
                relErrorP->MaxValue = 1.2;
                relErrorP->NumberOfBins = 120;
                AddHisto1(relErrorP);

                CaloEnergyError* enErr = AddHisto1<CaloEnergyError > ();
                Energy* en = AddHisto1<Energy > ();
                ObjectProperty<ElectronAnalysis::Electron> * relErrorEn = *enErr / en;
                relErrorEn->MinValue = 0;
                relErrorEn->MaxValue = 0.2;
                relErrorEn->NumberOfBins = 80;
                AddHisto1(relErrorEn);

                AddHisto1<DEtaSeedTrack > ();
            }

            ~AllHistograms() {
            };

        private:
            TIP < 0 > * _tip;
            TIP < 1 > * _tip_00;
            TIP_Significance < 0 > * _tip_sig;
            TIP_Significance < 1 > * _tip_sig_00;

            Skewness_ECALHits* _skew_hits;
            Charge charge;


            BremFraction brem;
            BremFraction_Exp brem_exp1;
            BremFraction_Exp brem_exp2;
        };

        class ChargeOK1 : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            ChargeOK1() : ObjectProperty<ElectronAnalysis::Electron>("DPhiInnerTrack_Seed_charge", "(#phi_{innermost} - #phi_{seed})", -2, 2, 4, "Electron", 1600) {
            };

            virtual ~ChargeOK1() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                //                double dphi_InSeed = t->DeltaPhi(Electron::_GSFTrack, Electron::_Seed);
                //                if (dphi_InSeed * t->charge() > 0)
                //                    return (1);
                //                else
                return -1;
            };
        };
    }
}

#include "ElecEffHandler.h"

#endif	/* _ELECTRONPROPERTIES_H */

