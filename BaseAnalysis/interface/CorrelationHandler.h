/* 
 * File:   CorrelationHandler.h
 * Author: hbakhshi
 *
 * Created on December 3, 2008, 11:34 AM
 */

#ifndef _CORRELATIONHANDLER_H
#define	_CORRELATIONHANDLER_H

#include "ObjectProperty.h"
#include <vector>
#include <TH2.h>
#include <string>
#include <iostream>
#include <iosfwd>
#include <sstream>
#include <TPad.h>
#include "TCanvas.h"
#include <TPaveText.h>
#include <TFrame.h>
#include <TMatrixTSym.h>
#include <TMatrixDSymEigen.h>

using namespace std;

template<class T>
class CorrelationHandler {
public:

    CorrelationHandler(string _name) {
        this->name = _name;
        NameOfVariables = NULL;
    };

    template<class OP>
    ObjectProperty<T>* AddProperty();
    ObjectProperty<T>* AddProperty(ObjectProperty<T>*);

    void Add(const T* theObj, double weight = 1) {
        TH2* h;
        for (uint i = 0; i < Properties.size() - 1; i++) {
            for (uint j = i + 1; j < Properties.size(); j++) {
                ObjectProperty<T>* prop_i = Properties.at(i);
                ObjectProperty<T>* prop_j = Properties.at(j);

                if ((h = GetHisto(prop_i, prop_j)) != NULL)
                    h->Fill(prop_i->ReadValue(theObj), prop_j->ReadValue(theObj), weight);
                else if ((h = GetHisto(prop_j, prop_i)) != NULL)
                    h->Fill(prop_j->ReadValue(theObj), prop_i->ReadValue(theObj), weight);
                else {
                    cout << "The element " << i << " and " << j << " cannot be found!(" << name << " says!)" << endl;
                    throw new std::exception();
                }
            }
        }
    };

    TMatrixTSym<double>* GetCovarianceMatrix() {
        TMatrixTSym<double> * ret = new TMatrixTSym<double>(Properties.size());
        TH2D* h;

        for (int i = 0; i < ret->GetNrows() - 1; i++) {
            TMatrixTRow<double> elem = (*ret)[i];
            for (int j = i + 1; j < ret->GetNcols(); j++) {
                ObjectProperty<T>* prop_i = Properties.at(i);
                ObjectProperty<T>* prop_j = Properties.at(j);

                if ((h = GetHisto(prop_i, prop_j)) != NULL)
                    elem[j] = h->GetCovariance();
                else if ((h = GetHisto(prop_j, prop_i)) != NULL)
                    elem[j] = h->GetCovariance();
            }
        }

        for (int i = 0; i < ret->GetNrows(); i++) {
            TMatrixTRow<double> elem = (*ret)[i];
            double rms_i = -1;

            ObjectProperty<T>* prop_i = Properties.at(i);
            ObjectProperty<T>* prop_iP = (i == ret->GetNrows() - 1) ? NULL : Properties.at(i + 1);
            ObjectProperty<T>* prop_iM = (i == 0) ? NULL : Properties.at(i - 1);

            if ((h = GetHisto(prop_i, prop_iP)) != NULL)
                rms_i = h->GetRMS(1);
            else if ((h = GetHisto(prop_iP, prop_i)) != NULL)
                rms_i = h->GetRMS(2);
            if ((h = GetHisto(prop_i, prop_iM)) != NULL)
                rms_i = h->GetRMS(1);
            else if ((h = GetHisto(prop_iM, prop_i)) != NULL)
                rms_i = h->GetRMS(2);

            if (rms_i < 0)
                elem[i] = -1;
            else
                elem[i] = rms_i*rms_i;
        }



        for (int j = 0; j < ret->GetNrows(); j++) {
            TMatrixTRow<double> elem = (*ret)[j];
            for (int i = 0; i < j; i++) {
                ObjectProperty<T>* prop_i = Properties.at(i);
                ObjectProperty<T>* prop_j = Properties.at(j);

                if ((h = GetHisto(prop_i, prop_j)) != NULL)
                    elem[i] = h->GetCovariance();
                else if ((h = GetHisto(prop_j, prop_i)) != NULL)
                    elem[i] = h->GetCovariance();
            }
        }

        return ret;
    };

    TMatrixTSym<double>* GetCorrelationMatrix() {
        TMatrixTSym<double> * ret = new TMatrixTSym<double>(Properties.size());
        TH2D* h;

        for (int i = 0; i < ret->GetNrows() - 1; i++) {
            TMatrixTRow<double> elem = (*ret)[i];
            for (int j = i + 1; j < ret->GetNcols(); j++) {
                ObjectProperty<T>* prop_i = Properties.at(i);
                ObjectProperty<T>* prop_j = Properties.at(j);

                if ((h = GetHisto(prop_i, prop_j)) != NULL)
                    elem[j] = h->GetCorrelationFactor();
                else if ((h = GetHisto(prop_j, prop_i)) != NULL)
                    elem[j] = h->GetCorrelationFactor();
            }
        }

        for (int i = 0; i < ret->GetNrows(); i++) {
            TMatrixTRow<double> elem = (*ret)[i];
            elem[i] = 1;
        }


        for (int j = 0; j < ret->GetNrows(); j++) {
            TMatrixTRow<double> elem = (*ret)[j];
            for (int i = 0; i < j; i++) {
                ObjectProperty<T>* prop_i = Properties.at(i);
                ObjectProperty<T>* prop_j = Properties.at(j);

                if ((h = GetHisto(prop_i, prop_j)) != NULL)
                    elem[i] = h->GetCorrelationFactor();
                else if ((h = GetHisto(prop_j, prop_i)) != NULL)
                    elem[i] = h->GetCorrelationFactor();
            }
        }

        return ret;
    }

    TCanvas* MakeCanvas();

    const TMatrixT<double>* GetEigenVectors(){
        TMatrixDSymEigen eigens(*GetCovarianceMatrix());
        return (new TMatrixT<double>(eigens.GetEigenVectors()));
    }

    vector<ObjectProperty<T>*>* GetUncorrelatedVariables(){
        vector<ObjectProperty<T>*>* ret = new vector<ObjectProperty<T>*>();
        const TMatrixT<double>* eigens = GetEigenVectors();

        for (uint i = 0; i < Properties.size(); i++) {
            ObjectProperty<T>* prop = *(Properties.at(0)) * ((*eigens)(0 , i)) ;
            for (int j = 1; j < eigens->GetNcols(); j++)
                prop = *(*(Properties.at(j)) * ((*eigens)(j , i))) + prop ;

            ret->push_back(prop);
        }

        return ret;
    }

    CorrelationHandler<T>* GetUncorrelatedCH(){
        vector<ObjectProperty<T>*>* newVars = GetUncorrelatedVariables();
        
        CorrelationHandler<T>* ret = new CorrelationHandler<T>(name + "_Uncorrelated");
        
        TPaveText* txtNameOfVariables = new TPaveText(0.0, 0.0, 1.0, 1.0);
        
        for (uint i = 0; i < newVars->size(); i++){
            std::ostringstream ss_name;
            ss_name << "Variable_" << i + 1;

            std::ostringstream ss_title;
            ss_title << "V_{" << i+1 << "}";

            std::ostringstream ss_draw;
            ss_draw << "V_{" << i+1 << "} = " << newVars->at(i)->PropertyTitle;

            TText* txt = txtNameOfVariables->AddText(ss_draw.str().c_str());
            txt->SetTextColor(kBlack);

            newVars->at(i)->PropertyName = ss_name.str();
            newVars->at(i)->PropertyTitle = ss_title.str();

            ret->AddProperty(newVars->at(i));
        }

        ret->NameOfVariables = new TCanvas();
        txtNameOfVariables->Draw();

        return ret;
    }

    void WriteAll(TDirectory* currentDir = 0 , bool writeEigens = false) {

        TDirectory* wd;
        if (currentDir != NULL && name != "")
            (wd = currentDir->mkdir(name.c_str()))->cd();
        else
            wd = currentDir;

        GetCorrelationMatrix()->Write("CorrelationMatrix");
        GetCovarianceMatrix()->Write("CovarianceMatrix");
        TCanvas* c = MakeCanvas();
        c->Write("TheCanvas");;

        if(writeEigens)
            GetEigenVectors()->Write("EigenVectors_Covariance");

        if(NameOfVariables != NULL){
            NameOfVariables->Write("NameOfVariables");
        }
        
        for (IntIntTH2::const_iterator itr = theHistos.begin(); itr != theHistos.end(); itr++) {
            itr->second->Write();
        }


        if (currentDir != NULL && name != "") {
            currentDir->cd();
        }
    };

    vector<ObjectProperty<T>*> Properties;
    string name;
    
private:
    typedef std::map<std::pair<int, int>, TH2D*> IntIntTH2;
    IntIntTH2 theHistos;

    TCanvas* NameOfVariables ;     

    TH2D* GetHisto(ObjectProperty<T>* first, ObjectProperty<T>* second) {
        if ((first == NULL) || (second == NULL))
            return NULL;

        int i = first->PropertyID;
        int j = second->PropertyID;

        for (IntIntTH2::const_iterator itr = theHistos.begin(); itr != theHistos.end(); itr++) {
            std::pair<int, int> first = itr->first;
            if (first.first == i && first.second == j)
                return itr->second;
        }
        return NULL;
    };

    void MakeHisto(ObjectProperty<T>* first, ObjectProperty<T>* second) {
        int i = first->PropertyID;
        int j = second->PropertyID;

        if (i == j)
            return;

        if ((GetHisto(first, second) == NULL) && (GetHisto(second, first) == NULL)) {
            TH2D* h = new TH2D(MakeHistoName(first, second).c_str(),
                    MakeHistoTitle(first, second).c_str(),
                    first->NumberOfBins, first->MinValue, first->MaxValue,
                    second->NumberOfBins, second->MinValue, second->MaxValue);

            h->SetXTitle(first->PropertyTitle.c_str());
            h->SetYTitle(second->PropertyTitle.c_str());

            theHistos.insert(pair< pair<int, int>, TH2D*>(std::pair<int, int>(i, j), h));
        }
    };

    string MakeHistoName(ObjectProperty<T>* first, ObjectProperty<T>* second) {
        string name1 = first->PropertyName;
        string name2 = second->PropertyName;

        return (name + "_" + name1 + "VS" + name2);
    };

    string MakeHistoTitle(ObjectProperty<T>* first, ObjectProperty<T>* second) {
        string name1 = first->PropertyName;
        string name2 = second->PropertyName;

        return (name1 + " Versus " + name2);
    };
};

template<class T>
template<class OP>
ObjectProperty<T>* CorrelationHandler<T>::AddProperty() {
    ObjectProperty<T>* prop = new OP();
    return AddProperty(prop);
}

template<class T>
ObjectProperty<T>* CorrelationHandler<T>::AddProperty(ObjectProperty<T>* prop) {
    if(prop->NumberOfBins > 500)
        prop->NumberOfBins = 500;
            
    for (uint i = 0; i < Properties.size(); i++) {
        ObjectProperty<T>* elem = Properties.at(i);
        MakeHisto(prop, elem);
    }

    this->Properties.push_back(prop);
    return prop;
}

template<class T>
TCanvas* CorrelationHandler<T>::MakeCanvas() {
    TCanvas* ret = new TCanvas();
    int nRows(Properties.size());

    TMatrixTSym<double>* covMat = GetCovarianceMatrix();
    TMatrixTSym<double>* corMat = GetCorrelationMatrix();

    TPad* header_labels = new TPad("header", "", 0.05, 0.95, 1, 1);
    header_labels->Divide(nRows, 1);
    header_labels->Draw();

    TVirtualPad* left_labels = new TPad("left", "", 0, 0, 0.05, 0.95);
    left_labels->Divide(1, nRows);
    left_labels->Draw();

    TVirtualPad* c = new TPad("main", "", 0.05, 0.0, 1, 0.95);
    c->Divide(nRows, nRows);
    c->Draw();

    for (int i = 0; i < nRows; i++) {
        ObjectProperty<T>* elem = Properties[i];
        TMatrixTRow<double> covRow_i = (*covMat)[i];
        TMatrixTRow<double> corRow_i = (*corMat)[i];

        int colNumber = i + 1;
        header_labels->cd(colNumber);
        TPaveText* txtLabel = new TPaveText(0.05, 0.05, 0.95, 0.95);
        txtLabel->SetFillColor(kWhite);
        txtLabel->SetBorderSize(1);
        txtLabel->SetTextSize(0.4);

        TText* txt = txtLabel->AddText(elem->PropertyTitle.c_str());
        txtLabel->Draw();

        colNumber = i + 1;
        left_labels->cd(colNumber);
        txtLabel = new TPaveText(0.05, 0.05, 0.95, 0.95);
        txtLabel->SetFillColor(kWhite);
        txtLabel->SetBorderSize(1);
        txtLabel->SetTextSize(0.25);
        
        txt = txtLabel->AddText(elem->PropertyTitle.c_str());
        txt->SetTextAngle(90);
        txtLabel->Draw();
        
        for (int j = i; j < nRows; j++) {
            //to write the value of correlation in the upper triangle
            // and draw the histogram in the lower triangle : 

            txtLabel = new TPaveText(0.05, 0.05, 0.95, 0.95);
            txtLabel->SetFillColor(kWhite);
            txtLabel->SetBorderSize(1);
            //txtLabel->SetCornerRadius(1);
            //txtLabel->SetTextSize(0.3);

            if (i != j) {
                std::ostringstream string_maker;
                string_maker << "Correlation : " << corRow_i[j];
                txtLabel->AddText(string_maker.str().c_str());

                std::ostringstream string_maker2;
                string_maker2 << "Covariance : " << covRow_i[j];
                txtLabel->AddText(string_maker2.str().c_str());

                ObjectProperty<T>* elem_j = Properties[j];

                TH2D* h2d = GetHisto(elem, elem_j);
                if (h2d == NULL)
                    h2d = GetHisto(elem_j, elem);

                if (h2d != NULL) {
                    colNumber = j * nRows + i + 1;
                    c->cd(colNumber);
                    h2d->Draw();
                }
            } else {
                std::ostringstream string_maker;
                string_maker << "Variance : " << covRow_i[j];
                txtLabel->AddText(string_maker.str().c_str());
            }

            colNumber = i * nRows + j + 1;
            c->cd(colNumber);
            txtLabel->Draw();
        }
    }

    return ret;
}
#endif	/* _CORRELATIONHANDLER_H */

