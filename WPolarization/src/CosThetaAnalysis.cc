/* 
 * File:   CosThetaAnalysis.cc
 * Author: hbakhshi
 * 
 * Created on February 1, 2012, 3:31 PM
 */

#include "WPolarization/interface/CosThetaAnalysis.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TROOT.h"
#include "../interface/ToyFitter.h"

CosThetaAnalysis::CosThetaAnalysis(const edm::ParameterSet& ps) : BASE(ps),
								  AcceptedEventTypes(ps.getParameter<vector<double> >("EventTypes")),
								  solverSolution((TopAnalysis::TTBarDileptonicEvent::SolverResults::solutions)(ps.getParameter<int >("SolverSolution"))),
								  solverName(ps.getParameter<string >("SolverName")),
								  FillGen(ps.getParameter<bool >("FillGen")),
								  FillTree(ps.getUntrackedParameter<bool >("FillTree", false)),
								  FillRec(ps.getParameter<bool >("FillRec")) {


  if (FillGen) {
    vector<int> GenDecayModesINT = ps.getParameter< vector<int> >("GenDecayModes");
    for (vector<int>::const_iterator int_mode = GenDecayModesINT.begin(); int_mode != GenDecayModesINT.end(); int_mode++)
      GenDecayModes.push_back((TopAnalysis::TTBarDileptonicEvent::TopDecays)(*int_mode));

    hCosThetaPosLepton_Gen = new TH1D("hCosThetaPosLepton_Gen", "cos(#theta) for positive Lepton (Gen)", 100, -1., 1.);
    hCosThetaNegLepton_Gen = new TH1D("hCosThetaNegLepton_Gen", "cos(#theta) for negative Lepton (Gen)", 100, -1., 1.);
    hCosThetaNeg_vs_Pos_Gen = new TH2D("hCosThetaNeg_vs_Pos_Gen", "cos(#theta) for ttbar events at generator level;cos(#theta) for top;cos(#theta) for antitop" , 100 , -1. , 1. , 100 , -1. , 1.);
    hCosThetaTopSpinNeg_vs_Pos_Gen = new TH2D("hCosThetaTopSpinNeg_vs_Pos_Gen", "cos(#theta) for ttbar events at generator level;cos(#theta) for top;cos(#theta) for antitop" , 100 , -1. , 1. , 100 , -1. , 1.);

    hCosThetaPosLepton_Gen_OLDDEF = new TH1D("hCosThetaPosLepton_Gen_OLDDEF", "cos(#theta) for positive Lepton (Gen, old def)", 100, -1., 1.);
    hCosThetaNegLepton_Gen_OLDDEF = new TH1D("hCosThetaNegLepton_Gen_OLDDEF", "cos(#theta) for negative Lepton (Gen, old def)", 100, -1., 1.);

    hTopMass_Gen = new TH1D("hTopMass_Gen", "Top Mass", 100, 130., 180.);
    hWMass_Gen = new TH1D("hWMass_Gen", "W Mass", 100, 60., 90.);
    hCosTheta1VS2Gen = new TH2D("hCosTheta1VS2Gen", "cos(#theta) formula comparison;old definitino;new definition", 100, -1.0, 1.0, 100, -1, 1);

    hDiffCosThetaVSPTop = new TH2D("hDiffCosThetaVSPTop", "#Delta cos(#theta) vs top momenteum;top momenteum;#Delta cos(#theta)", 100, 0., 400, 100, 0, 2);
    hDiffCosThetaVSPW = new TH2D("hDiffCosThetaVSPW", "#Delta cos(#theta) vs W momenteum;W momenteum;#Delta cos(#theta)", 100, 0., 400, 100, 0, 2);
    hDiffCosThetaVSMW = new TH2D("hDiffCosThetaVSMW", "#Delta cos(#theta) vs W Mass;W mass;#Delta cos(#theta)", 100, 0., 200, 100, 0, 2);
    hDiffCosThetaVSCosAngleTopW = new TH2D("hDiffCosThetaVSCosAngleTopW", "#Delta cos(#theta) vs w and top angle;W and top angle;#Delta cos(#theta)", 400, -1.0, 1.0, 100, 0, 2);
    hDiffCosThetaVSbEnergy = new TH2D("hDiffCosThetaVSbEnergy", "#Delta cos(#theta) vs b-energy;b-energy;#Delta cos(#theta)", 400, 0, 800, 100, 0, 2);
    hCosAngleTopWVSbEnergy = new TH2D("hCosAngleTopWVSbEnergy", "w and top angle vs b-energy;b-energy;W and top angle", 400, 0, 400.0, 100, -1, 1);
    hCosAngleTopW = new TH1D("hCosAngleTopW", "hCosAngleTopW", 400, -2, 2);

    hCosThetaPtLep_Gen = new TH2D("hCosThetaPtLep_Gen", "cos(#theta) vs. p_t of lepton", 100, -1, 1, 200, 0, 200);

    hCosThetaAllLeptonsVsTopEta_Gen = new TH2D("hCosThetaAllLeptonsVsTopEta_Gen" , "cos(#theta) vs. top #eta at generator level;top #eta;cos(#theta)", 100 , -10. , 10. , 100 , -1. , 1.);
  }
  if (FillRec) {
    hCosThetaPosLepton = new TH1D("hCosThetaPosLepton", "cos(#theta) for positive Lepton", 100, -1., 1.);
    hCosThetaNegLepton = new TH1D("hCosThetaNegLepton", "cos(#theta) for negative Lepton", 100, -1., 1.);
    hCosThetaNeg_vs_Pos = new TH2D("hCosThetaNeg_vs_Pos", "cos(#theta) for reconstructed events;top cos(#theta);antitop cos(#theta)" , 100 , -1. , 1. , 100 , -1. , 1.);
    hCosThetaNeg_vs_Pos_TTBar = new TH2D("hCosThetaNeg_vs_Pos_TTBar", "cos(#theta) for ttbar reconstructed events;top cos(#theta);antitop cos(#theta)" , 100 , -1. , 1. , 100 , -1. , 1.);

    hCosTheta1stLepton = new TH1D("hCosTheta1stLepton", "cos(#theta) for first Lepton", 100, -1., 1.);
    hCosTheta2ndLepton = new TH1D("hCosTheta2ndLepton", "cos(#theta) for second Lepton", 100, -1., 1.);

    hCosThetaAllLeptonsUnWeighted = new TH1D("hCosThetaAllLeptonsUnWeighted", "cos(#theta) for all Lepton, No weight", 100, -1., 1.);

    hCosThetaAllLeptonsVsTopEta = new TH2D("hCosThetaAllLeptonsVsTopEta" , "cos(#theta) vs. top #eta;top #eta;cos(#theta)", 100 , -10. , 10. , 100 , -1. , 1.);
    hCosThetaAllLeptonsVsLeptonIsolation = new TH2D("hCosThetaAllLeptonsVsLeptonIsolation", "cos(#theta) for all leptons vs. lepton isolation", 40, 0.0, 0.2, 100, -1, 1);
    hCosThetaAllLeptonsVsLeptonJetDR = new TH2D("hCosThetaAllLeptonsVsLeptonJetDR", "cos(#theta) for all leptons vs. lepton #delta R with the bjet", 400, 0.0, 20, 100, -1, 1);
    hCosThetaAllLeptonsVsLeptonPt = new TH2D("hCosThetaAllLeptonsVsLeptonPt", "cos(#theta) for all leptons vs. lepton p_t", 1000, 0.0, 1000, 100, -1, 1);
    hCosThetaAllGenVsREC = new TH2D("hCosThetaAllGenVsREC", "cos(#theta) for all leptons in gen-level vs. rec-level;cos(#theta_{rec});cos(#theta_{rec})", 100, -1.0, 1.0, 100, -1, 1);
    hCosThetaAllGenVsRECVsDR = new TH3D("hCosThetaAllGenVsRECvsDR", "cos(#theta) for all leptons in gen-level vs. rec-level;cos(#theta_{rec});cos(#theta_{rec});#Delta R", 100, -1.0, 1.0, 100, -1, 1, 80, 0.0, 8.0);

    if (FillTree) {
      fileTree = TFile::Open(ps.getParameter<string > ("TreeFileName").c_str(), "RECREATE");
      fileTree->cd();
      for (int i = 1; i < 11; i++) {
	ostringstream tree_name_;
	tree_name_ << "Tree_" << i;

	string tree_name = tree_name_.str();

	TTree* t = (allTrees[i] = new TTree(tree_name.c_str(), tree_name.c_str()));


	t->Branch("GenTopCosTheta", &genTopCosThetaValueHolder, "GenTopCosTheta/D");
	t->Branch("GenAntiTopCosTheta", &genAntiTopCosThetaValueHolder, "GenAntiTopCosTheta/D");

	t->Branch("GenCosTheta", &genCosThetaValueHolder, "GenCosTheta/D");
	t->Branch("RecCosTheta", &recCosThetaValueHolder, "RecCosTheta/D");

	t->Branch("EventWeight", &eventWeight, "EventWeight/D");
	t->Branch("eventType", &eventType, "eventType/I");
	t->Branch("GenEventType", &GenEventType, "GenEventType/I");

	t->Branch("PDFScalePDF", &PDFScalePDF, "PDFScalePDF/F");
	t->Branch("PDFx1", &PDFx1, "PDFx1/F");
	t->Branch("PDFx2", &PDFx2, "PDFx2/F");
	t->Branch("PDFxPDF1", &PDFxPDF1, "PDFxPDF1/F");
	t->Branch("PDFxPDF2", &PDFxPDF1, "PDFxPDF2/F");
	t->Branch("PDFID1", &PDFID1, "PDFID1/I");
	t->Branch("PDFID2", &PDFID2, "PDFID2/I");

	t->Branch("nPU", &nPU, "nPU/I");
	t->Branch("nPV", &nPV, "nPV/I");

	t->Branch("isElectron", &isElectron, "isElectron/O");
	t->Branch("isFirstLepton", &isFirstLepton, "isFirstLepton/O");
	t->Branch("LeptonPt", &LeptonPt, "LeptonPt/D");
	t->Branch("LeptonEta", &LeptonEta, "LeptonEta/D");
      }
      gROOT->cd();
    }
  }
}

CosThetaAnalysis::~CosThetaAnalysis() {
}

bool CosThetaAnalysis::Run(TopAnalysis::TTBarDileptonicEvent* ev) {


  double cosThetaGenPos = -2.0;
  double cosThetaGenNeg = -2.0;
  if (FillGen) {
    //    if (ev->TOP_Gen.W.isElectron == ev->TOPBar_Gen.W.isElectron) {
      cosThetaGenPos = ev->TOP_Gen.CosTheta();
      cosThetaGenNeg = ev->TOPBar_Gen.CosTheta();
      //cout << "ok" << endl;
      //} else {
      //cosThetaGenPos = (ev->TOP_Gen.W.isElectron ? ev->TOP_Gen.CosTheta() : ev->TOPBar_Gen.CosTheta() );
      //cosThetaGenNeg = (ev->TOP_Gen.W.isElectron ? ev->TOPBar_Gen.CosTheta() : ev->TOP_Gen.CosTheta() );
      //cout << "What the hell is this one "<< ev->TOP_Gen.W.isElectron <<"  "<< ev->TOPBar_Gen.W.isElectron << " "  <<  ev->GenDecayMode << endl;
      //}

    double cosThetaGenPos2 = ev->TOP_Gen.CosTheta(2);
    double cosThetaGenNeg2 = ev->TOPBar_Gen.CosTheta(2);

    //        cout << cosThetaGenNeg << "  " << cosThetaGenPos << endl;

    if (std::find(this->GenDecayModes.begin(), this->GenDecayModes.end(), ev->GenDecayMode) != this->GenDecayModes.end()) {
      //            cout << "Is filling ... " << ev->GenDecayMode ;
      //            for(int iii=0 ; iii<this->GenDecayModes.size() ; iii++)
      //                cout << " ... " << GenDecayModes[iii] ;
      //            cout << " The weight is : " << ev->Weight ;
      //            cout << endl;

      hCosTheta1VS2Gen->Fill(cosThetaGenPos2, cosThetaGenPos);
      hCosTheta1VS2Gen->Fill(cosThetaGenNeg2, cosThetaGenNeg);

      hDiffCosThetaVSPTop->Fill(ev->TOP_Gen.getTop().Vect().Mag(), fabs(cosThetaGenPos2 - cosThetaGenPos));
      hDiffCosThetaVSPW->Fill(ev->TOP_Gen.getW().Vect().Mag(), fabs(cosThetaGenPos2 - cosThetaGenPos));
      hDiffCosThetaVSMW->Fill(ev->TOP_Gen.getW().M(), fabs(cosThetaGenPos2 - cosThetaGenPos));
      hDiffCosThetaVSCosAngleTopW->Fill(ev->TOP_Gen.CosThetaTopW(), fabs(cosThetaGenPos2 - cosThetaGenPos));
      hDiffCosThetaVSbEnergy->Fill(ev->TOP_Gen.b.Energy(), fabs(cosThetaGenPos2 - cosThetaGenPos));
      hCosAngleTopWVSbEnergy->Fill(ev->TOP_Gen.b.Energy(), ev->TOP_Gen.CosThetaTopW());

      hDiffCosThetaVSPTop->Fill(ev->TOPBar_Gen.getTop().Vect().Mag(), fabs(cosThetaGenNeg2 - cosThetaGenNeg));
      hDiffCosThetaVSPW->Fill(ev->TOPBar_Gen.getW().Vect().Mag(), fabs(cosThetaGenNeg2 - cosThetaGenNeg));
      hDiffCosThetaVSMW->Fill(ev->TOPBar_Gen.getW().M(), fabs(cosThetaGenNeg2 - cosThetaGenNeg));
      hDiffCosThetaVSCosAngleTopW->Fill(ev->TOPBar_Gen.CosThetaTopW(), fabs(cosThetaGenNeg2 - cosThetaGenNeg));
      hDiffCosThetaVSbEnergy->Fill(ev->TOPBar_Gen.b.Energy(), fabs(cosThetaGenNeg2 - cosThetaGenNeg));
      hCosAngleTopWVSbEnergy->Fill(ev->TOPBar_Gen.b.Energy(), ev->TOPBar_Gen.CosThetaTopW());


      hCosAngleTopW->Fill(ev->TOP_Gen.CosThetaTopW());
      hCosAngleTopW->Fill(ev->TOPBar_Gen.CosThetaTopW());

      hTopMass_Gen->Fill(ev->TOP_Gen.getTop().M());
      hTopMass_Gen->Fill(ev->TOPBar_Gen.getTop().M());
      hWMass_Gen->Fill(ev->TOP_Gen.getW().M());
      hWMass_Gen->Fill(ev->TOPBar_Gen.getW().M());

      hCosThetaPosLepton_Gen->Fill(cosThetaGenPos, ev->Weight);
      hCosThetaNegLepton_Gen->Fill(cosThetaGenNeg, ev->Weight);
      hCosThetaNeg_vs_Pos_Gen->Fill( cosThetaGenPos , cosThetaGenNeg , ev->Weight );
      hCosThetaTopSpinNeg_vs_Pos_Gen->Fill( ev->TOPBar_Gen.CosThetaTopSpin() ,ev->TOP_Gen.CosThetaTopSpin()  );
      
      hCosThetaPosLepton_Gen_OLDDEF->Fill(cosThetaGenPos2, ev->Weight);
      hCosThetaNegLepton_Gen_OLDDEF->Fill(cosThetaGenNeg2, ev->Weight);

      hCosThetaAllLeptonsVsTopEta_Gen->Fill(cosThetaGenPos, ev->TOP_Gen.getTop().Eta(), ev->Weight);
      hCosThetaAllLeptonsVsTopEta_Gen->Fill(cosThetaGenNeg, ev->TOPBar_Gen.getTop().Eta(), ev->Weight);

      hCosThetaPtLep_Gen->Fill(cosThetaGenPos, ev->TOP_Gen.W.lepton.Pt(), ev->Weight);
      hCosThetaPtLep_Gen->Fill(cosThetaGenNeg, ev->TOPBar_Gen.W.lepton.Pt(), ev->Weight);
    }
  }
  if (!FillRec)
    return true;

  double evtType = EventTypeReader.ReadValue(ev);
  if (std::find(AcceptedEventTypes.begin(), AcceptedEventTypes.end(), evtType) == AcceptedEventTypes.end()) {
    return false;
  }

  if (!ev->SelectASolution(solverName, solverSolution))
    return false;

  //for first lepton :

  double costheta_top = ev->Top_Rec->CosTheta();
  double costheta_tbar = ev->TopBar_Rec->CosTheta();

  if ((FillGen && ev->GenDecayMode != ev->RecDecayMode) || !FillGen) {
    hCosThetaPosLepton->Fill(costheta_top, ev->Weight);
    hCosThetaNegLepton->Fill(costheta_tbar, ev->Weight);

    hCosThetaNeg_vs_Pos->Fill( costheta_top , costheta_tbar , ev->Weight );
  }else
    hCosThetaNeg_vs_Pos_TTBar->Fill( costheta_top , costheta_tbar , ev->Weight );

  hCosThetaAllLeptonsVsTopEta->Fill( ev->Top_Rec->getTop().Eta() , costheta_top , ev->Weight );
  hCosThetaAllLeptonsVsTopEta->Fill( ev->TopBar_Rec->getTop().Eta() , costheta_tbar , ev->Weight );

  hCosThetaAllLeptonsVsLeptonIsolation->Fill(ev->Top_Rec->W.leptonIsolationValue, costheta_top, ev->Weight);
  hCosThetaAllLeptonsVsLeptonJetDR->Fill(ev->Top_Rec->LeptonBDeltaR(), costheta_top, ev->Weight);
  hCosThetaAllLeptonsVsLeptonPt->Fill(ev->Top_Rec->W.lepton.Pt(), costheta_top, ev->Weight);

  hCosThetaAllLeptonsVsLeptonIsolation->Fill(ev->TopBar_Rec->W.leptonIsolationValue, costheta_tbar, ev->Weight);
  hCosThetaAllLeptonsVsLeptonJetDR->Fill(ev->TopBar_Rec->LeptonBDeltaR(), costheta_tbar, ev->Weight);
  hCosThetaAllLeptonsVsLeptonPt->Fill(ev->TopBar_Rec->W.lepton.Pt(), costheta_tbar, ev->Weight);


  if ((FillGen && ev->GenDecayMode == ev->RecDecayMode)) {
    hCosThetaAllGenVsREC->Fill(costheta_top, cosThetaGenPos, ev->Weight);
    hCosThetaAllGenVsREC->Fill(costheta_tbar, cosThetaGenNeg, ev->Weight);

    double dr1 = ROOT::Math::VectorUtil::DeltaR(ev->Top_Rec->getTop(), ev->TOP_Gen.getTop());
    hCosThetaAllGenVsRECVsDR->Fill(costheta_top, cosThetaGenPos, dr1, ev->Weight);
    double dr2 = ROOT::Math::VectorUtil::DeltaR(ev->TopBar_Rec->getTop(), ev->TOPBar_Gen.getTop());
    hCosThetaAllGenVsRECVsDR->Fill(costheta_tbar, cosThetaGenNeg, dr2, ev->Weight);
  }
  if (FillTree) {
    fileTree->cd();
    eventWeight = ev->Weight;
    nPU = ev->PUnumInteractions;
    nPV = ev->NPrimaryVertices;
    eventType = int(ev->RecDecayMode);

    PDFID1 = ev->PDFID1;
    PDFID2 = ev->PDFID2;
    PDFScalePDF = ev->PDFScalePDF;
    PDFx1 = ev->PDFx1;
    PDFx2 = ev->PDFx2;
    PDFxPDF1 = ev->PDFxPDF1;
    PDFxPDF2 = ev->PDFxPDF2;

    if (FillGen){
      GenEventType = int(ev->GenDecayMode);
      genTopCosThetaValueHolder = ev->Top_Rec->CosThetaTopSpin();
      genAntiTopCosThetaValueHolder = ev->TopBar_Rec->CosThetaTopSpin();
    }
    else{
      GenEventType = int(-100);
      genTopCosThetaValueHolder = -2.0;
      genAntiTopCosThetaValueHolder = -2.0;
    }
    if (costheta_top > 1.0 || costheta_top < -1.0)
      cout << "wrong costhet* t value" << costheta_top << endl;
    else {
      int binGen1; // = (int) (floor(costheta_top / 0.2) + 6.0);
      binGen1 = 1 + int (10 * (costheta_top + 1.0) / (2.0));
      genCosThetaValueHolder = cosThetaGenPos;
      recCosThetaValueHolder = costheta_top;

      isElectron = ev->Top_Rec->W.isElectron;
      isFirstLepton = (ev->FirstElectron->Charge == ev->Top_Rec->Charge);
      LeptonPt = ev->Top_Rec->W.lepton.Pt();
      LeptonEta = ev->Top_Rec->W.lepton.Eta();

      if (binGen1 > 10)
	binGen1 = 10;
      if (allTrees.count(binGen1) == 1)
	allTrees[binGen1]->Fill();
      else
	cout << "wrong binGen1 value : " << costheta_top << "  " << binGen1 << endl;
    }

    if (costheta_tbar > 1.0 || costheta_tbar < -1.0)
      cout << "wrong costhet* tbar value" << costheta_tbar << endl;
    else {
      int binGen2; //= (int) (floor(costheta_tbar / 0.2) + 6.0);
      binGen2 = 1 + int (10 * (costheta_tbar + 1.0) / (2.0));
      genCosThetaValueHolder = cosThetaGenNeg;
      recCosThetaValueHolder = costheta_tbar;

      isElectron = ev->TopBar_Rec->W.isElectron;
      isFirstLepton = (ev->FirstElectron->Charge == ev->TopBar_Rec->Charge);
      LeptonPt = ev->TopBar_Rec->W.lepton.Pt();
      LeptonEta = ev->TopBar_Rec->W.lepton.Eta();

      if (binGen2 > 10)
	binGen2 = 10;
      if (allTrees.count(binGen2) == 1)
	allTrees[binGen2]->Fill();
      else
	cout << "wrong binGen2 value : " << costheta_tbar << "  " << binGen2 << endl;
    }
    gROOT->cd();
  }

  hCosThetaAllLeptonsUnWeighted->Fill(costheta_top);
  hCosThetaAllLeptonsUnWeighted->Fill(costheta_tbar);

  if (ev->Top_Rec->W.lepton.Pt() > ev->TopBar_Rec->W.lepton.Pt()) {
    hCosTheta1stLepton->Fill(costheta_top, ev->Weight);
    hCosTheta2ndLepton->Fill(costheta_tbar, ev->Weight);
  } else {
    hCosTheta1stLepton->Fill(costheta_tbar, ev->Weight);
    hCosTheta2ndLepton->Fill(costheta_top, ev->Weight);
  }
  return true;
}

void CosThetaAnalysis::End() {
  if (FillTree) {
    fileTree->cd();
    for (int i = 1; i < 11; i++) {
      allTrees[i]->Write();
    }

    TH2* signal2DFromTree = new TH2D("hsignal2DFromTree", "signal2DFromTree", 10, -1.0, 1.0, 10000, -1.0, 1.0);
    for (int nBin = 1; nBin < 10 + 1; nBin++) {
      double binCenter = hCosThetaAllGenVsREC->GetXaxis()->GetBinCenter(nBin);
      ostringstream treeName;

      treeName << "Tree_" << nBin;
      TTree* tree = (TTree*) allTrees[nBin];

      double genCosTheta = 0.0;
      double eventWeight = 0.0;
      double lumiWeight = 0.0;
      int eventType = 0.0;

      tree->SetBranchAddress("GenCosTheta", &genCosTheta);
      tree->SetBranchAddress("EventWeight", &eventWeight);
      tree->SetBranchAddress("LumiWeight", &lumiWeight);
      tree->SetBranchAddress("eventType", &eventType);


      for (int entry = 0; entry < tree->GetEntries(); entry++) {

	tree->GetEntry(entry);

	double evtweight = eventWeight; //* lumiWeight;

	if (eventType == 3) { //dile
	  evtweight /= 0.994;
	  evtweight *= 0.192709;
	} else if (eventType == 1 || eventType == 2) {//emu
	  evtweight /= 0.995;
	  evtweight *= 0.197058;
	} else if (eventType == 4) {//emu
	  evtweight /= 0.997;
	  evtweight *= 0.189709;
	}

	signal2DFromTree->Fill(binCenter, genCosTheta, evtweight);
      }
      cout << "Tree_" << (nBin) << "is read" << endl;
    }

    signal2DFromTree->Write();

    fileTree->Close();
  }

  TFile* file = info::TheInfo->OutFile;
  file->cd();
  file->mkdir(this->Name.c_str())->cd();
  //    cout << this->Name << endl;

  if (FillRec) {
    hCosThetaPosLepton->Write();
    dGammaFunctin::Fit(hCosThetaPosLepton, 10)->Write();
    dGammaFunctin::Fit(hCosThetaPosLepton, 10, true, true)->Write();

    hCosThetaNeg_vs_Pos_TTBar->Write();
    hCosThetaNeg_vs_Pos->Write();

    hCosThetaNegLepton->Write();
    dGammaFunctin::Fit(hCosThetaNegLepton, 10)->Write();
    dGammaFunctin::Fit(hCosThetaNegLepton, 10, true, true)->Write();

    TH1D hAddRec("hCosThetaAllLepton", "cos(#theta) for all Leptons", 100, -1., 1.);
    hAddRec.Add(hCosThetaPosLepton, hCosThetaNegLepton);
    hAddRec.Write();
    dGammaFunctin::Fit(&hAddRec, 10)->Write();
    dGammaFunctin::Fit(&hAddRec, 10, true, true)->Write();

    hCosTheta1stLepton->Write();
    dGammaFunctin::Fit(hCosTheta1stLepton, 10)->Write();
    dGammaFunctin::Fit(hCosTheta1stLepton, 10, true, true)->Write();

    hCosTheta2ndLepton->Write();
    dGammaFunctin::Fit(hCosTheta2ndLepton, 10)->Write();
    dGammaFunctin::Fit(hCosTheta2ndLepton, 10, true, true)->Write();

    hCosThetaAllLeptonsUnWeighted->Write();
    dGammaFunctin::Fit(hCosThetaAllLeptonsUnWeighted, 10)->Write();
    dGammaFunctin::Fit(hCosThetaAllLeptonsUnWeighted, 10, true, true)->Write();

    hCosThetaAllLeptonsVsTopEta->Write();
    hCosThetaAllLeptonsVsLeptonIsolation->Write();
    hCosThetaAllLeptonsVsLeptonJetDR->Write();
    hCosThetaAllLeptonsVsLeptonPt->Write();
    hCosThetaAllGenVsREC->Write();
    hCosThetaAllGenVsRECVsDR->Write();

  }

  if (FillGen) {
    TH1D hNoWTBSum("hNoWTBSum", "", 10, -1, 1);

    hCosThetaPosLepton_Gen->Write();
    dGammaFunctin::Fit(hCosThetaPosLepton_Gen, 10, true, true)->Write();
        
    /*
    //        hCosThetaPosLepton_Gen->Print("ALL");
    //        cout << "wrote " << hCosThetaPosLepton_Gen->GetName() << endl;
    TH1* htmp1 = dGammaFunctin::Fit(hCosThetaPosLepton_Gen, 10);
    htmp1->Write();


    TH2D hTWB2D("hTWB2D", "", 10, -1, 1, 10, -1, 1);
    for (int i = 1; i < 11; i++)
    hTWB2D.SetBinContent(i, i, htmp1->GetBinContent(i));

    std::pair<TF3, LikelihoodFunction*> llPosLepton = LikelihoodFunction::getLLFunction("llPosLepton", &hNoWTBSum, htmp1, &hTWB2D, true);
    double x[3] = {1., 1., 1.};
    double xerr[3] = {1., 1., 1.};
    double correlation;
    GetMinimum(llPosLepton.first, x, xerr, correlation, true);
    double fneg = x[1];
    double f0 = x[0];
    double fpos = 1.0 - x[1] - x[0];

    double errfneg = xerr[1];
    double errf0 = xerr[0];
    double errfpos = sqrt(errf0 * errf0 + errfneg * errfneg + (2 * correlation));

    cout << "|" << fneg << "+-" << errfneg;
    cout << "|" << f0 << "+-" << errf0;
    cout << "|" << correlation;
    cout << "|" << fpos << "+-" << errfpos;
    cout << "|" << x[2] << "+-" << xerr[2] << "|" << endl;
    */

    hCosThetaNeg_vs_Pos_Gen->Write();
    hCosThetaTopSpinNeg_vs_Pos_Gen->Write();

    hCosThetaNegLepton_Gen->Write();
    dGammaFunctin::Fit(hCosThetaNegLepton_Gen, 10)->Write();
    dGammaFunctin::Fit(hCosThetaNegLepton_Gen, 10, true, true)->Write();


    TH1D hAddGen("hCosThetaAllLepton_Gen", "cos(#theta) for all Leptons (Gen)", 100, -1., 1.);
    hAddGen.Add(hCosThetaPosLepton_Gen, hCosThetaNegLepton_Gen);
    hAddGen.Write();
    dGammaFunctin::Fit(&hAddGen, 10)->Write();

    TH1D hAddGen_OLDDEF("hCosThetaAllLepton_Gen_OLDDEF", "cos(#theta) for all Leptons (Gen_OLDDEF)", 100, -1., 1.);
    hAddGen_OLDDEF.Add(hCosThetaPosLepton_Gen_OLDDEF, hCosThetaNegLepton_Gen_OLDDEF);
    hAddGen_OLDDEF.Write();
    dGammaFunctin::Fit(&hAddGen_OLDDEF, 10)->Write();

    hCosThetaPtLep_Gen->Write();

    hCosThetaAllLeptonsVsTopEta_Gen->Write();

    hWMass_Gen->Write();
    hTopMass_Gen->Write();

    hCosTheta1VS2Gen->Write();
    hCosAngleTopW->Write();
    hDiffCosThetaVSMW->Write();
    hDiffCosThetaVSPW->Write();
    hDiffCosThetaVSPTop->Write();
    hDiffCosThetaVSCosAngleTopW->Write();
    hDiffCosThetaVSbEnergy->Write();
    hCosAngleTopWVSbEnergy->Write();
  }
  //cout << this->Name << " ended" << endl;
  file->cd();
}
