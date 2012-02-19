#include "DataFormats/Provenance/interface/BranchIDListHelper.h"

#include "DataFormats/Provenance/interface/BranchIDListRegistry.h"
#include "DataFormats/Provenance/interface/ProductRegistry.h"
#include "FWCore/Utilities/interface/Algorithms.h"

namespace edm {

  BranchIDListHelper::BranchIDListHelper() :
    branchIDToIndexMap_(),
    branchListIndexMapper_() {}

  bool
  BranchIDListHelper:: updateFromInput(BranchIDLists const& bidlists, std::string const& fileName) {
    bool unchanged = true;
    BranchIDListRegistry& breg = *BranchIDListRegistry::instance();
    BranchIDListRegistry::collection_type& bdata = breg.data();
    BranchIDToIndexMap& branchIDToIndexMap = breg.extra().branchIDToIndexMap_;
    BranchListIndexMapper& branchListIndexMapper = breg.extra().branchListIndexMapper_;
    branchListIndexMapper.clear();
    typedef BranchIDLists::const_iterator Iter;
    typedef BranchIDListRegistry::const_iterator RegIter;
    for(Iter it = bidlists.begin(), itEnd = bidlists.end(); it != itEnd; ++it) {
      BranchListIndex oldBlix = it - bidlists.begin();
      RegIter j = find_in_all(bdata, *it);
      BranchListIndex blix = j - bdata.begin();
      if(j == bdata.end()) {
        breg.insertMapped(*it);
        for(BranchIDList::const_iterator i = it->begin(), iEnd = it->end(); i != iEnd; ++i) {
          ProductIndex pix = i - it->begin();
          branchIDToIndexMap.insert(std::make_pair(*i, std::make_pair(blix, pix)));
        }
      }
      branchListIndexMapper.insert(std::make_pair(oldBlix, blix));
      if(oldBlix != blix) {
        unchanged = false;
      }
    }
    return unchanged;
  }

  void
  BranchIDListHelper::updateRegistries(ProductRegistry const& preg) {
    BranchIDList bidlist;
    // Add entries for current process for ProductID to BranchID mapping.
    for(ProductRegistry::ProductList::const_iterator it = preg.productList().begin(), itEnd = preg.productList().end();
        it != itEnd; ++it) {
      if(it->second.produced()) {
        if(it->second.branchType() == InEvent) {
          bidlist.push_back(it->second.branchID().id());
        }
      }
    }
    BranchIDListRegistry& breg = *BranchIDListRegistry::instance();
    BranchIDToIndexMap& branchIDToIndexMap = breg.extra().branchIDToIndexMap_;
    if(!bidlist.empty()) {
      BranchListIndex blix = breg.data().size();
      preg.setProducedBranchListIndex(blix);
      breg.insertMapped(bidlist);
      for(BranchIDList::const_iterator i = bidlist.begin(), iEnd = bidlist.end(); i != iEnd; ++i) {
        ProductIndex pix = i - bidlist.begin();
        branchIDToIndexMap.insert(std::make_pair(*i, std::make_pair(blix, pix)));
      }
    }
  }

  void
  BranchIDListHelper::fixBranchListIndexes(BranchListIndexes& indexes) {
    BranchIDListRegistry& breg = *BranchIDListRegistry::instance();
    BranchListIndexMapper& branchListIndexMapper = breg.extra().branchListIndexMapper_;
    for(BranchListIndexes::iterator i = indexes.begin(), e = indexes.end(); i != e; ++i) {
      *i = branchListIndexMapper[*i];
    }
  }

  void
  BranchIDListHelper::clearRegistries() {
    BranchIDListRegistry& breg = *BranchIDListRegistry::instance();
    breg.data().clear();
    breg.extra().branchIDToIndexMap_.clear();
    breg.extra().branchListIndexMapper_.clear();
  }
}
