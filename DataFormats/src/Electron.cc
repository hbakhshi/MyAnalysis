#include "DataFormats/interface/Electron.h"
//#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
//#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
//#include <DataFormats/EgammaCandidates/interface/GsfElectron.h>
//#include <DataFormats/BeamSpot/interface/BeamSpot.h>

using namespace std;
using namespace reco;

ElectronAnalysis::Electron::Electron(const BaseElectron& e) : BaseElectron(e)/*,
							     nonGSFTrack( e.closestCtfTrackRef().get() ),
							     hitPattern(e.gsfTrack()->hitPattern())*/ {

    //    if(e.closestCtfTrackRef().isAvailable() && e.closestCtfTrackRef().isNonnull())
    //        nonGSFTrack.setTrack( e.closestCtfTrackRef().get() );
    // this->chi2 = e.gsfTrack()->chi2();
    // this->ndof = (short)e.gsfTrack()->ndof();
    // this->qOverP = e.gsfTrack()->qoverp();
    // this->qOverpError = e.gsfTrack()->qoverpError();
    // this->dxy = e.gsfTrack()->dxy();
    // this->dxyError = e.gsfTrack()->dxyError();

    // this->ambiguousGsfTracks_size = e.ambiguousGsfTracksSize();

    // this->isEcalDriven_ = e.isEcalDriven();
    // this->isTrackerDriven_ = e.isTrackerDriven();

    // edm::LogInfo("ElectronAnalysis_ELECTRON") << "a new electron has been created";
}

short ElectronAnalysis::Electron::NewCharge(ChargeState cs, const BeamSpot* bs) const {
    using namespace ElectronAnalysis;
    if (cs == Electron::ElectronCharge)
        return this->Charge;

    short gsfCharge = 0;

    short ctfCharge = 0;

    short scsCharge = 0;

    if (this->ElCInfoIsGsfCtfScPixCons)
        gsfCharge = ctfCharge = scsCharge = Charge;
    else if (this->ElCInfoIsGsfCtfCons) {
        gsfCharge = ctfCharge = Charge;
        scsCharge = -1 * Charge;
    } else if (this->ElCInfoIsGsfScPixCons) {
        gsfCharge = scsCharge = Charge;
        ctfCharge = -1 * Charge;
    } else {
        ctfCharge = scsCharge = Charge;
        gsfCharge = -1 * Charge;
    }

    if(this->ElClosestCtfTrackCharge < -1)
        ctfCharge = 0;

    switch (cs) {
        case Electron::ElectronCharge:
            return this->Charge;
        case Electron::GSF:
            return gsfCharge;
        case Electron::CTF:
            return ctfCharge;
        case Electron::SC:
            return scsCharge;
        case Electron::CommonValue:
            if (gsfCharge == scsCharge || ctfCharge == 0 || scsCharge == 0)
                return gsfCharge;
            else
                return ctfCharge;

        case Electron::GSF_CTF:
            if (gsfCharge == ctfCharge && ctfCharge != 0)
                return gsfCharge;
            else
                return 0;

        case Electron::GSF_SC:
            if (gsfCharge == scsCharge && scsCharge != 0)
                return gsfCharge;
            else
                return 0;

        case Electron::CTF_SC:
            if (scsCharge == ctfCharge)
                return ctfCharge;
            else
                return 0;
        case Electron::All:
            if (this->ElCInfoIsGsfCtfScPixCons) //(scsCharge == ctfCharge && ctfCharge == gsfCharge)
                return this->Charge;
            else
                return 0;
        case Electron::GSF_CTF1:
            if (gsfCharge == ctfCharge)
                return ctfCharge;
            else
                return gsfCharge;
    }
    return -4;
}

