/* 
 * File:   TreeCreator.h
 * Author: hbakhshi
 *
 * Created on December 30, 2008, 4:42 PM
 */

#ifndef _TREECREATOR_H
#define	_TREECREATOR_H

#include <string>
#include <vector>
#include <map>
#include <TFile.h>
#include <TTree.h>
#include <BaseAnalysis/interface/ObjectProperty.h>

using namespace std;

template<int numberOfElements, class T>
class TreeCreator {
public:
    typedef ObjectProperty<T>* LeafInfo;

    float Values[numberOfElements];

    TreeCreator(string treeName);

    virtual ~TreeCreator();

    virtual LeafInfo AddLeaf(LeafInfo leaf);

    virtual void CreateBranch(string _branchName = "AllInfo");

    int Fill(const T*);

    TTree* Get();

    void Write(TDirectory*);
protected:
    virtual void PreFill();

    TTree* tree;

    string LeafList;
    int NumberOfLeafsAdded;
    map<int, LeafInfo> Readers;
    vector<int> Readers_ids;
    string branchName;
};

template<int numberOfElements, class T>
TreeCreator<numberOfElements, T>::TreeCreator(string treeName) : NumberOfLeafsAdded(0) {
    tree = new TTree(treeName.c_str(), treeName.c_str());
}

template<int numberOfElements, class T>
TreeCreator<numberOfElements, T>::~TreeCreator() {

}

template<int numberOfElements, class T>
void TreeCreator<numberOfElements, T>::CreateBranch(string _branchName) {
    branchName = _branchName;
    tree->Branch(branchName.c_str(), &Values, LeafList.c_str());
}

template<int numberOfElements, class T>
TTree* TreeCreator<numberOfElements, T>::Get() {
    return tree;
}

template<int numberOfElements, class T>
void TreeCreator<numberOfElements, T>::Write(TDirectory* dir) {
    dir->cd();
    tree->Write();
}

template<int numberOfElements, class T>
void TreeCreator<numberOfElements, T>::PreFill() {
}

template<int numberOfElements, class T>
int TreeCreator<numberOfElements, T>::Fill(const T* obj) {
    for (uint i = 0; i < Readers_ids.size(); i++) {
        int reader_id = Readers_ids[i];
        LeafInfo reader = Readers[reader_id];
        Values[reader_id] = reader->ReadValue(obj);
    }

    PreFill();

    return tree->Fill();
}

template<int numberOfElements, class T>
ObjectProperty<T>* TreeCreator<numberOfElements, T>::AddLeaf(LeafInfo leaf) {
    if (NumberOfLeafsAdded < numberOfElements) {
        Readers.insert(make_pair(NumberOfLeafsAdded, leaf));
        Readers_ids.push_back(NumberOfLeafsAdded);
        LeafList += leaf->PropertyName + "/F";
        NumberOfLeafsAdded++;
        if (NumberOfLeafsAdded != numberOfElements)
            LeafList += ":";
    }
    return leaf;
}

#endif	/* _TREECREATOR_H */

