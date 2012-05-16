#include <stdlib.h>

#include "../interface/ToyFitter.h"
#include "../interface/FitValidator.h"
#include "../interface/ValidatorExecuter.h"

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
    if (argc != 8) {
        std::cout << "wrong argument list" << std::endl;
        string exe_name = string(argv[0]);
        std::cout << "usage : " + exe_name + " ChannelName AllEventsHistogramDirectory #FirstPEX #PEXs #FirstPullPEX #PullPEXs #CPU" << endl;
        return EXIT_FAILURE;
    } else {
        ChannelName = argv[1];
        AllEventsHistogramDirectory = argv[2];
        StartPEX = atoi(argv[3]);
        LPEX = atoi(argv[4]);
        StartPEXPull = atoi(argv[5]);
        LPEXPull = atoi(argv[6]);
        cpu = atoi(argv[7]);
    }
    CPU_ZERO(&mask_t);
    CPU_SET(cpu, &mask_t);
    if (sched_setaffinity(0, sizeof (cpu_set_t), &mask_t) < 0)
        perror("sched_setaffinity");
    
    std::cout << "RunFitValidation(" << StartPEX << "," << LPEX << "," << StartPEXPull << "," << LPEXPull << "," << ChannelName << "," << AllEventsHistogramDirectory << ")" << endl;
    RunFitValidation(StartPEX, LPEX, StartPEXPull, LPEXPull, ChannelName, AllEventsHistogramDirectory);
    return EXIT_SUCCESS;
}
