#ifndef DPHIANALYZER_H
#define	DPHIANALYZER_H

#include "NTupleAnalyzer/include/base/Analyzer.h"
#include "DataFormats/interface/DiLeptonTTBarEventProperties.h"
#include "TH1.h"
#include "TH3.h"
#include "TF1.h"


class DPhiAnalzer : public Analyzer<TopAnalysis::TTBarDileptonicEvent> {
public:
  typedef Analyzer<TopAnalysis::TTBarDileptonicEvent> BASE;


  TopAnalysis::DiLeptonTTBarEventProperties::LeptonsDPhi LeptonsDPhi;
  TopAnalysis::DiLeptonTTBarEventProperties::EventType EventTypeReader;
  std::vector<double> AcceptedEventTypes;
  bool ApplyWeights ;

  DPhiAnalzer(const edm::ParameterSet& ps)  : BASE(ps),
					      AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")),
					      ApplyWeights( ps.getParameter< bool >("ApplyWeights") ) {
    hElecSF = hLepSF(true);
    hMuonSF = hLepSF(false);
    hLEPSF_temp = hLepSF(false, "temp");  

    hDPhi = new TH1D( "hDPhi" , "Leptons #Delta#phi; #Delta#phi" , 14 , -3.15 , 3.15 );
  };

  virtual ~DPhiAnalzer(){};

  TH2* hElecSF;
  TH2* hMuonSF;
  TH2* hLEPSF_temp;


  bool Run(TopAnalysis::TTBarDileptonicEvent* ev){
    double evtType = EventTypeReader.ReadValue(ev);
    if (std::find(AcceptedEventTypes.begin(), AcceptedEventTypes.end(), evtType) == AcceptedEventTypes.end()) {
      return false;
    }
    
    double trg_sf_weight = 1.0;
    double evtLepSFWeight = 1.0;
    
    

    for(int id = 0 ; id < 2 ; id++){
      bool isElectron, isFirstLepton;
      float LeptonPt, LeptonEta;
      if(id == 0){
	isFirstLepton = true;
	isElectron = ev->FirstElectron->isElectron();
	LeptonPt = ev->FirstElectron->Pt;
	LeptonEta = ev->FirstElectron->Eta;
      }else{
	isFirstLepton = false;
	isElectron = ev->SecondElectron->isElectron();
	LeptonPt = ev->SecondElectron->Pt;
	LeptonEta = ev->SecondElectron->Eta;
      }
      if (isElectron) {
	if (isFirstLepton) {
	  if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
	    if (fabs(LeptonEta) < 1.5)
	      trg_sf_weight *= 0.9849;
	    else
	      trg_sf_weight *= 0.9774;
	  } else {
	    if (fabs(LeptonEta) < 1.5)
	      trg_sf_weight *= 0.9928;
	    else
	      trg_sf_weight *= 0.9938;
	  }
	} else {
	  if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
	    if (fabs(LeptonEta) < 1.5)
	      trg_sf_weight *= 0.9923;
	    else
	      trg_sf_weight *= 0.9953;
	  } else {
	    if (fabs(LeptonEta) < 1.5)
	      trg_sf_weight *= 0.9948;
	    else
	      trg_sf_weight *= 0.9956;
	  }
	}
      } else {
	if (isFirstLepton) {
	  if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
	    if (fabs(LeptonEta) < 0.8)
	      trg_sf_weight *= 0.9648;
	    else if (fabs(LeptonEta) < 1.2)
	      trg_sf_weight *= 0.9516;
	    else if (fabs(LeptonEta) < 2.1)
	      trg_sf_weight *= 0.9480;
	    else
	      trg_sf_weight *= 0.8757;
	  } else {
	    if (fabs(LeptonEta) < 0.8)
	      trg_sf_weight *= 0.9666;
	    else if (fabs(LeptonEta) < 1.2)
	      trg_sf_weight *= 0.9521;
	    else if (fabs(LeptonEta) < 2.1)
	      trg_sf_weight *= 0.9485;
	    else
	      trg_sf_weight *= 0.8772;
	  }
	} else {
	  if (20.0 <= LeptonPt && LeptonPt <= 30.0) {
	    if (fabs(LeptonEta) < 0.8)
	      trg_sf_weight *= 0.9655;
	    else if (fabs(LeptonEta) < 1.2)
	      trg_sf_weight *= 0.9535;
	    else if (fabs(LeptonEta) < 2.1)
	      trg_sf_weight *= 0.9558;
	    else
	      trg_sf_weight *= 0.9031;
	  } else {
	    if (fabs(LeptonEta) < 0.8)
	      trg_sf_weight *= 0.9670;
	    else if (fabs(LeptonEta) < 1.2)
	      trg_sf_weight *= 0.9537;
	    else if (fabs(LeptonEta) < 2.1)
	      trg_sf_weight *= 0.9530;
	    else
	      trg_sf_weight *= 0.8992;
	  }

	}
      }//the trg_sf_weight is known

      int bin_eta_pt = hLEPSF_temp->FindBin(LeptonPt, fabs(LeptonEta));
      if (isElectron)
	evtLepSFWeight *= hElecSF->GetBinContent(bin_eta_pt);
      else
	evtLepSFWeight *= hMuonSF->GetBinContent(bin_eta_pt);

    }

    
    double dphi = LeptonsDPhi.ReadValue( ev );
    double weight = ev->Weight * evtLepSFWeight * trg_sf_weight ;
    if(!ApplyWeights)
      weight = ev->Weight ;
    hDPhi->Fill( dphi , weight );
  };

  TH1D* hDPhi;
  virtual void End(){
    TFile* file = info::TheInfo->OutFile;
    file->cd();
    file->mkdir(this->Name.c_str())->cd();
    //    cout << this->Name << endl;
    hDPhi->Write();
  };
    
private:

  double SumOfErrors(double sys, double stat) {
    return sqrt(sys * sys + stat * stat);
  }

  TH2* hLepSF(bool electron, string appendix = "") {
    TH2* hret = new TH2D();

    string name___;
    if (electron)
      name___ = "hElecSF_" + appendix;
    else
      name___ = "hMuonSF_" + appendix;

    hret->SetNameTitle(name___.c_str(), name___.c_str());

    double pt_bins [] = {20, 22, 24, 26, 28, 30, 32, 34, 36, 40, 1000};
    double eta_bins[] = {0, 1.5, 10};
    hret->SetBins(10, pt_bins, 2, eta_bins);

    if (electron) {
      hret->SetBinContent(1, 1, 0.990);
      hret->SetBinError(1, 1, SumOfErrors(0.004, 0.001));

      hret->SetBinContent(2, 1, 0.999);
      hret->SetBinError(2, 1, SumOfErrors(0.003, 0.001));

      hret->SetBinContent(3, 1, 1.003);
      hret->SetBinError(3, 1, SumOfErrors(0.003, 0.002));

      hret->SetBinContent(4, 1, 0.997);
      hret->SetBinError(4, 1, SumOfErrors(0.002, 0.002));

      hret->SetBinContent(5, 1, 1.001);
      hret->SetBinError(5, 1, SumOfErrors(0.002, 0.000));

      hret->SetBinContent(6, 1, 0.999);
      hret->SetBinError(6, 1, SumOfErrors(0.001, 0.002));

      hret->SetBinContent(7, 1, 1.001);
      hret->SetBinError(7, 1, SumOfErrors(0.001, 0.000));

      hret->SetBinContent(8, 1, 0.998);
      hret->SetBinError(8, 1, SumOfErrors(0.001, 0.000));

      hret->SetBinContent(9, 1, 0.996);
      hret->SetBinError(9, 1, SumOfErrors(0.001, 0.000));

      hret->SetBinContent(10, 1, 0.993);
      hret->SetBinError(10, 1, SumOfErrors(0.000, 0.000));

      hret->SetBinContent(1, 2, 1.002);
      hret->SetBinError(1, 2, SumOfErrors(0.007, 0.005));

      hret->SetBinContent(2, 2, 1.018);
      hret->SetBinError(2, 2, SumOfErrors(0.006, 0.005));

      hret->SetBinContent(3, 2, 1.012);
      hret->SetBinError(3, 2, SumOfErrors(0.005, 0.006));

      hret->SetBinContent(4, 2, 1.006);
      hret->SetBinError(4, 2, SumOfErrors(0.002, 0.004));

      hret->SetBinContent(5, 2, 1.013);
      hret->SetBinError(5, 2, SumOfErrors(0.001, 0.004));

      hret->SetBinContent(6, 2, 1.003);
      hret->SetBinError(6, 2, SumOfErrors(0.002, 0.003));

      hret->SetBinContent(7, 2, 1.005);
      hret->SetBinError(7, 2, SumOfErrors(0.003, 0.002));

      hret->SetBinContent(8, 2, 1.005);
      hret->SetBinError(8, 2, SumOfErrors(0.003, 0.002));

      hret->SetBinContent(9, 2, 1.000);
      hret->SetBinError(9, 2, SumOfErrors(0.002, 0.001));

      hret->SetBinContent(10, 2, 0.997);
      hret->SetBinError(10, 2, SumOfErrors(0.001, 0.000));
    } else { //for muon
      hret->SetBinContent(1, 1, 0.993);
      hret->SetBinError(1, 1, SumOfErrors(0.002, 0.001));

      hret->SetBinContent(2, 1, 0.992);
      hret->SetBinError(2, 1, SumOfErrors(0.002, 0.001));

      hret->SetBinContent(3, 1, 0.996);
      hret->SetBinError(3, 1, SumOfErrors(0.002, 0.001));

      hret->SetBinContent(4, 1, 0.997);
      hret->SetBinError(4, 1, SumOfErrors(0.001, 0.001));

      hret->SetBinContent(5, 1, 0.997);
      hret->SetBinError(5, 1, SumOfErrors(0.001, 0.001));

      hret->SetBinContent(6, 1, 1.000);
      hret->SetBinError(6, 1, SumOfErrors(0.001, 0.000));

      hret->SetBinContent(7, 1, 0.999);
      hret->SetBinError(7, 1, SumOfErrors(0.001, 0.001));

      hret->SetBinContent(8, 1, 0.999);
      hret->SetBinError(8, 1, SumOfErrors(0.001, 0.000));

      hret->SetBinContent(9, 1, 0.999);
      hret->SetBinError(9, 1, SumOfErrors(0.001, 0.000));

      hret->SetBinContent(10, 1, 0.998);
      hret->SetBinError(10, 1, SumOfErrors(0.000, 0.000));

      hret->SetBinContent(1, 2, 1.001);
      hret->SetBinError(1, 2, SumOfErrors(0.003, 0.003));

      hret->SetBinContent(2, 2, 0.999);
      hret->SetBinError(2, 2, SumOfErrors(0.003, 0.004));

      hret->SetBinContent(3, 2, 1.004);
      hret->SetBinError(3, 2, SumOfErrors(0.002, 0.004));

      hret->SetBinContent(4, 2, 1.001);
      hret->SetBinError(4, 2, SumOfErrors(0.002, 0.005));

      hret->SetBinContent(5, 2, 1.001);
      hret->SetBinError(5, 2, SumOfErrors(0.002, 0.004));

      hret->SetBinContent(6, 2, 1.004);
      hret->SetBinError(6, 2, SumOfErrors(0.002, 0.004));

      hret->SetBinContent(7, 2, 1.005);
      hret->SetBinError(7, 2, SumOfErrors(0.003, 0.002));

      hret->SetBinContent(8, 2, 1.002);
      hret->SetBinError(8, 2, SumOfErrors(0.001, 0.002));

      hret->SetBinContent(9, 2, 1.002);
      hret->SetBinError(9, 2, SumOfErrors(0.001, 0.001));

      hret->SetBinContent(10, 2, 1.000);
      hret->SetBinError(10, 2, SumOfErrors(0.000, 0.000));
    }

    return hret;
  }
  

};

#endif
