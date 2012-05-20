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

    BTagWeight(int systematics = 0.0, int nTaggers = 1) : //systematics : 0=the exact bsf values ; 1 = 4%B -4%E ; -1 = -4%B 4%
    taggers(nTaggers),
    Systematics(systematics) {
    }

    virtual bool filter(vector<int> tags);
    double weight(vector<vector<JetInfo> > jets);

    static void GetEffSF_TCHEL(double pt, double eta, double discriminator_value, double& eff, double& sf, int Systematics);
    static void GetEffSF_SSVHEM(double pt, double eta, double discriminator_value, double& eff, double& sf, int Systematics);

    int Systematics;
private:
    int taggers;


};
#endif	/* BTAGWEIGHT_H */

