#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <TFile.h>
#include <TAttMarker.h>

#include "WPolarization/interface/TTbarSelector.h" 

#include "base/LoopHelper.h"
#include "BaseAnalysis/interface/ObjectProperty.h"
#include <sstream>
#include <CLHEP/Units/PhysicalConstants.h>

LoopHelper h;

#include <sched.h>

#include "ElectronCharge/include/EventPrinter.h"
#include "WPolarization/interface/BJetAssigner.h"
#include "WPolarization/interface/NeutrinoSolver.h"
#include "WPolarization/interface/CosThetaAnalysis.h"
#include "WPolarization/interface/GenRecComparison.h"
#include "WPolarization/interface/DPhiAnalyzer.h"

cpu_set_t mask_t;

int main(int argc, char *argv[]) {
    TH1::SetDefaultSumw2(true);
    
    info::AnalyzeArgs(argc, argv , "WPolarization");
    info::TheInfo->Print();
    return (EXIT_SUCCESS);


    CPU_ZERO(&mask_t) ;
    CPU_SET(info::CPU_Number , &mask_t);
    
    if (sched_setaffinity(0, sizeof (cpu_set_t), &mask_t) < 0)
        perror("sched_setaffinity");

    for (edm::VParameterSet::const_iterator anlz = info::TheInfo->Analyzers.begin();
            anlz != info::TheInfo->Analyzers.end(); anlz++) {
        string TYPE = anlz->getParameter<string > ("Type");
        if (TYPE == "...") {
            //do something
        }
        else if (TYPE == "b_assigner") {
            h.Analyzers.push_back(new BJetAssigner(*anlz));
        }
        else if (TYPE == "neutrino_solver") {
            h.Analyzers.push_back(new NeutrinoSolver(*anlz));
        }
        else if (TYPE == "costheta") {
            h.Analyzers.push_back(new CosThetaAnalysis(*anlz));
        }
        else if (TYPE == "Printer") {
            h.Analyzers.push_back(new EventPrinter(*anlz));
        }
        else if (TYPE == "GenRecComparison") {
            h.Analyzers.push_back(new GenRecComparison(*anlz));
        }
        else if (TYPE == "DPhiAnalyzer") {
            h.Analyzers.push_back(new DPhiAnalzer(*anlz));
        }
    }
    h.Start(* (info::TheInfo));
    info::TheInfo->Print();
    return (EXIT_SUCCESS);
}
