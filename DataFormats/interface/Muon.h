/* 
 * File:   Muon.h
 * Author: hbakhshi
 *
 * Created on November 4, 2010, 9:26 PM
 */

#ifndef MUON_H
#define	MUON_H

#include "Lepton.h"

namespace ElectronAnalysis {

    class Muon : public Lepton {
    public:

        int MuGood; //[NMus]
        int MuIsIso; //[NMus]
        int MuIsGlobalMuon; //[NMus]
        int MuIsTrackerMuon; //[NMus]
        float MuInnerTkPt; //[NMus]
        float MuPtE; //[NMus]
        float MuEt; //[NMus]
        float MuRelIso03; //[NMus]
        float MuIso03SumPt; //[NMus]
        float MuIso03EmEt; //[NMus]
        float MuIso03HadEt; //[NMus]
        float MuIso03EMVetoEt; //[NMus]
        float MuIso03HadVetoEt; //[NMus]
        float MuIso05SumPt; //[NMus]
        float MuIso05EmEt; //[NMus]
        float MuIso05HadEt; //[NMus]
        float MuEem; //[NMus]
        float MuEhad; //[NMus]
        float MuNChi2; //[NMus]
        int MuNGlHits; //[NMus]
        int MuNMuHits; //[NMus]
        int MuNTkHits; //[NMus]
        int MuNPxHits; //[NMus]
        float MuInnerTkNChi2; //[NMus]
        int MuNMatches; //[NMus]
        int MuNChambers; //[NMus]
        float MuCaloComp; //[NMus]
        float MuSegmComp; //[NMus]
        int MuIsGMPT; //[NMus]
        int MuIsGMTkChiComp; //[NMus]
        int MuIsGMStaChiComp; //[NMus]
        int MuIsGMTkKinkTight; //[NMus]
        int MuIsAllStaMuons; //[NMus]
        int MuIsAllTrkMuons; //[NMus]
        int MuIsTrkMuonArbitrated; //[NMus]
        int MuIsAllArbitrated; //[NMus]
        int MuIsTMLSLoose; //[NMus]
        int MuIsTMLSTight; //[NMus]
        int MuIsTM2DCompLoose; //[NMus]
        int MuIsTM2DCompTight; //[NMus]
        int MuIsTMOneStationLoose; //[NMus]
        int MuIsTMOneStationTight; //[NMus]
        int MuIsTMLSOptLowPtLoose; //[NMus]
        int MuIsTMLSAngLoose; //[NMus]
        int MuIsTMLastStationAngTight; //[NMus]
        int MuIsTMOneStationAngTight; //[NMus]
        int MuIsTMOneStationAngLoose; //[NMus]

        virtual bool isMuon() {
            return true;
        };

        virtual double IsolationValue() {
            double iso = (MuIso03SumPt + MuIso03EmEt + MuIso03HadEt);
            iso /= std::max(Pt, 20.0f);
            return iso;
        };

    };

}
#endif	/* MUON_H */

