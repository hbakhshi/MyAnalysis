/* 
 * File:   Predict.h
 * Author: hbakhshi
 *
 * Created on January 16, 2011, 5:49 PM
 */
/* COPIED FROM :
 * File:   Predict.h
 * Author: hbakhshi
 *
 * Created on February 10, 2010, 8:49 PM
 */

#ifndef NUMBERHOLDER
#define	NUMBERHOLDER

#include <string>
#include "DataFormats/interface/DiElectronEvent.h"

using namespace std;

//bool use_match1 = false;

struct numberHolder {
    string Name;
    int nEvents;
    int nPos;
    int nPosGen;
    int nNeg;
    int nNegGen;
    int nGoods;
    int nBads;

    numberHolder(string name);

    void Add(const ElectronAnalysis::DiLepton * ev);

    void Print();
};
//numberHolder nSSEvents("Same Signs");
//
//numberHolder nOSEvents("Opposite Signs");
//
//numberHolder nBSEvents("Bad Signs");
//
//numberHolder nAnals_("Analyzed");
#endif /* NumberHolder */


#ifndef PREDICT_H
#define	PREDICT_H

#include <stddef.h>
#include <map>

#include "DataFormats/interface/ElectronProperties.h"
#include "DataFormats/interface/DiElectronEventProperties.h"

using namespace std;

template<class T>
class Predictor {
public:

    Predictor(string name, double pmm, bool fit = false) : total_pmm(pmm),
    use_fit(fit),
    Name(name) {
    };

    virtual ~Predictor() {
    };

    virtual double Predict(const T*) = 0;

    virtual void SetProp(ObjectProperty<T>*, TH1*) = 0;

    virtual void Finish() = 0;

    double total_pmm;
    bool use_fit;
    string Name;
};

class Predictor1D : public Predictor<ElectronAnalysis::Electron> {
public:
    typedef ElectronAnalysis::Electron T;
    typedef Predictor<T> base;

    Predictor1D(string name, std::vector<double>* args);

    virtual ~Predictor1D();

    virtual double Predict(const T* obj);

    virtual void SetProp(ObjectProperty<T>* op, TH1* h);

    void AddProp(ObjectProperty<T>* op, std::vector<TH1*> h);

    void SetErrorCoeff(double coeff);

    void Finish(bool print, TDirectory* old_dir = NULL);

    virtual void Finish();

    typedef std::map<ObjectProperty<T>*, std::vector<TH1*> > map_prop_histo;
    map_prop_histo props_histos;
    double ErrorCoeff;

    std::vector<double> sumProbs;
    std::vector<double> sumErrs;

    std::vector<double> sumFittedProbs;
    std::vector<double> sumFittedErrs;

    int NBadCharges;
    int NAllElectrons;

    virtual void WriteAll(TDirectory* dir);

    double Value(int VariableID);

    double Error(int VariableID);

    string GetVariableName(int VariableID);

    int NVariables();

    int NHistograms();

    TH1* GetHisto(int histoId, ObjectProperty<T>** obj);

    void Fill(const ElectronAnalysis::DiLepton* evt);
};

#endif	/* PREDICT_H */

#ifndef PREDIT_ANALYZE
#define PREDIT_ANALYZE

#include "PMMReader.h"
#include "DataFormats/interface/DiElectronEvent.h"
#include "NTupleAnalyzer/include/base/Analyzer.h"

class PMMPredict : public Analyzer<ElectronAnalysis::DiLepton> {
public:

    typedef Analyzer<ElectronAnalysis::DiLepton> BASE;

    PMMReader reader_all;
    PMMReader reader_BarrelEndCap;
    PMMReader::ChargeSource ChargeSource;
    bool CTFGSFMatche;
    bool ApplyD0;
    Predictor1D predictor_EE;
    Predictor1D predictor_EM;

    struct Numbers {
        int EM;
        int MM;
        int EE;

        std::multimap<int, int> EventNumbersEM;
        std::multimap<int, int> EventNumbersEE;
        string Name;

        Numbers(string name) {
            EM = 0;
            EE = 0;
            MM = 0;

            Name = name;
        };
        //returns true if the event has at least one electron

        bool Add(int nmuons, int run = 0, int event = 0) {
            if (nmuons == 0) {
                EE++;

                EventNumbersEE.insert(pair<int, int>(run, event));
            } else if (nmuons == 1) {
                EM++;
                EventNumbersEM.insert(pair<int, int>(run, event));
            } else if (nmuons == 2) {
                MM++;
                return false;
            }

            return true;
        };

        void PrintHeader() {
            std::cout << "| Name | MM + EM + EE | SUM " << endl;
        };

        void Print() {
            std::cout << "|" << Name << "|" << MM << "+" << EM << "+" << EE << "|" << MM + EM + EE << endl;
        };
    };

    Numbers NSS;
    Numbers NOS;
    Numbers NFailed;

    PMMPredict(const edm::ParameterSet& ps);

    void OnChangeFile(FileInfo* fi, DataSet* ds);
    bool Run(ElectronAnalysis::DiLepton* ev);
    void End();

    ~PMMPredict() {
    };
};

#endif
