/* 
 * File:   TreeReaderEvent.h
 * Author: hbakhshi
 *
 * Created on February 1, 2011, 1:50 PM
 */

#ifndef TREEREADEREVENT_H
#define	TREEREADEREVENT_H

#include "NTupleAnalyzer/include/base/TreeReader.hh"
#include <map>

class TreeReaderEvent : public TreeReader {
public:
    TreeReaderEvent(TreeReader* tree) : TreeReader(*tree){};
    TreeReaderEvent(TTree *tree = NULL) : TreeReader(tree){HTJets=0.0;};
    virtual ~TreeReaderEvent(){};

    std::map< std::string, std::vector<int> > ElectronsToAnalyze;
    std::map< std::string, std::vector<int> > MuonsToAnalyze;

    std::vector<int> GoodJets;
    double HTJets;
    void Print() const{
    }
};

#endif	/* TREEREADEREVENT_H */

