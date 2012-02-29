/* 
 * File:   BJetAssigner.cc
 * Author: hbakhshi
 * 
 * Created on January 30, 2012, 7:34 PM
 */

#include "WPolarization/interface/BJetAssigner.h"

BJetAssigner::BJetAssigner(const edm::ParameterSet& ps) : BASE(ps),
random_generator(ps.getUntrackedParameter<int>("RandomSeed", 7764563)),
the_b_index(-1),
the_bbar_index(-1) {
    method_name = ps.getParameter<string > ("method");
    if (method_name == "random")
        this->Method = method_random;
    else if (method_name == "genmatch")
        this->Method = method_genmatch;
}

BJetAssigner::~BJetAssigner() {
}

bool BJetAssigner::Run(TopAnalysis::TTBarDileptonicEvent* ev) {
    switch (this->Method) {
        case method_random:
            if (random_generator.Uniform(0.0, 1.0) > 0.5) {
                the_b_index = 0;
                the_bbar_index = 1;
            } else {
                the_b_index = 1;
                the_bbar_index = 0;
            }
            return true;
        default:
            throw ("method " + method_name + " is not implemented yet");
    }
    return false;
}
