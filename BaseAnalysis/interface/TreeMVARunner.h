/* 
 * File:   TreeMVARunner.h
 * Author: hbakhshi
 *
 * Created on January 1, 2009, 11:37 AM
 */

#ifndef _TREEMVARUNNER_H
#define	_TREEMVARUNNER_H

#include "TreeCreator.h"
#include "TMVA/Factory.h"

template<int numberOfElements, class T>
class TreeMVARunner : public TreeCreator<numberOfElements, T> {
public:
    typedef ObjectProperty<T>* LeafInfo;
    typedef TreeCreator<numberOfElements, T> BASE;

    //To create a tree on the fly
    TreeMVARunner(string treeName);
    //To import an existing tree and use it
    TreeMVARunner(TTree*);

    virtual ~TreeMVARunner();

    virtual void AddLeaf(LeafInfo leaf, bool useInMVA = true);

    void AddLeaf(string name);

    virtual void CreateBranch(TFile*,string _branchName = "AllInfo");

    void BookMVAMethod(TMVA::Types::EMVA type, string name, string options);

    void RunMVA(string signalCondition, bool runTest = false, bool evaluate = false);

    vector<string> methodNames;
protected:
    TMVA::Factory * mvaFactory;
    vector<LeafInfo> Readers_UseInMVA;
};

template<int numberOfElements, class T>
TreeMVARunner<numberOfElements, T>::TreeMVARunner(string treeName) : BASE(treeName) {
}

template<int numberOfElements, class T>
TreeMVARunner<numberOfElements, T>::~TreeMVARunner() {
}

template<int numberOfElements, class T>
void TreeMVARunner<numberOfElements, T>::AddLeaf(LeafInfo leaf, bool useInMVA) {
    BASE::AddLeaf(leaf);
    if (useInMVA)
        Readers_UseInMVA.push_back(leaf);
}

template<int numberOfElements, class T>
void TreeMVARunner<numberOfElements, T>::CreateBranch(TFile* file , string _branchName ) {
    BASE::CreateBranch(_branchName);

    string fileName(BASE::tree->GetName());
    fileName += ".root";
    //TFile* file = new TFile(fileName.c_str(), "RECREATE");
    mvaFactory = new TMVA::Factory(BASE::tree->GetName(), file);


    for (uint i = 0; i < Readers_UseInMVA.size(); i++) {
        string name(_branchName);
        name += "." + Readers_UseInMVA[i]->PropertyName;
        mvaFactory->AddVariable(name);
    }
    //return file;
}

template<int numberOfElements, class T>
void TreeMVARunner<numberOfElements, T>::BookMVAMethod(TMVA::Types::EMVA type, string name, string options) {
    if (mvaFactory != NULL) {
        mvaFactory->BookMethod(type, name.c_str(), options.c_str());
        string __name(BASE::tree->GetName());
        __name += "_" + name + ".weights.txt";
        methodNames.push_back(__name);
    }
}

template<int numberOfElements, class T>
void TreeMVARunner<numberOfElements, T>::RunMVA(string signalCondition, bool runTest , bool evaluate ) {
    if (mvaFactory != NULL) {
        signalCondition = BASE::branchName + "." + signalCondition;
        string bkgCondition("!(");
        bkgCondition += signalCondition;
        bkgCondition += ")";

        mvaFactory->SetInputTrees(BASE::tree, signalCondition.c_str(), bkgCondition.c_str());

#if ROOT_VERSION_CODE >= ROOT_VERSION(5, 15, 0)
        TVirtualTreePlayer* tplayer = tree->GetPlayer();
        int nSignal = tplayer->GetEntries(signalCondition.c_str());
        int nBkg = tplayer->GetEntries(bkgCondition.c_str());

        mvaFactory->PrepareTrainingAndTestTree("", nSignal, nBkg, 1, 1, "SplitMode=Block:!V");
#else
        mvaFactory->PrepareTrainingAndTestTree("", -1);
#endif

        mvaFactory->TrainAllMethods();

        if (runTest)
            mvaFactory->TestAllMethods();

        if (evaluate)
            mvaFactory->EvaluateAllMethods();
    }
}

#endif	/* _TREEMVARUNNER_H */

