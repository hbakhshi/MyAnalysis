/* 
 * File:   BJetAssigner.cc
 * Author: hbakhshi
 * 
 * Created on January 30, 2012, 7:34 PM
 */

#include "WPolarization/interface/BJetAssigner.h"

BJetAssigner::BJetAssigner(const edm::ParameterSet& ps) : BASE(ps),
random_generator(ps.getUntrackedParameter<int>("RandomSeed", 7764563)){
    method_name = ps.getParameter<string>("method");
    if(method_name == "random")
        this->Method = method_random;
    else if(method_name == "genmatch")
        this->Method = method_genmatch;
}

BJetAssigner::~BJetAssigner() {
}

bool BJetAssigner::Run(TopAnalysis::TTBarDileptonicEvent* ev){
    switch(this->Method){
        case method_random:
            if(random_generator.Uniform(0.0,1.0) > 0.5){
                ev->Top_Rec.b.SetXYZT( ev->Jets[0].X() , ev->Jets[0].Y() , ev->Jets[0].Z() , ev->Jets[0].T()   );
                ev->TopBar_Rec.b.SetXYZT( ev->Jets[1].X() , ev->Jets[1].Y() , ev->Jets[1].Z() , ev->Jets[1].T()   );
            }else{
                ev->Top_Rec.b.SetXYZT( ev->Jets[1].X() , ev->Jets[1].Y() , ev->Jets[1].Z() , ev->Jets[1].T()   );
                ev->TopBar_Rec.b.SetXYZT( ev->Jets[0].X() , ev->Jets[0].Y() , ev->Jets[0].Z() , ev->Jets[0].T()   );
            }
            return true;
        default:
            throw ("method " + method_name + " is not implemented yet") ;
    }
    return false;
}
