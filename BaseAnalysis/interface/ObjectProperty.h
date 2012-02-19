/* 
 * File:   IObjectProperty.h
 * Author: hbakhshi
 *
 * Created on December 2, 2008, 11:23 AM
 */

#ifndef _IOBJECTPROPERTY_H
#define	_IOBJECTPROPERTY_H

#include <string>
#include <TH1.h>
#include <TH2.h>
#include "TH1Ext.h"
#include "TH2Ext.h"
#include "PropertyOperatorResult.h"
#include "UnaryOperators.h"

using namespace std;

template<class T>
class ObjectProperty {
public:

    ObjectProperty(string PN, string PT, double min, double max, int NBins, string ON, int ID) : ObjectName(ON),
    PropertyID(ID),
    PropertyName(PN),
    PropertyTitle(PT),
    MinValue(min),
    MaxValue(max),
    NumberOfBins(NBins) {

    };

    virtual ~ObjectProperty() {
    };

    virtual void SetAxis(TAxis* axis) {
    };

    string ObjectName;
    int PropertyID;
    string PropertyName;
    string PropertyTitle;
    double MinValue;
    double MaxValue;
    int NumberOfBins;

    virtual double ReadValue(const T*) const = 0;

    virtual TH1Ext<T>* GetH1(string name, string title = "");

    virtual TH2Ext<T>* GetH2(ObjectProperty<T>* otherProperty, string name, string title = "");

    template<class OtherProperty>
    TH2Ext<T>* GetH2(string name, string title = "");

    //Operators

    ObjectProperty<T> * operator +(const ObjectProperty<T>* ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Plus));
    };

    ObjectProperty<T> * operator -(const ObjectProperty<T>* ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Minus));
    };

    ObjectProperty<T> * operator *(const ObjectProperty<T>* ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Multiply));
    };

    ObjectProperty<T> * operator /(const ObjectProperty<T>* ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Divide));
    };

    ObjectProperty<T> * operator +(double ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Plus));
    };

    ObjectProperty<T> * operator -(double ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Minus));
    };

    ObjectProperty<T> * operator *(double ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Multiply));
    };

    ObjectProperty<T> * operator /(double ObjToAdd) {
        return (new PropertyOperatorResult<T > (this, ObjToAdd, PropertyOperatorResult<T>::Divide));
    };
};

template<class T>
TH1Ext<T>* ObjectProperty<T>::GetH1(string name, string title) {
    if (title == "")
        title = name;
    if (name != "") {
        name += ("_" + this->ObjectName + "_" + this->PropertyName);
        //        title += (" (" + this->ObjectName + "," + this->PropertyName + ")" );
    } else {
        name = this->ObjectName + "_" + this->PropertyName;
        //        title = (" (" + this->ObjectName + "," + this->PropertyName + ")" );
    }

    TH1Ext<T>* ret = new TH1Ext<T > (this);

    ret->SetNameTitle(name.c_str(), title.c_str());
    ret->SetBins(this->NumberOfBins, this->MinValue, this->MaxValue);
    ret->SetXTitle(this->PropertyTitle.c_str());

    this->SetAxis(ret->GetXaxis());

    return ret;
}

template<class T>
TH2Ext<T>* ObjectProperty<T>::GetH2(ObjectProperty<T>* otherProperty, string name, string title) {
    if (title == "")
        title = name;

    if (name != "")
        name += ("_" + ObjectName + "_" + PropertyName + "vs" + otherProperty->PropertyName);
    else
        name = ObjectName + "_" + PropertyName + "vs" + otherProperty->PropertyName;

    TH2Ext<T>* ret = new TH2Ext<T > (this, otherProperty);
    ret->SetNameTitle(name.c_str(), title.c_str());
    ret->SetBins(NumberOfBins, MinValue, MaxValue, otherProperty->NumberOfBins, otherProperty->MinValue, otherProperty->MaxValue);

    ret->SetXTitle(PropertyTitle.c_str());
    ret->SetYTitle(otherProperty->PropertyTitle.c_str());

    this->SetAxis(ret->GetXaxis());
    otherProperty->SetAxis(ret->GetYaxis());

    return ret;
}

template<class T>
template<class OtherProperty>
TH2Ext<T>* ObjectProperty<T>::GetH2(string name, string title) {
    ObjectProperty<T> otherProperty = new OtherProperty();
    return GetH2(otherProperty, name, title);
}

template<class arrayType>
class ObjectPropertyFromArray : public ObjectProperty<int> {
public:
    typedef ObjectProperty<int> BASE;

    struct ObjectInfo {
        string ObjectName;
        int PropertyID;
        string PropertyName;
        string PropertyTitle;
        double MinValue;
        double MaxValue;
        int NumberOfBins;

        ObjectInfo(string PN, string PT, double min, double max, int NBins, string ON, int ID) : ObjectName(ON),
        PropertyID(ID),
        PropertyName(PN),
        PropertyTitle(PT),
        MinValue(min),
        MaxValue(max),
        NumberOfBins(NBins){

        };
    };
    class Exception : public exception {
    public:

        string __WHAT;

        Exception(string WHAT) : __WHAT(WHAT) {
        }

        virtual ~Exception() throw () {
        };

        virtual const char* what() const throw () {

            return __WHAT.c_str();
        }
    };


    arrayType* Values;
    int* MaxPossibleIndex;

    ObjectPropertyFromArray(arrayType* valuesHolder, ObjectInfo inf) : BASE(inf.PropertyName, inf.PropertyTitle, inf.MinValue, inf.MaxValue, inf.NumberOfBins, inf.ObjectName, inf.PropertyID) {
        Values = valuesHolder;

        MaxPossibleIndex = NULL;
    }

    ObjectPropertyFromArray(arrayType* valuesHolder, int* maxIndex , ObjectInfo inf) : BASE(inf.PropertyName, inf.PropertyTitle, inf.MinValue, inf.MaxValue, inf.NumberOfBins, inf.ObjectName, inf.PropertyID) {
        Values = valuesHolder;
        MaxPossibleIndex = maxIndex;
    }

    virtual double ReadValue(const int* index) const {
        int idx = *index;

        if(MaxPossibleIndex != NULL)
            if(idx >= *MaxPossibleIndex)
            {
                std::stringstream ss;
                ss << "ObjectPropertyFromArray::ReadValue ==> " << PropertyName << "-" << ObjectName << " : Just  " << *MaxPossibleIndex << " " << ObjectName << "s exist. But " << *index << " is requested.";
                throw Exception(ss.str());
            }

        arrayType val = *(Values + idx);
        return (double) val;
    }
};

typedef ObjectPropertyFromArray<int> ObjectPropertyFromIntArray;
typedef ObjectPropertyFromArray<double> ObjectPropertyFromDblArray;
typedef ObjectPropertyFromArray<float> ObjectPropertyFromFltArray;
#endif	/* _IOBJECTPROPERTY_H */

