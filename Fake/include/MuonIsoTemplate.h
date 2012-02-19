/* 
 * File:   MuonIsoTemplate.h
 * Author: hbakhshi
 *
 * Created on February 9, 2011, 2:38 PM
 */

#ifndef MUONISOTEMPLATE_H
#define	MUONISOTEMPLATE_H

#include "BaseAlgorithm.h"

using namespace std;

class MuonIsoTemplate : public BaseAlgorithm {
public:
    typedef BaseAlgorithm base;

    MuonIsoTemplate(string name, string ctr, TreeReaderEvent* tree);

    virtual ~MuonIsoTemplate();

    string MakeString();
    vector<FakeInfo> GetAll();

    virtual vector<Object> GetFakableObjs();

    string FakableMuonTag;
    string MuonTag;

    int PARAM_EtaRegion;
protected:
    virtual void AnalyzeArgs();
private:
};


#endif	/* MUONISOTEMPLATE_H */

