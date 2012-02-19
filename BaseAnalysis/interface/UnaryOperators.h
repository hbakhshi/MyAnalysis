/* 
 * File:   UnaryOperators.h
 * Author: hbakhshi
 *
 * Created on January 14, 2009, 12:42 PM
 */

#ifndef _UNARYOPERATORS_H
#define	_UNARYOPERATORS_H

#include "ObjectProperty.h"
#include <string>
#include "ObjectIDHandler.h"
#include <cmath>

using namespace std;

template<class T>
class BaseUnaryOperator : public ObjectProperty<T> {
public:

    BaseUnaryOperator(ObjectProperty<T>* _prop) : ObjectProperty<T>("", "", 0, 0, 0, _prop->ObjectName, 0),
    prop(_prop),returnTheProbH1(false) {
        Function = 0;
    };

    ~BaseUnaryOperator() {
    };

    void Set(string name, string title , double funcID) {
        if(Function != 0){
            string::size_type pos = string::npos;
            string::size_type len(4);
            if( (pos = name.find("{pn}")) == string::npos ){
                name += "_{pn}";
                pos = name.find("{pn}");
            }
            this->PropertyName = name.replace(pos , len , prop->PropertyName);

            if( (pos = title.find("{pt}")) == string::npos ){
                name += "{pt}";
                pos = name.find("{pt}");
            }
            this->PropertyTitle = title.replace(pos , len , prop->PropertyTitle);

            double binSize = (prop->MaxValue - prop->MinValue)/prop->NumberOfBins;
            this->MaxValue = -100000;
            this->MinValue = 100000;
            for (int k = 0; k < prop->NumberOfBins; k++) {
                double dbl = Function(prop->MinValue + k * binSize);

                if(this->MaxValue < dbl)
                    this->MaxValue = dbl;

                if(this->MinValue > dbl)
                    this->MinValue = dbl;
            }

            this->NumberOfBins = int((this->MaxValue - this->MinValue)/binSize);
            this->PropertyID = OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO->FindID(prop->PropertyID, funcID , this->ObjectName, -1, true);
        }
    };

    virtual double ReadValue(const T* obj) const {
        if(Function != 0)
            return Function(prop->ReadValue(obj));
        else
            return NAN;
    };

    double (*Function)(double);
    ObjectProperty<T>* prop;

    bool returnTheProbH1;
    virtual TH1Ext<T>* GetH1(string name, string title = ""){
        if(returnTheProbH1){
            TH1Ext<T>* ret = prop->GetH1(name, title);
            ret->GetXaxis()->SetTitle(this->PropertyTitle.c_str());
            return ret;
        }
        else
            return ObjectProperty<T>::GetH1(name, title);
    }
};

template<class T>
ObjectProperty<T>* sqrt(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::sqrt ;
    ret->Set("sqrt" , "#sqrt{{pt}}" , .1);
    return ret;
}

template<class T>
ObjectProperty<T>* abs(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::fabs ;
    ret->Set("abs" , "#||{{pt}}" , .2);
    return ret;
}

template<class T>
ObjectProperty<T>* sin(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::sin;
    ret->Set("sin" , "sin#(){{pt}}" , .3);
    return ret;
}

template<class T>
ObjectProperty<T>* cos(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::cos;
    ret->Set("cos" , "cos#(){{pt}}" , .4);
    return ret;
}

template<class T>
ObjectProperty<T>* tan(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::tan;
    ret->Set("tan" , "tan#(){{pt}}" , .5);
    return ret;
}

template<class T>
ObjectProperty<T>* log(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::log;
    ret->Set("log" , "log#(){{pt}}" , .6);
    return ret;
}

template<class T>
ObjectProperty<T>* exp(ObjectProperty<T>* prop){
    BaseUnaryOperator<T>* ret = new BaseUnaryOperator<T>(prop);
    ret->Function = std::exp;
    ret->Set("exp" , "e^{{pt}}" , .7);
    return ret;
}

template<class T>
ObjectProperty<T>* power(ObjectProperty<T>* prop , double power){
    ObjectProperty<T>* ret = exp( (*log(prop)) * power);
    std::ostringstream ss_name;
    ss_name << prop->PropertyName << "_power_" << power ;
    string _name = ss_name.str();
    string::size_type point_pos = _name.find(".");
    if(point_pos != string::npos)
        _name = _name.replace(point_pos , 1 , "_" );
    ret->PropertyName = _name;

    std::ostringstream ss_title;
    ss_title << prop->PropertyTitle  << "^{" << power << "}" ;
    ret->PropertyTitle = ss_title.str();

    return ret;
}

#endif	/* _UNARYOPERATORS_H */

