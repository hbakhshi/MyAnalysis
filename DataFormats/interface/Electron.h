#ifndef _ELECTRON_H
#define	_ELECTRON_H

//#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
//#include "DataFormats/PatCandidates/interface/Electron.h"
//#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
//#include "DataFormats/EgammaReco/interface/ClusterShape.h"
//#include "ECalRecHitInfo.h"
//#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/HitPattern.h"
//#include "DataFormats/Common/interface/Ref.h"
//#include "DataFormats/TrackReco/interface/Track.h"
//#include "MyTrack.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include <vector>
#include <math.h>

using namespace std;
using namespace reco;


#include "Lepton.h"

namespace ElectronAnalysis {
    //typedef pat::Electron BaseElectron;

    class BaseElectron : public Lepton {
    public:
        int ElGood; //[NEles]
        int ElIsIso; //[NEles]
        int ElChargeMisIDProb; //[NEles]
        float ElPtE; //[NEles]
        float ElEt; //[NEles]
        float ElSCEta; //[NEles]

        float ElGsfTkPt; //[NEles]
        float ElGsfTkEta; //[NEles]
        float ElGsfTkPhi; //[NEles]
        float ElTrkMomentumError; //[NEles]
        float ElEcalEnergyError; //[NEles]
        float ElEleMomentumError; //[NEles]
        int ElNBrems; //[NEles]

        float ElRelIso03; //[NEles]
        float ElRelIso04; //[NEles]
        float ElDR03TkSumPt; //[NEles]
        float ElDR04TkSumPt; //[NEles]
        float ElDR03EcalRecHitSumEt; //[NEles]
        float ElDR04EcalRecHitSumEt; //[NEles]
        float ElDR03HcalTowerSumEt; //[NEles]
        float ElDR04HcalTowerSumEt; //[NEles]
        float ElNChi2; //[NEles]
        
        int ElCInfoIsGsfCtfCons; //[NEles]
        int ElCInfoIsGsfCtfScPixCons; //[NEles]
        int ElCInfoIsGsfScPixCons; //[NEles]
        int ElScPixCharge; //[NEles]
        float ElClosestCtfTrackPt; //[NEles]
        float ElClosestCtfTrackEta; //[NEles]
        float ElClosestCtfTrackPhi; //[NEles]
        int ElClosestCtfTrackCharge; //[NEles]
        float ElIDMva; //[NEles]
        int ElIDTight; //[NEles]
        int ElIDLoose; //[NEles]
        int ElIDRobustTight; //[NEles]
        int ElIDRobustLoose; //[NEles]
        int ElIDsimpleWPrelIso; //[NEles]
        int ElIDsimpleWP80relIso; //[NEles]
        int ElIDsimpleWP85relIso; //[NEles]
        int ElIDsimpleWP90relIso; //[NEles]
        int ElIDsimpleWP95relIso; //[NEles]
        int ElInGap; //[NEles]
        int ElEcalDriven; //[NEles]
        int ElTrackerDriven; //[NEles]
        int ElBasicClustersSize; //[NEles]
        float Elfbrem; //[NEles]
        float ElHcalOverEcal; //[NEles]
        float ElE1x5; //[NEles]
        float ElE5x5; //[NEles]
        float ElE2x5Max; //[NEles]
        float ElSigmaIetaIeta; //[NEles]
        float ElDeltaPhiSeedClusterAtCalo; //[NEles]
        float ElDeltaEtaSeedClusterAtCalo; //[NEles]
        float ElDeltaPhiSuperClusterAtVtx; //[NEles]
        float ElDeltaEtaSuperClusterAtVtx; //[NEles]
        float ElCaloEnergy; //[NEles]
        float ElTrkMomAtVtx; //[NEles]
        float ElESuperClusterOverP; //[NEles]
        int ElNumberOfMissingInnerHits; //[NEles]
        int ElSCindex; //[NEles]
        float ElConvPartnerTrkDist; //[NEles]
        float ElConvPartnerTrkDCot; //[NEles]
        float ElConvPartnerTrkPt; //[NEles]
        float ElConvPartnerTrkEta; //[NEles]
        float ElConvPartnerTrkPhi; //[NEles]
        float ElConvPartnerTrkCharge; //[NEles]
        int ElScSeedSeverity; //[NEles]
        float ElE1OverE9; //[NEles]
        float ElS4OverS1; //[NEles]

    };

    class Electron : public BaseElectron {
    public:

        Electron() {
        };
        Electron(const BaseElectron&);

        virtual bool isElectron() {
            return true;
        };

        virtual ~Electron() {
        };

        enum Objects {
            _Momentum = 0,
            //            _GSFTrack = 1,
            //            _GSFTrack_OUT = 2,
            _SuperCluster = 3,
            //            _Seed = 4,
            _NearestTrack = 5,
            _BestMatch1 = 6,
            _BestMatch3 = 7
        };

        virtual math::XYZTLorentzVector get4Vector(int mode_) const {
            Objects mode = (Objects) mode_;
            math::RhoEtaPhiVector tmp;
            math::XYZTLorentzVector ret;
            switch (mode) {
                case _Momentum:
                    ret.SetXYZT(Px, Py, Pz, E);
                    break;
                    //                case _GSFTrack:
                    //                    ret.SetXYZT(trackMomentumAtVtx().x(), trackMomentumAtVtx().y(), trackMomentumAtVtx().z(), trackMomentumAtVtx().r());
                    //                    break;
                    //                case _GSFTrack_OUT:
                    //                    ret.SetXYZT(trackMomentumOut().x(), trackMomentumOut().y(), trackMomentumOut().z(), trackMomentumOut().r());
                    //                    break;
                case _SuperCluster:
                    ret.SetXYZT(ElCaloEnergy * sin(Theta) * cos(Phi), ElCaloEnergy * sin(Theta) * sin(Phi), ElCaloEnergy * cos(Theta), ElCaloEnergy);
                    break;
                    //                case _Seed:
                    //                    ret.SetXYZT(SuperClusterInfo.Seed_Energy * sin(SuperClusterInfo.Seed_position.theta()) * cos(SuperClusterInfo.Seed_position.phi()), SuperClusterInfo.Seed_Energy * sin(SuperClusterInfo.Seed_position.theta()) * sin(SuperClusterInfo.Seed_position.phi()), SuperClusterInfo.Seed_Energy * cos(SuperClusterInfo.Seed_position.theta()), SuperClusterInfo.Seed_Energy);
                    //                    break;
                case _NearestTrack:
                    tmp.SetCoordinates(ElClosestCtfTrackPt, ElClosestCtfTrackEta, ElClosestCtfTrackPhi);
                    ret.SetXYZT(tmp.X(), tmp.Y(), tmp.Z(), tmp.R());
                    break;
                case _BestMatch1:
                case _BestMatch3:
                    tmp.SetCoordinates(GenPt, GenEta, GenPhi);
                    ret.SetXYZT(tmp.X(), tmp.Y(), tmp.Z(), GenE);
                    break;

            }
            return ret;
        };

        enum ChargeState {
            ElectronCharge = 101,
            GSF = 102,
            CTF = 103,
            SC = 104,
            CommonValue = 105,
            GSF_CTF = 106,
            GSF_SC = 107,
            CTF_SC = 108,
            All = 109,
            GSF_CTF1 = 110
        };

        short NewCharge(ChargeState, const reco::BeamSpot* bs = NULL) const;

        bool IsBadCharge(bool stat1 = true) const {
            if (stat1)
                return (this->NewCharge(GSF_CTF) == GenCharge);
            else
                return (this->NewCharge(GSF_CTF) == GenCharge);
        };

        bool IsBadTrackMomentum() const {
            return (fabs(PError(_Momentum, _BestMatch1)) > 5);
        };

        float PError(Objects errorOfWhat, Objects whichIsTrue) const {
            return (get4Vector(errorOfWhat).P() - get4Vector(whichIsTrue).P()) / get4Vector(whichIsTrue).P();
        };

        float PtError(Objects errorOfWhat, Objects whichIsTrue) const {
            return (get4Vector(errorOfWhat).Pt() - get4Vector(whichIsTrue).Pt()) / get4Vector(whichIsTrue).Pt();
        };

        float EError(Objects errorOfWhat, Objects whichIsTrue) const {
            return (get4Vector(errorOfWhat).energy() - get4Vector(whichIsTrue).energy()) / get4Vector(whichIsTrue).energy();
        };

        float MassError(Objects errorOfWhat, Objects whichIsTrue) const {
            return (get4Vector(errorOfWhat).M() - get4Vector(whichIsTrue).M()) / get4Vector(whichIsTrue).M();
        };

        float DeltaPhi(Objects What1, Objects What2) const {
            float ret = get4Vector(What2).Phi() - get4Vector(What1).Phi();
            if (ret > M_PI)
                ret = (2 * M_PI - ret);
            else if (ret < -M_PI)
                ret = (2 * M_PI + ret);

            return ret;
        };

        float DeltaPhi(const Electron* e, Objects What = _Momentum) const {
            float ret = e->get4Vector(What).Phi() - get4Vector(What).Phi();
            if (ret > M_PI)
                ret = (2 * M_PI - ret);
            else if (ret < -M_PI)
                ret = (2 * M_PI + ret);

            return ret;
        };

        float DeltaEta(Objects What1, Objects What2)const {
            return get4Vector(What1).Eta() - get4Vector(What2).Eta();
        };

        float DeltaR(Objects What1, Objects What2)const {
            float deta = DeltaEta(What1, What2);
            float dphi = DeltaPhi(What1, What2);

            return sqrt(deta * deta + dphi * dphi);
        };

        //just to mimic Ptr class, to stay consistent with older versions

        const Electron* get() const {
            return this;
        }
    };

    typedef vector<Electron> ElectronCollection;
}
#endif

