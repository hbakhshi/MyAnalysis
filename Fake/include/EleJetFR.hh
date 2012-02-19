/* 
 * File:   EleJetFR.hh
 * Author: hbakhshi
 *
 * Created on July 2, 2010, 7:02 PM
 */

#ifndef _ELEJETFR_HH
#define	_ELEJETFR_HH

#include <stdlib.h>
#include "BaseAlgorithm.h"

using namespace std;

class EleJetFR : public BaseAlgorithm {
public:
    typedef BaseAlgorithm base;

    EleJetFR(string name, string ctr, TreeReaderEvent* tree);

    virtual ~EleJetFR();

    string MakeString();

    vector<FakeInfo> GetAll();
protected:
    virtual void AnalyzeArgs(vector<Cut>*) {};
private:
    double PARAM_DR_MIN;
    double PARAM_DR_MAX;
};

#endif	/* _ELEJETFR_HH */

