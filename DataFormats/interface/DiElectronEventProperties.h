/* 
 * File:   DiElectronEventProperties.h
 * Author: hbakhshi
 *
 * Created on December 5, 2008, 3:42 PM
 */

#ifndef _DIELECTRONEVENTPROPERTIES_H
#define	_DIELECTRONEVENTPROPERTIES_H

#include "BaseAnalysis/interface/ObjectProperty.h"
#include "BaseAnalysis/interface/Histograms.h"
#include "BaseAnalysis/interface/TH1Ext.h"
#include "DiElectronEvent.h"
#include "ElectronProperties.h"
#include "Electron.h"
#include <math.h>
#include <limits>

#include <string>
#include <TSystem.h>
#include <TH1D.h>
#include <map>
#include "ElectronProperties.h"
#include "BaseAnalysis/interface/EfficiencyHandler.h"

#define NAN_2 numeric_limits<double>::infinity()

using namespace std;

namespace ElectronAnalysis {
    namespace DiElectronEventProperties {

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

        template<Electron::Objects what = Electron::_Momentum>
                class InvariantMass : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            InvariantMass() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("InvariantMass", "Invariant Mass", 0, 200, 200, "DiElectronEvent", 500 + int(what)) {
                string s = "";
                string s2 = "";
                switch (what) {
                    case Electron::_Momentum:
                        s = "Electrons ";
                        s2 = "Electrons_";
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

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent* t) const {
                return t->GetInvMass(what);
            };
        };

        class Z2EE : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            Z2EE() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("IsZ2EE", "Is (z#rightarrow e^{+}e^{-})", 0, 2, 2, "DiElectronEvent", 2) {
            };

            virtual ~Z2EE() {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent* t) const {
                return t->IsZ2ee;
            };
        };

        class CSA07ID : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            CSA07ID() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("CSA07ID", "CSA07 Process ID", 0, 50, 50, "DiElectronEvent", 3) {
            };

            virtual ~CSA07ID() {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent* t) const {
                return t->EventNumber;
            };
        };

        class RelativeMassError : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            RelativeMassError() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("RelativeMassError", "RelativeMassError", 0, 0.5, 500, "DiElectronEvent", 5) {
            };

            virtual ~RelativeMassError() {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent* t) const {
                cout << this->PropertyName << " is not implemented" << endl;
                double error1 = 0; //t->FirstElectron->ecalEnergyError() / t->FirstElectron->ecalEnergy();
                double error2 = 0; //t->SecondElectron->ecalEnergyError() / t->SecondElectron->ecalEnergy();
                return (error1 + error2) / 2;
            };
        };

        class NumberOfJets : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            NumberOfJets() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("NumberOfJets", "# Jets", 0, 15, 15, "DiElectronEvent", 2115) {
            };

            virtual ~NumberOfJets() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent * t) const {
                return t->NJets;
            };
        };

        class NumberOfMuons : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            NumberOfMuons() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("NumberOfMuons", "# Muons", 0, 15, 15, "DiElectronEvent", 2195) {
            };

            virtual ~NumberOfMuons() {
            };

            virtual void SetAxis(TAxis * axis) {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent * t) const {
                return t->NMuons;
            };
        };

        template<int iii> // 0 : just jets. 1 : +leptnos. 2 : +leptons +PFMet
        class HT : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            HT() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("HT", "HT", 0, 200, 50, "DiElectronEvent", 2116) {
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

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent * t) const {
                double ret = 0;

                ret += t->JetsSumPt.Rho();

                if (iii > 0) {
                    ret += t->FirstElectron->Pt;
                    ret += t->SecondElectron->Pt;
                }

                if (iii == 2)
                    ret += t->PFMET.Rho();

                return ret;
            };
        };

        template<int iii> // 0 : just jets. 1 : +leptnos. 2 : +leptons +PFMet
        class MHT : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            MHT() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("MHT", "MHT", 20, 620, 60, "DiElectronEvent", 2120) {
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

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent * t) const {
                double retX = 0;
                double retY = 0;
                retX += t->JetsSumPt.x();
                retY += t->JetsSumPt.y();


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

        class PFMet : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            PFMet() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("PFMET", "PFMET", 0, 300, 60, "DiElectronEvent", 2125) {

            };

            ~PFMet() {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent * t) const {
                return t->PFMET.rho();
            }
        };

        class M_T : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            M_T() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("M_T", "Transverse mass", 0, 200, 50, "DiElectronEvent", 2126) {

            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent * t) const {
                return t->M_T();
            }
        };

        class ElectronSelector {
        public:

            ElectronSelector() {
            };

            virtual ~ElectronSelector() {
            };

            double ReadValue(const ElectronAnalysis::DiElectronEvent*, ObjectProperty<ElectronAnalysis::Electron>*) {
                return nan("NAN");
            };

            string Pre() {
                return "";
            };

            string Pre2() {
                return "";
            };

        };

        template<int i> // -1 : min , 0 : avetage , 1 : max,2 : the value if they are in the same bin and NAN if they are not
        class EleReader : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:
            ElectronSelector* Selector;

            EleReader(ObjectProperty<ElectronAnalysis::Electron>* _reader, ElectronSelector* selector = NULL)
            : ObjectProperty<ElectronAnalysis::DiElectronEvent>("", " of electrons", 0, 0, 0, "DiElectronEvent", 0),
            Selector(selector),
            reader(_reader) {
                this->PropertyName = reader->PropertyName;
                this->PropertyID = 1000 + 3 * reader->PropertyID + i;
                string pre(reader->PropertyTitle);
                string pre2 = "";
                switch (i) {
                    case 0:
                        pre += "_{average}";
                        pre2 = "avg_";
                        break;
                    case 1:
                        pre += "_{max}";
                        pre2 = "max_";
                        break;
                    case -1:
                        pre += "_{min}";
                        pre2 = "min_";
                        break;
                    case 2:
                        pre += "^{common}";
                        pre2 = "Common_";
                        break;
                    default:
                        if (Selector == NULL) {
                            cout << "EleReader says : Selector must be set" << endl;
                            throw new std::exception();
                        }
                        pre += Selector->Pre();
                        pre2 = Selector->Pre2();
                        break;
                }
                this->PropertyTitle = pre + this->PropertyTitle;
                this->PropertyName = pre2 + reader->PropertyName;

                this->MaxValue = reader->MaxValue;
                this->MinValue = reader->MinValue;
                this->NumberOfBins = reader->NumberOfBins;
            };

            virtual ~EleReader() {
            };

            ObjectProperty<ElectronAnalysis::Electron>* reader;

            virtual TH1Ext<ElectronAnalysis::DiElectronEvent>* GetH1(string name, string title = "") {
                TH1Ext<ElectronAnalysis::DiElectronEvent>* ret = ObjectProperty<ElectronAnalysis::DiElectronEvent>::GetH1(name, title);
                TH1Ext<ElectronAnalysis::Electron>* sample = this->reader->GetH1(name, title);

                int nBins = sample->GetNbinsX();
                double bins[1000];
                for (int kkk = 0; kkk < nBins; kkk++)
                    bins[kkk] = sample->GetBinLowEdge(kkk + 1);
                bins[nBins] = bins[nBins - 1] + sample->GetBinWidth(nBins + 1);
                ret->SetBins(nBins, bins);

                return ret;
            }

            virtual TH2Ext<ElectronAnalysis::DiElectronEvent>* GetH2(ObjectProperty<ElectronAnalysis::DiElectronEvent>* otherProperty, string name, string title = "") {
                TH2Ext<ElectronAnalysis::DiElectronEvent>* ret = ObjectProperty<ElectronAnalysis::DiElectronEvent>::GetH2(otherProperty, name, title);
                TH1Ext<ElectronAnalysis::Electron>* sample = this->reader->GetH1(name, title);

                int nBins = sample->GetNbinsX();
                double bins[1000];
                for (int kkk = 0; kkk < nBins; kkk++)
                    bins[kkk] = sample->GetBinLowEdge(kkk + 1);
                bins[nBins] = bins[nBins - 1] + sample->GetBinWidth(nBins + 1);

                double bins_Y[1000];
                int binY_iii = 0;
                for (double bin_low_edge = otherProperty->MinValue;
                        bin_low_edge <= otherProperty->MaxValue;
                        bin_low_edge += (otherProperty->MaxValue - otherProperty->MinValue) / otherProperty->NumberOfBins, binY_iii++)
                    bins_Y[binY_iii] = bin_low_edge;
                
                ret->SetBins(nBins, bins , otherProperty->NumberOfBins, bins_Y);

                return ret;
            }

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent* t) const {
                float max = reader->ReadValue(t->FirstElectron_.get());
                float min = reader->ReadValue(t->SecondElectron_.get());

                float tmp;
                if (max < min) {
                    tmp = min;
                    min = max;
                    max = tmp;
                }

                switch (i) {
                    case 0:
                        return (max + min) / 2;
                    case 1:
                        return max;
                    case -1:
                        return min;
                    case 2:
                    {
                        double binL = (this->MaxValue - this->MinValue) / double(this->NumberOfBins);

                        double binMax;
                        modf((max - this->MinValue) / binL, &binMax);

                        double binMin;
                        modf((min - this->MinValue) / binL, &binMin);

                        double diff = fabs(binMax - binMin);

                        //cout << binL << " "<< max << "(" << binMax << ")  " << min << "(" << binMin << ")  " << diff << "   " << this->MaxValue << "   " << this->MinValue <<  endl;

                        if (diff >= 1)
                            return NAN_2; //this->MinValue - 100000. ;
                        else
                            return max;
                    }
                    default:
                        if (Selector != NULL)
                            return Selector->ReadValue(t, reader);
                }

                return -1;
            };
        };

        template<int i>
        class PtOFElectron : public EleReader<i> {
        public:

            PtOFElectron() : EleReader<i>(new ElectronAnalysis::ElectronProperties::Pt()) {
                this->NumberOfBins = 32;
                this->MaxValue = 170;
                this->MinValue = 10;
            };

            ~PtOFElectron() {
            };
        };

        template<int i>
        class EtaOFElectron : public EleReader<i> {
        public:

            typedef EleReader<i> BASE;

            EtaOFElectron(bool EndcapBarrel = false) : EleReader<i>(new ElectronAnalysis::ElectronProperties::Eta(EndcapBarrel)) {
            };

            ~EtaOFElectron() {
            };
        };

        template<int i>
        class BremOFElectron : public EleReader<i> {
        public:

            BremOFElectron() : EleReader<i>(new ElectronAnalysis::ElectronProperties::BremFraction()) {
                this->NumberOfBins = 20;
            };

            ~BremOFElectron() {
            };
        };

        template<int i>
        class CaloErrorOFElectron : public EleReader<i> {
        public:

            CaloErrorOFElectron() : EleReader<i>(new ElectronAnalysis::ElectronProperties::CaloEnergyError()) {
                this->NumberOfBins = 20;
            };

            ~CaloErrorOFElectron() {
            };
        };

        template<int i, int j = 0 >
                class TIPOFElectron : public EleReader<i> {
        public:

            TIPOFElectron() : EleReader<i>(abs(new ElectronAnalysis::ElectronProperties::TIP<j>())) {
                this->NumberOfBins = 10;
            };

            ~TIPOFElectron() {
            };
        };

        template<int i >
        class MomErrorOFElectron : public EleReader<i> {
        public:

            MomErrorOFElectron() : EleReader<i>(new ElectronAnalysis::ElectronProperties::RelativeMomError()) {
                this->NumberOfBins = 10;
            };

            ~MomErrorOFElectron() {
            };
        };

        template<int i >
        class MomErrorOFElectron2 : public EleReader<i> {
        public:

            MomErrorOFElectron2() : EleReader<i>(new ElectronAnalysis::ElectronProperties::RelativeMomError2()) {
                this->NumberOfBins = 10;
            };

            ~MomErrorOFElectron2() {
            };
        };

        template<int i >
        class ChargeOK1 : public EleReader<i> {
        public:

            ChargeOK1() : EleReader<i>(new ElectronAnalysis::ElectronProperties::ChargeOK1()) {
            };

            ~ChargeOK1() {
            };
        };

        class DMOverM_SC : public ObjectProperty<ElectronAnalysis::DiElectronEvent> {
        public:

            DMOverM_SC() : ObjectProperty<ElectronAnalysis::DiElectronEvent>("DMOverM_SC", "", 0, 2, 2, "DiElectronEvent", 2) {
            };

            virtual ~DMOverM_SC() {
            };

            virtual double ReadValue(const ElectronAnalysis::DiElectronEvent* t) const {
                return t->IsZ2ee;
            };
        };

        class AllHistograms : public Histograms<ElectronAnalysis::DiElectronEvent> {
        public:

            AllHistograms(string name) : Histograms<ElectronAnalysis::DiElectronEvent>(name) {
                //                AddHisto1 <Z2EE > ();
                AddHisto1 <InvariantMass<Electron::_Momentum> > ();
                AddHisto1 <InvariantMass<Electron::_SuperCluster> > ();
                //                AddHisto1 <InvariantMass<Electron::_GSFTrack> > ();
                AddHisto1 <InvariantMass<Electron::_NearestTrack> > ();
                //                AddHisto1 <InvariantMass<Electron::_BestMatch1> > ();
            };

            ~AllHistograms() {
            };
        };
    }
}

#include "DiElecEffHandler.h"

#endif	/* _DIELECTRONEVENTPROPERTIES_H */

