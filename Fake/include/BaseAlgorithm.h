/* 
 * File:   BaseAlgorithm.h
 * Author: hbakhshi
 *
 * Created on June 30, 2010, 3:33 PM
 */

#ifndef _BASEALGORITHM_H
#define	_BASEALGORITHM_H

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <stdio.h>
#include <limits>
#include <stdlib.h>

#include "DataFormats/interface/TreeReaderEvent.h"

#include <TH1.h>

#include "BaseAnalysis/interface/ObjectProperty.h"
#include "BaseAnalysis/interface/EfficiencyHandler.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace std;

class BaseAlgorithm {
public:

    struct Object {
        double pt;
        double eta;
        double phi;

        int index;

        Object() {
            pt = 0;
            eta = 0;
            phi = 0;

            index = -1;
        }

        Object(double PT, double Eta, double Phi, int idx) {
            pt = PT;
            eta = Eta;
            phi = Phi;

            index = idx;
        }
    };

    class FakeInfo {
    public:
        Object FakableObject;
        Object Electron;
        bool hasEle;

        Object Probabilities;
        double coeff1;
        double coeff2;
        double coeff3;

        static bool HasEle(const FakeInfo* ff) {
            return ff->hasEle;
        };

        class All_Property : public ObjectProperty<FakeInfo> {
        public:
            typedef FakeInfo T;
            typedef ObjectProperty<T> BASE;

            All_Property(string objName) : BASE("All", "Total", 0, 1.0, 1, objName, 4) {
            };

            virtual ~All_Property() {

            }

            virtual double ReadValue(const T*) const {
                return 0.5;
            }
        };

        class Phi_Property : public ObjectProperty<FakeInfo> {
        public:
            typedef FakeInfo T;
            typedef ObjectProperty<T> BASE;

            Phi_Property(string objName) : BASE("Phi", "#phi", -CLHEP::pi, CLHEP::pi, 21, objName, 3) {
            };

            virtual ~Phi_Property() {

            }

            virtual double ReadValue(const T* obj) const {
                return obj->FakableObject.phi;
            }
        };

        class PT_Property : public ObjectProperty<FakeInfo> {
        public:
            typedef FakeInfo T;
            typedef ObjectProperty<T> BASE;
            vector<double> bins;

            PT_Property(string objName, vector<double>* _bins = NULL) : BASE("Pt", "P_t", 0, 500, 50, objName, 1) {
                if (_bins != NULL) {
                    bins.assign(_bins->begin(), _bins->end());
                    this->PropertyName = "PtBinned";
                    this->PropertyID = 101;
                    this->NumberOfBins = bins.size();
                }
            };

            virtual ~PT_Property() {

            }

            virtual double ReadValue(const T* obj) const {
                return obj->FakableObject.pt;
            };

            virtual TH1Ext<T>* GetH1(string name, string title = "") {
                TH1Ext<T>* ret = BASE::GetH1(name, title);
                if (bins.size() > 0) {
                    double bins_[100];
                    for (int ii = 0; ii < bins.size(); ii++)
                        bins_[ii] = bins[ii];

                    ret->SetBins(bins.size() - 1, bins_);
                }
                return ret;
            }

            virtual TH2Ext<FakeInfo>* GetH2(ObjectProperty<FakeInfo>* otherProperty, string name, string title = "") {
                TH2Ext<T>* ret = BASE::GetH2(otherProperty, name, title);
                if (bins.size() > 0) {
                    double bins_Y[100];
                    TH1* hOther = otherProperty->GetH1(name, title);
                    hOther->GetXaxis()->GetLowEdge(bins_Y);
                    bins_Y[hOther->GetNbinsX()] = hOther->GetXaxis()->GetXmax();
                    int nbins_yyy = hOther->GetNbinsX();
                    delete hOther;

                    double bins_[100];
                    for (int ii = 0; ii < bins.size(); ii++)
                        bins_[ii] = bins[ii];

                    ret->SetBins(bins.size() - 1, bins_, nbins_yyy, bins_Y);
                }
                return ret;
            }
        };

        class Eta_Property : public ObjectProperty<FakeInfo> {
        public:
            typedef FakeInfo T;
            typedef ObjectProperty<T> BASE;
            vector<double> bins;
            bool ABSoluteValue;

            Eta_Property(string objName, bool endcap_barrel = false, bool abs = false) : BASE("Eta", "#eta", -2.5, 2.5, 20, objName, 2), ABSoluteValue(abs) {
                if (endcap_barrel) {
                    double bins_[] = {-2.5, -1.566, -1.4442, 0.0, 1.4442, 1.566, 2.5};
                    if (ABSoluteValue) {
                        bins.assign(bins_ + 3, bins_ + 7);
                        this->PropertyName = "ABS_EndcapBarrel";
                        this->PropertyTitle = "|#eta|";
                        this->PropertyID = 102;
                        this->NumberOfBins = bins.size();
                    } else {
                        bins.assign(bins_, bins_ + 7);
                        this->PropertyName = "EndcapBarrel";
                        this->PropertyID = 103;
                        this->NumberOfBins = bins.size();
                    }
                } else if (ABSoluteValue) {
                    this->PropertyName = "ABS_Eta";
                    this->PropertyTitle = "|#eta|";
                    this->PropertyID = 104;
                    this->NumberOfBins = 10;
                    this->MinValue = 0.0;
                }
            };

            virtual ~Eta_Property() {

            }

            virtual double ReadValue(const T* obj) const {
                if (ABSoluteValue)
                    return fabs(obj->FakableObject.eta);
                else
                    return obj->FakableObject.eta;
            };

            virtual TH1Ext<T>* GetH1(string name, string title = "") {
                TH1Ext<T>* ret = BASE::GetH1(name, title);
                if (bins.size() > 0) {
                    double bins_[100];
                    for (int ii = 0; ii < bins.size(); ii++)
                        bins_[ii] = bins[ii];

                    ret->SetBins(bins.size() - 1, bins_);
                }
                return ret;
            }

            virtual TH2Ext<FakeInfo>* GetH2(ObjectProperty<FakeInfo>* otherProperty, string name, string title = "") {
                TH2Ext<T>* ret = BASE::GetH2(otherProperty, name, title);
                if (bins.size() > 0) {
                    double bins_Y[100];
                    TH1* hOther = otherProperty->GetH1(name, title);
                    hOther->GetXaxis()->GetLowEdge(bins_Y);
                    bins_Y[hOther->GetNbinsX()] = hOther->GetXaxis()->GetXmax();
                    int nbins_yyy = hOther->GetNbinsX();
                    delete hOther;

                    double bins_[100];
                    for (int ii = 0; ii < bins.size(); ii++)
                        bins_[ii] = bins[ii];

                    ret->SetBins(bins.size() - 1, bins_, nbins_yyy, bins_Y);
                }
                return ret;
            }
        };
    };

    //Constructor for the mode of calculation of the Fake Rate
    BaseAlgorithm(string name, string str, TreeReaderEvent* tree);

    virtual string MakeString() = 0;
    virtual vector<FakeInfo> GetAll() = 0;

    virtual vector<Object> GetFakableObjs();
    virtual vector<Object> GetElectrons();

    virtual ~BaseAlgorithm();

    virtual void WriteAll(TDirectory* dir);

    void SetTree(TreeReaderEvent* theTree) {
        this->TheTree = theTree;
    }
    string algoName;
protected:
    string algoParams;
    string foParams;
    string leptonParams;

    struct Cut {
        string objectType;
        string propertyName;

        double minVal;
        double maxVal;

        TLeaf* leaf;
        string type;

        string LeafName() {
            return objectType + propertyName;
        }

        Cut() {
            leaf = NULL;

            maxVal = numeric_limits<int>::max();
            minVal = numeric_limits<int>::min();
        }
    };
    vector<Cut> FO_Cuts;
    vector<Cut> Lep_Cuts;

    TreeReaderEvent* TheTree;

    vector<string> SplitString(string Str, const char* delim) {
        vector<string> ret;
        char str[Str.length()];
        //std::cout << "  " ;//Str << "  " << string(delim) << std::endl;
        strcpy(str, Str.c_str());
        char * pch = strtok(str, delim);
        while (pch != NULL) {
            ret.push_back(string(pch));
            pch = strtok(NULL, delim);
        }
        return ret;
    }
    bool PassCuts(vector<Cut>* cuts, int id);
    virtual int ParseCuts(string, vector<Cut>*, string objName);

    void FillAll(vector<FakeInfo>* info);

    EfficiencyHandler< FakeInfo >* FakeRates;
    EfficiencyH1<FakeInfo>* effTotal;

    enum {
        Calculation = 1
    } ObjectMode;

protected:

    BaseAlgorithm() {
    }; //just for inheritence !
};

#endif	/* _BASEALGORITHM_H */

