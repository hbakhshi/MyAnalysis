/* 
 * File:   IntLumHandler.h
 * Author: hbakhshi
 *
 * Created on March 10, 2010, 5:15 PM
 */

#ifndef _INTLUMHANDLER_H
#define	_INTLUMHANDLER_H

#include <exception>
#include <string>
#include <vector>
#include <sstream>

#include "TDirectory.h"
#include "TMath.h"

using namespace std;

class InvalidNumbers : public std::exception {
public:
    string WHAT;

    InvalidNumbers(string wht) : WHAT(wht) {
    };

    virtual ~InvalidNumbers() throw () {
    };

    virtual const char* what() const throw () {
        return WHAT.c_str();
    }
};

template<class Filler, class Object>
class IntLumHandler {
public:

    IntLumHandler(double xsec, double workig_intLum, int Total, string name_, vector<double>* args, int MIN_TO_SPLIT = 20) : XSec(xsec),
    Working_IntLum(workig_intLum),
    TotalNEvents(Total),
    Name(name_),
    CurrentEvent(0) {
        int NIt = NIterations();
        if (NIt == 0)
            throw InvalidNumbers("These events are not suitable for this intLuminocity");

        if (NIt > MIN_TO_SPLIT)
            for (int i = 0; i < NIt; i++) {
                std::stringstream name;
                name << Name << "_" << Working_IntLum << "_" << i;
                Filler f_i(name.str(), args);
                Fillers.push_back(f_i);
            } else {
            std::stringstream name;
            name << Name << "_" << Working_IntLum << "_" << 0;
            Filler f_i(name.str(), args);
            Fillers.push_back(f_i);
        }
    };

    virtual ~IntLumHandler() {
    };

    double NEventsNeeded() {
        return XSec*Working_IntLum;
    };

    int NIterations() {
        return int(floor(TotalNEvents / NEventsNeeded()));
    };

    Filler* CurrentFiller() {
        int current_id = int(floor(CurrentEvent / NEventsNeeded()));
        if (current_id < int(Fillers.size()))
            return &(Fillers.at(current_id));
        else
            return NULL;
    }

    void Fill(const Object* obj) {
        if (obj != NULL) {
            Filler* filler = CurrentFiller();
            if (filler != NULL) {
                filler->Fill(obj);
            }
        }
        CurrentEvent++;
    };

    void Write(TDirectory* dir) {
        std::stringstream name;
        name << Name << "_" << Working_IntLum;
        TDirectory* newDir = dir->mkdir(name.str().c_str());
        newDir->cd();

        vector<double> val_mean;
        vector<double> val_min;
        vector<double> val_max;
        vector<double> err_mean;

        int varid;
        for (varid = 0; varid <this->NVariables(); varid++) {
            val_max.push_back( -10000. );
            val_min.push_back(10000. );
            val_mean.push_back(0.0 );
            err_mean.push_back(0.0);
        }

        for (uint i = 0; i < Fillers.size(); i++) {
            Filler* elem = &(Fillers[i]);
            elem->WriteAll(newDir);

            for (varid = 0; varid < this->NVariables(); varid++) {
                val_mean[varid] += elem->Value(varid);

                if (elem->Value(varid) > val_max[varid])
                    val_max[varid] = elem->Value(varid);
                if (elem->Value(varid) < val_min[varid])
                    val_min[varid] = elem->Value(varid);

                err_mean[varid] += elem->Error(varid);
            }
        }


        newDir->cd();

        for (varid = 0; varid < this->NVariables(); varid++) {
            val_mean[varid] /= Fillers.size();
            err_mean[varid] /= Fillers.size();

            double distance = val_max[varid] - val_min[varid];


            VAL_Init.push_back( val_mean[varid] );
            ERR_Init.push_back( err_mean[varid] );

            if (Fillers.size() > 1) {

                string hName = "hTemplate_" + this->GetVariableName(varid);
                string hTitle = "Template Results (" + this->GetVariableName(varid) + ")";

                TH1* h = new TH1D(hName.c_str(), hTitle.c_str(), 12, val_min[varid] - (distance / 10.), val_max[varid] + (distance / 10.));
                for (uint s = 0; s < Fillers.size(); s++)
                    h->Fill(Fillers[s].Value(varid));


                string fName = "fGaussian" + this->GetVariableName(varid);
                TF1* f = new TF1(fName.c_str(), "[0]*Gaus(x,[1],[2])", h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());

                f->SetParName(0, "Coeff");
                f->SetParameter("Coeff", Fillers.size() * TMath::Gaus(0., 0., err_mean[varid], kTRUE));
                f->SetParName(1, "Value");
                f->SetParameter("Value", val_mean[varid]);
                f->SetParName(2, "Error");
                f->SetParameter("Error", err_mean[varid]);


                h->Fit(f, "Q");
                h->Fit(f, "Q");
                h->Fit(f, "Q");

                VAL_.push_back( f->GetParameter("Value") );
                ERR_.push_back(f->GetParameter("Error") );

                h->Write();
                f->Write();
            } else {
                VAL_.push_back( val_mean[varid] );
                ERR_.push_back( err_mean[varid] );
            }
        }

        dir->cd();
    };

    double XSec;
    double Working_IntLum;
    double TotalNEvents;
    string Name;
    int CurrentEvent;

    std::vector<Filler> Fillers;

    int NVariables() {
        return Fillers.begin()->NVariables();
    }

    string GetVariableName(int i) {
        return Fillers.begin()->GetVariableName(i);
    }

    vector<double> VAL_Init;

    double ValueInit(int varid = 0) {
        return VAL_Init[varid];
    };
    vector<double> ERR_Init;

    double ERRInit(int varid = 0) {
        return ERR_Init[varid];
    };

    vector<double> VAL_;

    double Value(int varid = 0) {
        return VAL_[varid];
    };
    vector<double> ERR_;

    double ERR(int varid = 0) {
        return ERR_[varid];
    };
};

template<class Filler, class Object , class elements = IntLumHandler<Filler, Object> >
class IntLumis : public std::vector<elements> {
public:
    //typedef IntLumHandler<Filler, Object> elements;
    typedef std::vector < elements > base;

    IntLumis(double xsec, int Total, string name_, int MIN_TO_SPLIT_ = 20) : XSec(xsec),
    TotalNEvents(Total),
    Name(name_),
    MIN_TO_SPLIT(MIN_TO_SPLIT_) {

    };

    void AddLumi(double Lumi, vector<double>* args = NULL) {
        elements elem(XSec, Lumi, TotalNEvents, Name, args, MIN_TO_SPLIT);
        base::push_back(elem);
    }

    void Fill(const Object* obj) {
        for (uint i = 0; i < base::size(); i++) {
            elements* elem = &(base::at(i));

            elem->Fill(obj);
        }
    }

    virtual void Write(TDirectory* dir) {
        if (base::size() == 0)
            return;

        TDirectory* newDir = dir->mkdir(Name.c_str());
        newDir->cd();

        vector<TH1*> hCompareInits;
        vector<TH1*> hCompareFits;

        int NVars = base::at(0).NVariables();
        for (int varid = 0; varid < NVars; varid++) {
            string name = base::at(0).GetVariableName(varid);

            string hName = name + "_hCompareInit";
            string hTitle = name + " vs. Int. Lumi (Mean values)";
            hCompareInits.push_back(new TH1D(hName.c_str(), hTitle.c_str(), base::size(), 0, base::size()) );

            hName += name + "_hCompareFit";
            hTitle = name + " vs. Int. Lumi (Fit values)";
            hCompareFits.push_back(new TH1D(hName.c_str(), hTitle.c_str(), base::size(), 0, base::size()) );
        }

        for (uint i = 0; i < base::size();) {
            elements* elem = &(base::at(i));

            elem->Write(newDir);

            newDir->cd();

            std::stringstream bin_name;
            bin_name << elem->Working_IntLum;

            i++;
            for (int varid = 0; varid < base::at(0).NVariables(); varid++) {
                hCompareFits[varid]->GetXaxis()->SetBinLabel(i, bin_name.str().c_str());
                hCompareFits[varid]->SetBinContent(i, elem->Value(varid));
                hCompareFits[varid]->SetBinError(i, elem->ERR(varid));

                hCompareInits[varid]->GetXaxis()->SetBinLabel(i, bin_name.str().c_str());
                hCompareInits[varid]->SetBinContent(i, elem->ValueInit(varid));
                hCompareInits[varid]->SetBinError(i, elem->ERRInit(varid));
            }
        }

        for (int varid = 0; varid < base::at(0).NVariables(); varid++) {
            hCompareFits[varid]->Write();
            hCompareInits[varid]->Write();
        }
        dir->cd();
    }

    double XSec;
    double TotalNEvents;
    string Name;
    int MIN_TO_SPLIT;

    ~IntLumis() {
    };
};

#endif	/* _INTLUMHANDLER_H */

