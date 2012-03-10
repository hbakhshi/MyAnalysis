/* 
 * File:   DiElectronEvent.h
 * Author: hbakhshi
 *
 * Created on November 26, 2008, 10:41 AM
 */

#ifndef _DIELECTRONEVENT_H
#define	_DIELECTRONEVENT_H

#include "Math/VectorUtil.h"
#include "TLorentzVector.h"
#include "Electron.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/Vector.h"
//#include "DataFormats/VertexReco/interface/Vertex.h"
//#include "DataFormats/Common/interface/Ptr.h"
//#include "DataFormats/VertexReco/interface/VertexFwd.h"
//#include "DataFormats/TrackReco/interface/Track.h"

namespace ElectronAnalysis {

    template<class LeptonType>
    class DiLeptonEvent {
    public:

        DiLeptonEvent() {
        };

        LeptonType* FirstElectron;
        LeptonType* SecondElectron;

        short NJets;
        short NPhotons;
        short NMuons;
        short NTaus;

        short EventID;
        float Weight;

        math::XYZVectorD PFMET;

        unsigned int EventNumber;
        unsigned int RunID;
        //edm::Ptr<reco::Vertex> PrimaryVertex;
        bool IsZ2ee;

        float GetInvMass2(int i) const {
            return GetInvMass((Electron::Objects) i);
        }

        float GetInvMass(Electron::Objects mode) const {
            return (FirstElectron->get4Vector(mode) + SecondElectron->get4Vector(mode)).M();
        }

        float GetTransMass(Electron::Objects mode) const {
            return 1.;
        }

        bool IsClean() const {
            return true;
            return (NJets + NPhotons + NMuons + NTaus) == 0;
        }

        bool IsSameSign() const {
            return (FirstElectron->Charge == SecondElectron->Charge);
        };

        short IsSameSign(Electron::ChargeState cs, const BeamSpot* bs = NULL) const {
            short ch1 = FirstElectron->NewCharge(cs, bs);
            short ch2 = SecondElectron->NewCharge(cs, bs);

            if (ch1 == ch2 && ch1 != 0)
                return 1;
            else if (ch1 == 0 || ch2 == 0)
                return 0;
            else
                return -1;
        };

        short EventCharge() const {
            return (FirstElectron->Charge + SecondElectron->Charge);
        };

        short EventCharge_MC(bool stat1 = true) const {
            if (stat1)
                return (FirstElectron->GenCharge + SecondElectron->GenCharge);
            else
                return (FirstElectron->GenCharge + SecondElectron->GenCharge);
        };

        bool HasBadTrackMomentum() const {
            return (FirstElectron->IsBadTrackMomentum() && SecondElectron->IsBadTrackMomentum());
        };

        bool IsZ2ee_Reco() const {
            float invmass = GetInvMass(Electron::_SuperCluster);
            float deltaPhi = 1.; //FirstElectron->DeltaPhi(SecondElectron.get());
            deltaPhi += 0.0;
            return (invmass >= 60 && invmass <= 120);
        };

        short NWrongChargeElectrons(bool stat1 = true) const {
            short ret = 0;
            if (FirstElectron->IsBadCharge(stat1))
                ret++;
            if (SecondElectron->IsBadCharge(stat1))
                ret++;

            return ret;
        };

        float GetMomentumOfElectron(short state, Electron::Objects mode)const {
            float max = this->FirstElectron->get4Vector(mode).Pt();
            float min = this->SecondElectron->get4Vector(mode).Pt();

            float tmp;
            if (max < min) {
                tmp = min;
                min = max;
                max = tmp;
            }

            switch (state) {
                case 0:
                    return (max + min) / 2;
                case 1:
                    return max;
                case -1:
                    return min;
            }

            return -1;
        };

        bool IsSameFlavour() const {
            return (FirstElectron->isElectron() == SecondElectron->isElectron());
        }

        virtual void Print() {
            cout << "---" << this->RunID << ":" << this->EventNumber << "---" << endl;
            for (int ii = 0; ii < 2; ii++) {
                LeptonType* lep;

                if (ii == 0) {
                    lep = FirstElectron;
                    cout << "First ";
                } else if (ii == 1) {
                    lep = SecondElectron;
                    cout << "Second ";
                }

                cout << (lep->isMuon() ? "Mu :" : "El :");

                cout << "Pt" << lep->Pt;
                cout << "-Eta" << lep->Eta;
                cout << "-Phi" << lep->Phi;
                cout << "-Charge" << lep->Charge;

                cout << endl;
            }

            cout << "Njets" << NJets;
            cout << "-PFMet" << PFMET.Rho();
            cout << "-InvMass" << this->GetInvMass(Electron::_Momentum);

            cout << endl;
            cout << "------------------" << endl;
        };
    };

    class DiElectronEvent : public DiLeptonEvent<Electron> {
    public:

        double M_T() const;

        Electron FirstElectron_;
        Electron SecondElectron_;

        math::XYZVectorD JetsSumPt;

        DiElectronEvent();
        //DiElectronEvent(edm::Handle< std::vector<Electron> >, int, int);

        bool passNewCuts(double d0_cut, ElectronAnalysis::Electron::ChargeState chs = ElectronAnalysis::Electron::ElectronCharge,
                bool check_trk_charge = true, bool check_dphi_charge = true, int check_conversion = 0) const;

        short AreTheZElectronsReconstructedWell()const;
    };

    class DiLepton : public DiLeptonEvent<Lepton> {
    public:
        DiLepton();

        vector<math::XYZTLorentzVector> Jets;
        vector<int> BJets;
        vector<double> Jets_TrackCountingHighEff;
        vector<double> Jets_simpleSecondaryVertexHighEff;
        math::XYZVectorD CaloMET;

        virtual void Print();
    };
}

#endif	/* _DIELECTRONEVENT_H */

