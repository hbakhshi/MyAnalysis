/* 
 * File:   EfficiencyHandlerReader.h
 * Author: hbakhshi
 *
 * Created on February 25, 2011, 5:33 PM
 */

#ifndef EFFICIENCYHANDLERREADER_H
#define	EFFICIENCYHANDLERREADER_H

#include "ObjectProperty.h"

#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphAsymmErrors.h"

using namespace std;

struct Efficiency1DObjects {
public:
    TH1* hAll;
    TH1* hPassed;
    TH1* hEff;
    TGraphAsymmErrors* ASymmGraph;

    void Print() {
        hAll->Print();
        hPassed->Print();
        hEff->Print();

        ASymmGraph->Print();
    }
};

struct Efficiency2DObjects {
    TH2* hAll;
    TH2* hPassed;
    TH2* hEff;

    void Print() {
        hAll->Print();
        hPassed->Print();
        hEff->Print();
    }
};

class EfficiencyHandlerReaderException : public std::exception {
public:

    EfficiencyHandlerReaderException(string msg) throw () : message(msg) {
    };

    virtual ~EfficiencyHandlerReaderException() throw () {
    };

    virtual const char* what() const throw () {
        return ("EfficiencyHandlerReader : " + message).c_str();
    }
private:
    string message;
};

template<class T>
class EfficiencyHandlerReader {
public:

    EfficiencyHandlerReader(TDirectory* dir, string Name);
    ~EfficiencyHandlerReader();

    Efficiency1DObjects Get(ObjectProperty<T>*)throw (EfficiencyHandlerReaderException);
    Efficiency2DObjects Get(ObjectProperty<T>*, ObjectProperty<T>*)throw (EfficiencyHandlerReaderException);
private:
    TDirectory* Directory;
    string Name;
};

template<class T>
EfficiencyHandlerReader<T>::EfficiencyHandlerReader(TDirectory* dir, string name) {
    this->Name = name;

    Directory = dir->GetDirectory(Name.c_str());
}

template<class T>
EfficiencyHandlerReader<T>::~EfficiencyHandlerReader() {
}

template<class T>
Efficiency1DObjects EfficiencyHandlerReader<T>::Get(ObjectProperty<T>* obj) throw (EfficiencyHandlerReaderException){
    TDirectory* dir_tmp = this->Directory->GetDirectory(obj->PropertyName.c_str());
    TList* lis_of_keys = dir_tmp->GetListOfKeys();
    if (dir_tmp == 0){
        EfficiencyHandlerReaderException* exp1 = new EfficiencyHandlerReaderException("No such a directory for " + obj->PropertyName);
        throw *exp1;
    }
    Efficiency1DObjects ret;
    ret.hAll = dynamic_cast<TH1*> (dir_tmp->Get(lis_of_keys->At(0)->GetName()));
    ret.hPassed = dynamic_cast<TH1*> (dir_tmp->Get(lis_of_keys->At(1)->GetName()));
    ret.ASymmGraph = dynamic_cast<TGraphAsymmErrors*> (dir_tmp->Get(lis_of_keys->At(2)->GetName()));
    ret.hEff = dynamic_cast<TH1*> (dir_tmp->Get(lis_of_keys->At(4)->GetName()));

    return ret;
}

template<class T>
Efficiency2DObjects EfficiencyHandlerReader<T>::Get(ObjectProperty<T>* obj, ObjectProperty<T>* obj2) throw (EfficiencyHandlerReaderException){
    TDirectory* dir_tmp = this->Directory->GetDirectory((obj->PropertyName + "_" + obj2->PropertyName).c_str());
    if (dir_tmp == 0)
        dir_tmp = this->Directory->GetDirectory((obj2->PropertyName + "_" + obj->PropertyName).c_str());

    if (dir_tmp == 0){
        EfficiencyHandlerReaderException* exp1 = new EfficiencyHandlerReaderException("No such a directory for " + obj2->PropertyName + " and " + obj->PropertyName);
        throw *exp1;
    }
    TList * lis_of_keys = dir_tmp->GetListOfKeys();

    Efficiency2DObjects ret;
    ret.hAll = dynamic_cast<TH2*> (dir_tmp->Get(lis_of_keys->At(0)->GetName()));
    ret.hPassed = dynamic_cast<TH2*> (dir_tmp->Get(lis_of_keys->At(1)->GetName()));
    ret.hEff = dynamic_cast<TH2*> (dir_tmp->Get(lis_of_keys->At(2)->GetName()));

    return ret;
}

#endif	/* EFFICIENCYHANDLERREADER_H */

