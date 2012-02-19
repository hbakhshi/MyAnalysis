/* 
 * File:   PmmCalculator.h
 * Author: hbakhshi
 *
 * Created on November 25, 2010, 1:01 PM
 */

#ifndef PMMCALCULATOR_H
#define	PMMCALCULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <TFile.h>
#include <TAttMarker.h>

#include "BaseAnalysis/interface/ObjectProperty.h"
#include <DataFormats/interface/DiElecEffHandler_IJ.h>
#include <DataFormats/interface/DiElectronEventProperties.h>
#include <DataFormats/interface/ElectronProperties.h>
#include <DataFormats/interface/DiElectronEvent.h>
#include <DataFormats/interface/Electron.h>
#include <BaseAnalysis/interface/Histograms.h>
#include <BaseAnalysis/interface/HistogramsManager.h>
#include <Gtypes.h>
#include <TCanvas.h>
#include <TH1.h>
#include <sstream>
#include <CLHEP/Units/PhysicalConstants.h>
#include "NTupleAnalyzer/include/base/Analyzer.h"

using namespace ElectronAnalysis::DiElectronEventProperties;

class PMMCalculator : public Analyzer<ElectronAnalysis::DiElectronEvent> {
public:

    int NNN;
    
    PMMCalculator(const edm::ParameterSet& ps) : Analyzer<ElectronAnalysis::DiElectronEvent>(ps),
    chargeErrorGen1("CErrorGen1", 1),
    CERL_GsfCharge("GsfCharge"),
    CERL_EleCharge("EleCharge"),
    CERL_CTFCharge("CTFCharge"),
    CERL_AllCharge("AllCharge"),
    AllHistosForEvents(ps.getParameter<string>("HistogramsName")),
    NNN(0),
    xsec_inverse(0.0),
    current_int_lum(0.0),

    UseGen(ps.getParameter<bool>("UseGen")),
    DirName(ps.getParameter<string>("DirName")) {

        CERL_CTFCharge.PrintOrder = ps.getParameter<std::vector<string> >("CTFChargePrintOrder");
        CERL_EleCharge.PrintOrder = ps.getParameter<std::vector<string> >("EleChargePrintOrder");
        CERL_GsfCharge.PrintOrder = ps.getParameter<std::vector<string> >("GSFChargePrintOrder");
        CERL_AllCharge.PrintOrder = ps.getParameter<std::vector<string> >("AllChargePrintOrder");

        hIsoEle1vsIsoEle2 = new TH2D("hIsoEle1vsIsoEle2" , "" , 100 , 0, 1, 100, 0,1);
        hD0Ele1vsD0Ele2 = new TH2D("hD0Ele1vsD0Ele2" , "" , 500 , 0, 0.5, 500, 0,5);
    };

    void OnChangeFile(FileInfo* fi, DataSet* ds);
    bool Run(ElectronAnalysis::DiElectronEvent* ev);
    void End();
private:
    ElectronAnalysis::ElectronProperties::ChargeErrorProbabilityHistograms chargeErrorGen1;


    CERL_CHARGE<ElectronAnalysis::Electron::GSF> CERL_GsfCharge;
    CERL_CHARGE<ElectronAnalysis::Electron::ElectronCharge> CERL_EleCharge;
    CERL_CHARGE<ElectronAnalysis::Electron::CTF> CERL_CTFCharge;
    CERL_CHARGE<ElectronAnalysis::Electron::All> CERL_AllCharge;

    HistogramsManager< ElectronAnalysis::DiElectronEvent > AllHistosForEvents;
    Histograms< ElectronAnalysis::DiElectronEvent >* CurrentEventHistos;

    double xsec_inverse;
    double current_int_lum;

    bool UseGen;
    string DirName;

    TH2* hIsoEle1vsIsoEle2;
    TH2* hD0Ele1vsD0Ele2;

    TH1* hD0Electrons;
};



#endif	/* PMMCALCULATOR_H */

