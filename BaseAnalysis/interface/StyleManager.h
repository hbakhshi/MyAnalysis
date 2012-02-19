/* 
 * File:   StyleManager.h
 * Author: hbakhshi
 *
 * Created on January 29, 2009, 7:45 PM
 */

#ifndef _STYLEMANAGER_H
#define	_STYLEMANAGER_H

#include "TH1.h"
#include "TStyle.h"
#include "TRandom.h"

class StyleManager {
public:
    //option = 1 : fill ; 2 : line

    static TH1 * SetStyle(TH1* h, int k, short option = 1) {
        int fillStyle = -1;
        short fillColor = -1;





	//h->SetLineWidth(1);
        switch (k) {
            case 0:
                h->SetLineWidth(2);
                fillStyle = 3454;
                fillColor = (short) 46;
                break;
            case 1:
                fillStyle = 3445;
                fillColor = (short) 30;
                break;
            case 2:
                fillColor = 38;
                break;
            case 3:
                fillColor = 40;
                break;
            case 4:
                fillColor = 47;
                break;
            case 5:
                fillColor = 14;
                break;
            case 6:
                fillColor = 32;
                break;
            case 7:
                fillColor = short(kBlack);
                break;
            default:
                break;
        }
        if (fillStyle < 0)
            fillStyle = (330 + k) *10 + k;
        if (fillColor < 0)
            fillColor = short(gRandom->Uniform(3, 50));


        h->SetFillColor(kWhite);
        h->SetFillStyle(0);
        h->SetLineColor(fillColor);
//        h->SetFillAttributes();
//        h->SetLineAttributes();

        if (option == 1) {
            h->SetFillColor(fillColor);
            h->SetFillStyle(fillStyle);
//            h->SetFillAttributes();
        } else if (option == 2) {
            //Nothing to do ...
        }
        return h;
    };
};

#endif	/* _STYLEMANAGER_H */

