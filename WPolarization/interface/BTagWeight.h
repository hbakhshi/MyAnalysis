/* 
 * File:   BTagWeight.h
 * Author: hbakhshi
 *
 * Created on April 5, 2012, 1:40 PM
 */

#ifndef BTAGWEIGHT_H
#define	BTAGWEIGHT_H

#include "TF1.h"
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

class BTagWeight {
public:

    struct JetInfo {

        JetInfo(float mceff, float datasf, float t = 0.) : eff(mceff), sf(datasf), tag(t) {
        }
        double eff;
        double sf;
        int tag;
    };

    BTagWeight(int nTaggers = 1) : taggers(nTaggers) {
    }

    virtual bool filter(vector<int> tags);
    double weight(vector<vector<JetInfo> > jets);

    static void GetEffSF_TCHEL(double pt, double eta, double discriminator_value, double& eff, double& sf);
    
private:
    int taggers;

};
#endif	/* BTAGWEIGHT_H */

