/* 
 * File:   TreeMVAReader.h
 * Author: hbakhshi
 *
 * Created on January 1, 2009, 11:46 AM
 */

#ifndef _TREEMVAREADER_H
#define	_TREEMVAREADER_H

#include <map>
#include "TreeCreator.h"
#include "TMVA/Reader.h"

using namespace std;

template<int numberOfElements, class T>
class TreeMVAReader : public TreeCreator<numberOfElements, T> {
public:
    typedef TreeCreator<numberOfElements, T> BASE;

    TreeMVAReader(string treeName) : BASE(treeName) {
    };

    virtual ~TreeMVAReader() {
    };

    virtual void AddLeaf(LeafInfo leaf, bool useInMVA = true) {
        BASE::AddLeaf(leaf);
        if (useInMVA)
            Readers_UseInMVA.insert(make_pair(NumberOfLeafsAdded - 1, leaf));
    }

    void AddMVALeaf(string name, string FileName) {
        if (NumberOfLeafsAdded < numberOfElements) {

            pair<string, string> str_pair = make_pair<string, string > (name, FileName);
            mvaMethods.insert(make_pair(NumberOfLeafsAdded, str_pair));

            LeafList += name + "/F";
            NumberOfLeafsAdded++;
            if (NumberOfLeafsAdded != numberOfElements)
                LeafList += ":";
        }
    };

    virtual void CreateBranch(string _branchName = "AllInfo") {
        BASE::CreateBranch(_branchName);

        mvaReader = new TMVA::Reader();
        for (map<int, LeafInfo>::const_iterator itr = Readers_UseInMVA.begin(); itr != Readers_UseInMVA.end(); itr++) {
            string name(branchName);
            name += "." + itr->second->PropertyName;
            mvaReader->AddVariable(name.c_str(), &(Values[itr->first]));
        }

        map<int, pair<string, string> >::const_iterator method_itr = mvaMethods.begin();
        for (; method_itr != mvaMethods.end(); method_itr++) {
            int i = method_itr->first;
            pair<string, string> info = method_itr->second;
            mvaReader->BookMVA(info.first, info.second);
        }
    };

protected:

    virtual void PreFill() {
        map<int, pair<string, string> >::const_iterator method_itr = mvaMethods.begin();
        for (; method_itr != mvaMethods.end(); method_itr++) {
            int i = method_itr->first;
            pair<string, string> info = method_itr->second;
            Values[i] = mvaReader->EvaluateMVA(info.first);
        }
    };

    TMVA::Reader* mvaReader;
    map<int, LeafInfo> Readers_UseInMVA;
    map<int, pair<string, string> > mvaMethods;

};


#endif	/* _TREEMVAREADER_H */

