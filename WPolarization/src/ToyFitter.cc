#include <stdlib.h>

#include "../interface/ToyFitter.h"
#include "../interface/FitValidator.h"
#include "../interface/ValidatorExecuter.h"
#include "TROOT.h"
#include <sched.h>
cpu_set_t mask_t;

int main(int argc, char *argv[]) {
    TH1::SetDefaultSumw2(true);

    string ChannelName;
    string AllEventsHistogramDirectory;
    int StartPEX;
    int LPEX;
    int StartPEXPull;
    int LPEXPull;
    int cpu;
    
    int printAll;
    int zwqscale;
    int sampleToRewieght;
    int other_syst;

    string ttbarName = "";
    int mode = -1;
    if (argc == 9) {
        ChannelName = argv[1];
        AllEventsHistogramDirectory = argv[2];
        cpu = atoi(argv[3]);

        
        ttbarName = argv[4];
        printAll = atoi(argv[5]);
        zwqscale = atoi(argv[6]);
        sampleToRewieght = atoi(argv[7]);
        other_syst = atoi(argv[8]);
        
        mode = 0;
    } else if (argc == 8) {
        ChannelName = argv[1];
        AllEventsHistogramDirectory = argv[2];
        StartPEX = atoi(argv[3]);
        LPEX = atoi(argv[4]);
        StartPEXPull = atoi(argv[5]);
        LPEXPull = atoi(argv[6]);
        cpu = atoi(argv[7]);

        mode = 1;
    } else {
        std::cout << "wrong argument list" << std::endl;
        string exe_name = string(argv[0]);
        std::cout << "usage : " + exe_name + " ChannelName AllEventsHistogramDirectory #FirstPEX #PEXs #FirstPullPEX #PullPEXs #CPU" << endl;
        std::cout << "usage : " + exe_name + " ChannelName AllEventsHistogramDirectory #CPU TreeFileName printAll zwqscale sampleToReweight weightSign" << endl;
        return EXIT_FAILURE;
    }
    CPU_ZERO(&mask_t);
    CPU_SET(cpu, &mask_t);
    if (sched_setaffinity(0, sizeof (cpu_set_t), &mask_t) < 0)
        perror("sched_setaffinity");

    if (mode == 1) {
        std::cout << "RunFitValidation(" << StartPEX << "," << LPEX << "," << StartPEXPull << "," << LPEXPull << "," << ChannelName << "," << AllEventsHistogramDirectory << ")" << endl;
        RunFitValidation(StartPEX, LPEX, StartPEXPull, LPEXPull, ChannelName, AllEventsHistogramDirectory);
    } else if (mode == 0) {
        //std::cout << "RunFit("  << ChannelName << "," << AllEventsHistogramDirectory << "," << ttbarName << ")" << endl;
        RunFit(ChannelName, AllEventsHistogramDirectory, ttbarName, printAll , zwqscale , sampleToRewieght , other_syst);
    }
    return EXIT_SUCCESS;
}
