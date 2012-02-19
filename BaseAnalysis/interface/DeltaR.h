/* 
 * File:   DeltaR.h
 * Author: ajafari
 *
 * Created on July 10, 2009, 9:40 AM
 */

#ifndef _DELTAR_H
#define	_DELTAR_H
#include "math.h"
class DeltaR{
public:
    explicit DeltaR(){};
    ~DeltaR(){};
    double getValue(double delEta, double delPhi){
        double dPhi = fabs(delPhi);
        if(dPhi>M_PI)
            dPhi = 2*M_PI-dPhi;
        return(sqrt(dPhi*dPhi+delEta*delEta));
    }
};


#endif	/* _DELTAR_H */

