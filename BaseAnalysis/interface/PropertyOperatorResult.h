/* 
 * File:   PropertyOperatorResult.h
 * Author: hbakhshi
 *
 * Created on December 17, 2008, 3:12 PM
 */

#ifndef _PROPERTYOPERATORRESULT_H
#define	_PROPERTYOPERATORRESULT_H

#include "ObjectProperty.h"
#include "ObjectIDHandler.h"
#include <sstream>
#include <cmath>

template<class T>
class PropertyOperatorResult : public ObjectProperty<T> {
public:

    enum Operand {
        Plus,
        Minus,
        Divide,
        Multiply
    };

    PropertyOperatorResult(const ObjectProperty<T> * _prop1, const ObjectProperty<T> * _prop2, Operand _O) : ObjectProperty<T>("", "", 0, 0, 0, "", 0) {
        O = _O;
        prop1 = _prop1;
        prop2 = _prop2;

        if (OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO == 0)
            OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO = new OBJECT_ID_INFO_COLLECTION();

        mode = true;
        Set();
    };

    PropertyOperatorResult(const ObjectProperty<T> * _prop1, double _number, Operand _O) : ObjectProperty<T>("", "", 0, 0, 0, "", 0) {
        O = _O;
        prop1 = _prop1;
        number = _number;

        if (OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO == 0)
            OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO = new OBJECT_ID_INFO_COLLECTION();

        mode = false;
        Set_ByNumber();
    };

    ~PropertyOperatorResult() {
    };

    virtual double ReadValue(const T* t) const {
        if (mode)
            switch (O) {
                case Plus:
                    return prop1->ReadValue(t) + prop2->ReadValue(t);
                case Minus:
                    return prop1->ReadValue(t) - prop2->ReadValue(t);
                case Divide:{
                    double val2 = prop2->ReadValue(t);
                    if (val2 == 0)
                        return signbit(prop1->ReadValue(t))*100;
                    else
                        return (prop1->ReadValue(t) / prop2->ReadValue(t));
                }
                case Multiply:
                    return prop1->ReadValue(t) * prop2->ReadValue(t);
            } else
            switch (O) {
                case Plus:
                    return prop1->ReadValue(t) + number;
                case Minus:
                    return prop1->ReadValue(t) - number;
                case Divide:{
                    double val2 = number;
                    if (val2 == 0)
                        return signbit(prop1->ReadValue(t))*100;
                    else
                        return (prop1->ReadValue(t) / val2);
                }
                case Multiply:
                    return prop1->ReadValue(t) * number;
            }

        return -100;
    };
private:
    const ObjectProperty<T> * prop1;
    const ObjectProperty<T> * prop2;
    double number;
    bool mode; // if true the operation is between two property and if is false then it is between a number and a property
    Operand O;

    void Set_ByNumber() {
        std::ostringstream ss_name;
        std::ostringstream ss_title;

        float ___iii;
        int prec = 2;
        if(std::modf(number , &___iii) == 0.0)
            prec = 0;

        ss_name.precision(0);
        ss_title.precision(0);

        switch (O) {
            case Plus:
                this->MaxValue = this->prop1->MaxValue + number;
                this->MinValue = this->prop1->MinValue + number;
                this->NumberOfBins = prop1->NumberOfBins;

                ss_name << this->prop1->PropertyName << "_Plus_" << fixed <<number;
                this->PropertyName = ss_name.str();

                ss_title << this->prop1->PropertyTitle << " + " << fixed << number;
                this->PropertyTitle = ss_title.str();
                break;
            case Minus:
                this->MaxValue = this->prop1->MaxValue - number;
                this->MinValue = this->prop1->MinValue - number;
                this->NumberOfBins = prop1->NumberOfBins;

                ss_name << this->prop1->PropertyName << "_Minus_" << fixed << number;
                this->PropertyName = ss_name.str();

                ss_title << this->prop1->PropertyTitle << " - " << fixed << number;
                this->PropertyTitle = ss_title.str();
                break;
            case Multiply:
                if (number > 0) {
                    this->MaxValue = this->prop1->MaxValue * number;
                    this->MinValue = this->prop1->MinValue * number;
                } else {
                    this->MinValue = this->prop1->MaxValue * number;
                    this->MaxValue = this->prop1->MinValue * number;
                }

                this->NumberOfBins = int(prop1->NumberOfBins * std::fabs(number));

                ss_name << this->prop1->PropertyName << "_Multiply_" << fixed << number;
                this->PropertyName = ss_name.str();

                ss_title << this->prop1->PropertyTitle << " #times " << fixed << number;
                this->PropertyTitle = ss_title.str();
                break;
            case Divide:
                if (number > 0) {
                    this->MaxValue = this->prop1->MaxValue / number;
                    this->MinValue = this->prop1->MinValue / number;
                } else {
                    this->MinValue = this->prop1->MaxValue / number;
                    this->MaxValue = this->prop1->MinValue / number;
                }

                this->NumberOfBins = prop1->NumberOfBins;

                ss_name << this->prop1->PropertyName << "_Divide_" <<  fixed << number;
                this->PropertyName = ss_name.str();

                ss_title << "#frac{" << this->prop1->PropertyTitle << "}{" << fixed << number << "}";
                this->PropertyTitle = ss_title.str();
                break;
        }

        this->ObjectName = this->prop1->ObjectName;
        this->PropertyID = OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO->FindID(prop1->PropertyID, number, this->ObjectName, short(O), true);
    };

    void Set() {
        double binsize1 = (this->prop1->MaxValue - this->prop1->MinValue) / double(this->prop1->NumberOfBins);
        double binsize2 = (this->prop2->MaxValue - this->prop2->MinValue) / double(this->prop2->NumberOfBins);
        double binsizeMin = min<double>(binsize1, binsize2);


        double maxmax = 0;
        double maxmin = 0;
        double minmax = 0;
        double minmin = 0;

        switch (O) {
            case Plus:
                this->MaxValue = this->prop1->MaxValue + this->prop2->MaxValue;
                this->MinValue = this->prop1->MinValue + this->prop2->MinValue;
                this->NumberOfBins = int((this->MaxValue - this->MinValue) / binsizeMin);
                this->PropertyName = this->prop1->PropertyName + "_Plus_" + this->prop2->PropertyName;
                this->PropertyTitle = this->prop1->PropertyTitle + " + " + this->prop2->PropertyTitle;
                break;
            case Minus:
                maxmax = this->prop1->MaxValue - this->prop2->MaxValue;
                maxmin = this->prop1->MaxValue - this->prop2->MinValue;
                minmax = this->prop1->MinValue - this->prop2->MaxValue;
                minmin = this->prop1->MinValue - this->prop2->MinValue;


                this->MaxValue = max<double>(maxmax, max<double>(maxmin, max<double>(minmax, minmin)));
                this->MinValue = min<double>(maxmax, min<double>(maxmin, min<double>(minmax, minmin)));
                this->NumberOfBins = int((this->MaxValue - this->MinValue) / binsizeMin);
                this->PropertyName = this->prop1->PropertyName + "_Minus_" + this->prop2->PropertyName;
                this->PropertyTitle = this->prop1->PropertyTitle + " - " + this->prop2->PropertyTitle;
                break;
            case Divide:
                maxmax = (prop2->MaxValue == 0) ? signbit(prop1->MaxValue)*100 : this->prop1->MaxValue / this->prop2->MaxValue;
                maxmin = (prop2->MinValue == 0) ? signbit(prop1->MaxValue)*100 : this->prop1->MaxValue / this->prop2->MinValue;
                minmax = (prop2->MaxValue == 0) ? signbit(prop1->MinValue)*100 : this->prop1->MinValue / this->prop2->MaxValue;
                minmin = (prop2->MinValue == 0) ? signbit(prop1->MinValue)*100 : this->prop1->MinValue / this->prop2->MinValue;


                this->MaxValue = max<double>(maxmax, max<double>(maxmin, max<double>(minmax, minmin)));
                this->MinValue = min<double>(maxmax, min<double>(maxmin, min<double>(minmax, minmin)));
                this->NumberOfBins = int((this->MaxValue - this->MinValue) / binsizeMin);
                this->PropertyName = this->prop1->PropertyName + "_Divide_" + this->prop2->PropertyName;
                this->PropertyTitle = "#frac{" + this->prop1->PropertyTitle + "}{" + this->prop2->PropertyTitle + "}";
                break;
            case Multiply:
                maxmax = this->prop1->MaxValue * this->prop2->MaxValue;
                maxmin = this->prop1->MaxValue * this->prop2->MinValue;
                minmax = this->prop1->MinValue * this->prop2->MaxValue;
                minmin = this->prop1->MinValue * this->prop2->MinValue;

                this->MaxValue = max<double>(maxmax, max<double>(maxmin, max<double>(minmax, minmin)));
                this->MinValue = min<double>(maxmax, min<double>(maxmin, min<double>(minmax, minmin)));
                this->NumberOfBins = int((this->MaxValue - this->MinValue) / binsizeMin);
                this->PropertyName = this->prop1->PropertyName + "_Multiply_" + this->prop2->PropertyName;
                this->PropertyTitle = this->prop1->PropertyTitle + " #times " + this->prop2->PropertyTitle;
                break;
        }

        this->ObjectName = this->prop1->ObjectName;
        this->PropertyID = OBJECT_ID_INFO_COLLECTION::SINGLETON_OBJECT_ID_INFO->FindID(prop1->PropertyID, prop2->PropertyID, this->ObjectName, short(O));
    };
};

#endif	/* _PROPERTYOPERATORRESULT_H */

