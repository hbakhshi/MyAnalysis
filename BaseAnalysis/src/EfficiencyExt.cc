#include "../interface/EfficiencyExt.h"

TF1*  TF1Wrapper::F = new TF1();
double TF1Wrapper::Get(double x){
    return F->Eval(x);
}
   