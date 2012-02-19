//#include "DataFormats/VertexReco/interface/Vertex.h"
#include <DataFormats/interface/DiElectronEvent.h>
#include <complex>

using namespace ElectronAnalysis;

/*DiElectronEvent::DiElectronEvent(edm::Handle< std::vector<Electron> > handle, int first, int second) :
  FirstElectron(handle,first),
  SecondElectron(handle,second),
  FirstSharedHits(-1.),
  SecondSharedHits(-1.) {
    this->FirstElectron = &(this->FirstElectron_);
    this->SecondElectron = &(this->SecondElectron_);
    }*/

DiElectronEvent::DiElectronEvent() {
    this->FirstElectron = &(this->FirstElectron_);
    this->SecondElectron = &(this->SecondElectron_);
}

// check_conversion : 0 = NO. 1 = just geometrical. 2 = Geometrical & NMissingHits < 2. 3 = Geometrical & NMissingHits < 1. 4 : NMissingHits < 2. 5 : NMissingHits < 1

bool DiElectronEvent::passNewCuts(double d0_cut, ElectronAnalysis::Electron::ChargeState chs, bool check_trk_charge, bool check_dphi_charge, int check_conversion) const {
    bool d0_1 = fabs(FirstElectron->D0BS) < d0_cut;
    bool d0_2 = fabs(SecondElectron->D0BS) < d0_cut;


    bool ch_1 = (FirstElectron->NewCharge(chs) != 0);
    bool ch_2 = (SecondElectron->NewCharge(chs) != 0);
    //cout << d0_cut << " " << FirstElectron->dxy_IP << "(" << d0_1 << ") " << SecondElectron->dxy_IP << "(" << d0_2 << ")-";

    bool trk1 = true;
    bool trk2 = true;

    if (check_trk_charge) {
        trk1 = (FirstElectron->NewCharge(Electron::CTF) == FirstElectron->NewCharge(chs));
        trk2 = (SecondElectron->NewCharge(Electron::CTF) == SecondElectron->NewCharge(chs));
    }

    //cout << trk1 << "-" << trk2 <<  "- returns: " << (d0_1 && d0_2 && trk2 && trk1) << endl;

    bool dphi1 = true;
    bool dphi2 = true;

    if (check_dphi_charge) {
        dphi1 = (FirstElectron->NewCharge(Electron::SC) == FirstElectron->NewCharge(chs));
        dphi2 = (SecondElectron->NewCharge(Electron::SC) == SecondElectron->NewCharge(chs));
    }

    bool conv_1 = true;
    bool conv_2 = true;

    if (check_conversion != 0) {
        bool conv_1_geo = FirstElectron->ElIDsimpleWP80relIso < 4;
        bool conv_1_hit1 = FirstElectron->ElNumberOfMissingInnerHits > 1;
        bool conv_1_hit0 = FirstElectron->ElNumberOfMissingInnerHits > 0;

        bool conv_2_geo = SecondElectron->ElIDsimpleWP80relIso < 4;
        bool conv_2_hit1 = SecondElectron->ElNumberOfMissingInnerHits > 1;
        bool conv_2_hit0 = SecondElectron->ElNumberOfMissingInnerHits > 0;

        switch (check_conversion) {
            case 1:
                conv_1 = !conv_1_geo;
                conv_2 = !conv_2_geo;
                break;
            case 2:
                conv_1 = !(conv_1_geo || conv_1_hit1);
                conv_2 = !(conv_2_geo || conv_2_hit1);
                break;
            case 3:
                conv_1 = !(conv_1_geo || conv_1_hit0);
                conv_2 = !(conv_2_geo || conv_2_hit0);
                break;
            case 4:
                conv_1 = !(conv_1_hit1);
                conv_2 = !(conv_2_hit1);
                break;
            case 5:
                conv_1 = !(conv_1_hit0);
                conv_2 = !(conv_2_hit0);
                break;
        }
    }

    return (d0_1 && d0_2 && ch_1 && ch_2 && trk2 && trk1 && dphi2 && dphi1 && conv_1 && conv_2);
}

short DiElectronEvent::AreTheZElectronsReconstructedWell()const {
    //cout << "TEST" << endl;
    return ( fabs(FirstElectron->GenID) == 11 && fabs(SecondElectron->GenID) == 11);
}

DiLepton::DiLepton() {
    this->EventID = 1;
}

void DiLepton::Print() {
    cout << "-----------------------------------------------------------------------------" << endl;

    DiLeptonEvent<Lepton>::Print();

    for(int i=0; i< NJets ; i++)
        cout << "J" << i << "pt" << Jets[i].Rho() << "-" ;

    cout << "-----------------------------------------------------------------------------" << endl;
}

double DiElectronEvent::M_T() const {
    //    double mt_jets = 2 * this->PFMET.Rho() ;
    //    mt_jets *= this->JetsSumPt.Rho() ;
    //    mt_jets *= (1.0 - cos( JetsSumPt.phi() - PFMET.phi() ));

    double mt_ele1 = 2 * this->PFMET.Rho();
    mt_ele1 *= this->FirstElectron_.Pt;
    mt_ele1 *= (1.0 - cos(FirstElectron_.Phi - PFMET.phi()));

    //    double mt_ele2 = 2 * this->FirstElectron_.Pt ;
    //    mt_ele2 *= this->SecondElectron_.Pt;
    //    mt_ele2 *= (1.0 - cos( SecondElectron_.Phi - FirstElectron_.Phi ));
    //
    //    double ret = max( mt_jets , max(mt_ele1, mt_ele2) );

    double ret = mt_ele1;
    return sqrt(ret);
}

