/* 
 * File:   Factory.h
 * Author: hbakhshi
 *
 * Created on July 5, 2010, 8:33 PM
 */

#ifndef _FACTORY_H
#define	_FACTORY_H


#include "BaseAlgorithm.h"
#include "EleIsoTemplate.h"
#include "EleJetFR.hh"

#include "TKey.h"

#include <vector>
#include <exception>
using namespace std;

class Factory {
public:

    class FakeRate_Factory_Exception : public exception {
    public:

        virtual const char* what() const throw () {
            return "The directory is not a standard one, FakeRate_Factory says.";
        }
    };
    static FakeRate_Factory_Exception myex;

    static BaseAlgorithm* FakeRate_Factory(TDirectory* dir, TreeReader* tree);
    static vector<BaseAlgorithm*>* GetAllFakeRates(TDirectory* mainDir);

    static vector<BaseAlgorithm*> allFR;
    static TDirectory* theFile;
};
#endif	/* _FACTORY_H */

