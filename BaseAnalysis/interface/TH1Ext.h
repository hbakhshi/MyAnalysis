/* 
 * File:   TH1Ext.h
 * Author: hbakhshi
 *
 * Created on December 2, 2008, 1:10 PM
 */

#ifndef _TH1EXT_H
#define	_TH1EXT_H

#include <TH1.h>
#include "ObjectProperty.h"
#include <string>
#include <iostream>

using namespace std;

template<class T>
class ObjectProperty;

template<class T, class Base = TH1D>
class TH1Ext : public Base {
public:

    TH1Ext(const ObjectProperty<T>* theProp) : TH1D() {
        theProperty = theProp;
    };

    virtual int Fill(const T* theObject, double weight = 1) {
        if (theProperty == 0) {
            cout << "NULL !!!";
            throw std::exception();
        } else {
            double val = theProperty->ReadValue(theObject);
            return Base::Fill(val, weight);
        }
    };

    string GetPropertyName() {
        return theProperty->PropertyName;
    };

    string GetPropertyObjectName() {
        return theProperty->ObjectName;
    };

    string GetPropertyTitle() {
        return theProperty->PropertyTitle;
    };

    int GetPropertyID() {
        return theProperty->PropertyID;
    };

    bool IsIt(int propID) {
        return GetPropertyID() == propID;
    };

    double MaxPropVal(){
        return theProperty->MaxValue;
    };

    double MinPropVal(){
        return theProperty->MinValue;
    };

    const ObjectProperty<T>* theProperty;
protected:
};

#endif	/* _TH1EXT_H */

