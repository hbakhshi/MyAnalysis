/* 
 * File:   Lepton.h
 * Author: hbakhshi
 *
 * Created on November 4, 2010, 8:43 PM
 */

#ifndef LEPTON_H
#define	LEPTON_H

class Lepton {
public:

    Lepton() : index_in_tree(-1){
    };

    float Px;
    float Py;
    float Pz;
    float Pt;
    float E;
    float Eta;
    float Theta;
    float Phi;
    float D0BS;
    float D0PV;
    float D0E;
    float DzBS;
    float DzPV;
    float DzE;
    int Charge;

    int GenID;
    int GenStatus;
    int GenCharge;
    float GenPt;
    float GenEta;
    float GenPhi;
    float GenE;
    int GenMID;
    int GenMStatus;
    int GenMCharge;
    float GenMPt;
    float GenMEta;
    float GenMPhi;
    float GenME;
    int GenGMID;
    int GenGMStatus;
    int GenGMCharge;
    float GenGMPt;
    float GenGMEta;
    float GenGMPhi;
    float GenGME;

    virtual math::XYZTLorentzVector get4Vector(int mode) const {
        math::XYZTLorentzVector ret;
        if(mode == 0)
            ret.SetXYZT(Px, Py, Pz, E);        

        return ret;
    }

    virtual bool isMuon() {return false;};
    virtual bool isElectron() {return false;};
    
    int index_in_tree;
};

#endif	/* LEPTON_H */

