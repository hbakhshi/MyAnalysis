/* 
 * File:   EfficiencyHandler.h
 * Author: ajafari
 *
 * Created on January 30, 2009, 4:56 PM
 */

#ifndef _EFFICIENCYHANDLER_H
#define	_EFFICIENCYHANDLER_H


#include <vector>
#include <string>
#include "EfficiencyH1.h"
#include "EfficiencyH2.h"
#include "EfficiencyExt.h"
#include "ObjectProperty.h"

#include "TAxis.h"

using namespace std;

template<class T, class Efficiency = EfficiencyH1<T>, class Efficiency2 = EfficiencyH2<T> >
class EfficiencyHandler {
public:

    EfficiencyHandler(string name) : Name(name),
    ExtMode(false) {

    };

    //If the instance is created by this constructor, all of the inner eff's use EfficiencyExt

    EfficiencyHandler(string name, ObjectProperty<T>* inside, TF1* _fitFunc, double xmin, double xmax) : Name(name),
    ExtMode(true),
    ExtInside(inside),
    ExtfitFunc(_fitFunc),
    ExtXMin(xmin),
    ExtXMax(xmax) {

    };

    ~EfficiencyHandler() {

    };

    bool (*Condition)(const T*);
    bool (*AllCondition)(const T*);

    template<class ObjProp>
    EfficiencyH1<T>* AddProp1(bool (*Cond)(const T*) = 0, bool (*AllCond)(const T*) = 0) {
        ObjProp* prop = new ObjProp();
        return AddProp(prop, Cond, AllCond);
    }

    template<class ObjProp, class ObjProp2>
    EfficiencyH2<T>* AddProp2(bool (*Cond)(const T*) = 0, bool (*AllCond)(const T*) = 0) {
        ObjectProperty<T>* prop = new ObjProp();
        ObjectProperty<T>* prop2 = new ObjProp2();
        return AddProp2(prop, prop2, Cond, AllCond);
    }

    EfficiencyH1<T>* AddProp(ObjectProperty<T>* prop, bool (*Cond)(const T*) = 0, bool (*AllCond)(const T*) = 0) {
        EfficiencyH1<T>* eff;
        if (!ExtMode) {
            eff = new Efficiency(prop, Name);
        } else {
            eff = new EfficiencyExt<T > (prop, ExtInside, ExtfitFunc, ExtXMin, ExtXMax, Name);
        }

        if (Cond != 0)
            eff->Condition = Cond;
        else
            eff->Condition = Condition;

        if (AllCond != 0)
            eff->AllCondition = AllCond;
        else
            eff->AllCondition = AllCondition;

        allProps.push_back(eff);
        return eff;
    };

    EfficiencyH2<T>* AddProp2(ObjectProperty<T>* prop, ObjectProperty<T>* prop2, bool (*Cond)(const T*) = 0, bool (*AllCond)(const T*) = 0) {

        EfficiencyH2<T>* eff;
        eff = new Efficiency2(prop, prop2, Name);

        if (Cond != 0)
            eff->Condition = Cond;
        else
            eff->Condition = Condition;

        if (AllCond != 0)
            eff->AllCondition = AllCond;
        else
            eff->AllCondition = AllCondition;

        allProps2.push_back(eff);
        return eff;
    };

    void Fill(const T* theObject, double weight = 1) {
        for (uint i = 0; i < allProps.size(); i++)
            allProps[i]->Fill(theObject, weight);
        
        for (uint i = 0; i < allProps2.size(); i++)
            allProps2[i]->Fill(theObject, weight);
    };

    virtual void WriteAll(TDirectory* dir) {
        if (dir == 0)
            return;
        //        std::cout<<"========= "<<Name<<std::endl;
        TDirectory* my_dir = dir->mkdir(Name.c_str());
        my_dir->cd();

        for (uint i = 0; i < allProps.size(); i++)
            allProps[i]->WriteAll(my_dir);

        for (uint i = 0; i < allProps2.size(); i++)
            allProps2[i]->WriteAll(my_dir);

        dir->cd();
    };

    virtual void Print(){};

    uint GetProperties() {
        return allProps.size();
    };

    EfficiencyH1<T>* GetEffH1(int i) {
        return allProps.at(i);
    };

    EfficiencyH2<T>* GetEffH2(int i) {
        return allProps2.at(i);
    };

//    template<class PropType>
//    std::vector<EfficiencyH1<T>*> GetEffByPropType() {
//        std::vector<EfficiencyH1<T>*> ret;
//
//        for (unsigned int i = 0; i < GetProperties(); i++) {
//            EfficiencyH1<T>* e = (Efficiency*) GetEffH1(i);
//            if (e->CheckPropType<PropType>()) {
//                ret.push_back(e);
//            }
//        }
//
//        return ret;
//    }

    string GetName(){
        return this->Name;
    }
protected:
    string Name;
private:
    vector<EfficiencyH1<T>*> allProps;
    vector<EfficiencyH2<T>*> allProps2;


    bool ExtMode;
    //variables needed for ext mode
    ObjectProperty<T>* ExtInside;
    TF1* ExtfitFunc;
    double ExtXMin, ExtXMax;
};

#endif	/* _EFFICIENCYHANDLER_H */

