/* 
 * File:   TH2Ext.h
 * Author: hbakhshi
 *
 * Created on December 2, 2008, 1:25 PM
 */

#ifndef _TH2EXT_H
#define	_TH2EXT_H

#include <TH1.h>
#include "ObjectProperty.h"

template<class T , class Base = TH2D>
class TH2Ext : public Base{
public:
    TH2Ext(const ObjectProperty<T>* The1stProperty ,const ObjectProperty<T>* The2ndProperty) : Base(){
        the1stProperty = The1stProperty;
        the2ndProperty = The2ndProperty;
    };

    int Fill(const T* theObject, double weight = 1){
       return Base::Fill(the1stProperty->ReadValue(theObject) , the2ndProperty->ReadValue(theObject) , weight);
    };

    int GetProp1ID(){
        return the1stProperty->PropertyID;
    };

    int GetProp2ID(){
        return the2ndProperty->PropertyID;
    };

    string GetMixedName(){
        return (the1stProperty->PropertyName + "_" + the2ndProperty->PropertyName);
    };

    short IsIt(int _1st , int _2nd){
        if ((GetProp1ID()==_1st) && (GetProp2ID()==_2nd))
            return 1;
        else if((GetProp1ID()==_2nd) && (GetProp2ID()==_1st))
            return -1;
        else
            return 0;
    }
protected:
    const ObjectProperty<T>* the1stProperty;
    const ObjectProperty<T>* the2ndProperty;
};

#endif	/* _TH2EXT_H */

