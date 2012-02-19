#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <TFile.h>
#include <TAttMarker.h>

#ifdef SUSYSelector
#include "ElectronCharge/include/SSSusySelector.h"
#include "ElectronCharge/include/Predict.h"
#elif ZSelector
#include "ElectronCharge/include/ZEventSelector.h"
#elif FRCalculator
#include "Fake/include/CalculateFakeRate.h"
#include "Fake/include/SameFlavourDiLeptonFakeEstimator.h"
#include "Fake/include/SingleLeptonSelector.h"
#endif
#include "base/LoopHelper.h"
#include "BaseAnalysis/interface/ObjectProperty.h"
#include <sstream>
#include <CLHEP/Units/PhysicalConstants.h>

LoopHelper h;

#include "ElectronCharge/include/PMMReader.h"
#include "ElectronCharge/include/PmmCalculator.h"

#include <sched.h>

#include "ElectronCharge/include/EventPrinter.h"

cpu_set_t mask_t;

int main(int argc, char *argv[]) {
    //GetStatusString = EventSelector::GetStatusString;
    info::AnalyzeArgs(argc, argv);
    info::TheInfo->Print();

    //__CPU_SET(info::CPU_Number, &mask_t); //Run over CPU #1
    __CPU_ALLOC(info::CPU_Number);
    if (sched_setaffinity(0, sizeof (cpu_set_t), &mask_t) < 0)
        perror("sched_setaffinity");

    for (edm::VParameterSet::const_iterator anlz = info::TheInfo->Analyzers.begin();
            anlz != info::TheInfo->Analyzers.end(); anlz++) {
        string TYPE = anlz->getParameter<string > ("Type");
        if (TYPE == "PMM") {
#ifdef ZEventSelector_H
#warning ZEventSelector_H
            h.Analyzers.push_back(new PMMCalculator(*anlz));
#endif
        } else if (TYPE == "EstimateMisChargeds") {
#ifdef SSSusyEVENTSELECTOR_H
#warning SSSusyEVENTSELECTOR_H
            h.Analyzers.push_back(new PMMPredict(*anlz));
#endif
        } else if (TYPE == "FRCalc") {
#ifdef CALCULATEFAKERATE_H
#warning CALCULATEFAKERATE_H
            h.Analyzers.push_back(new CalculateFakeRate(*anlz));
#endif
        } else if (TYPE == "SameFlavorFakeEstimator") {
#ifdef CALCULATEFAKERATE_H
#warning CALCULATEFAKERATE_H
            h.Analyzers.push_back(new SameFlavourDiLeptonFakeEstimator(*anlz));
#endif
        }
        else if (TYPE == "Printer") {
            h.Analyzers.push_back(new EventPrinter(*anlz));
        }
    }
    h.Start(* (info::TheInfo));
    info::TheInfo->Print();
    return (EXIT_SUCCESS);
}
