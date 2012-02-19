/* 
 * File:   EleIsoTemplate.h
 * Author: hbakhshi
 *
 * Created on June 30, 2010, 5:55 PM
 */

#ifndef _ELEISOTEMPLATE_H
#define	_ELEISOTEMPLATE_H

#include "BaseAlgorithm.h"

using namespace std;

class EleIsoTemplate : public BaseAlgorithm {
public:
    typedef BaseAlgorithm base;

    EleIsoTemplate(string name, string ctr, TreeReaderEvent* tree);    
    
    virtual ~EleIsoTemplate();

    string MakeString();
    vector<FakeInfo> GetAll();

    virtual vector<Object> GetFakableObjs();

    bool PARAM_ConvRejAll;
    bool PARAM_ConvRejEle;
    int PARAM_EtaRegion;
    int PARAM_GenFilter;

    string FakableElectronTag;
    string ElectronTag;
protected:
    virtual void AnalyzeArgs();
private:
};

#endif	/* _ELEISOTEMPLATE_H */

