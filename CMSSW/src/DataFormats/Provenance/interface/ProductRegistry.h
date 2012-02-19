#ifndef DataFormats_Provenance_ProductRegistry_h
#define DataFormats_Provenance_ProductRegistry_h

/**
   \file
   Implementation of ProductRegistry

   \original author Stefano ARGIRO
   \current author Bill Tanenbaum
   \date 19 Jul 2005
*/

#include "DataFormats/Provenance/interface/BranchDescription.h"
#include "DataFormats/Provenance/interface/BranchKey.h"
#include "DataFormats/Provenance/interface/BranchListIndex.h"
#include "DataFormats/Provenance/interface/BranchType.h"
#include "DataFormats/Provenance/interface/ConstBranchDescription.h"
#include "DataFormats/Provenance/interface/ProductTransientIndex.h"
#include "DataFormats/Provenance/interface/Transient.h"
#include "DataFormats/Provenance/interface/TransientProductLookupMap.h"

#include "boost/array.hpp"

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace edm {

  /**
     \class ProductRegistry ProductRegistry.h "edm/ProductRegistry.h"

     \brief

     \original author Stefano ARGIRO
     \current author Bill Tanenbaum
     \date 19 Jul 2005
  */
  class ProductRegistry {

  public:
    typedef std::map<BranchKey, BranchDescription> ProductList;

    ProductRegistry();

    // A constructor from the persistent data memebers from another product registry.
    // saves time by not copying the transient components.
    // The constructed registry will be frozen by default.
    explicit ProductRegistry(ProductList const& productList, bool toBeFrozen = true);

    virtual ~ProductRegistry() {}

    typedef std::map<BranchKey, ConstBranchDescription> ConstProductList;

    void addProduct(BranchDescription const& productdesc, bool iFromListener = false);

    void copyProduct(BranchDescription const& productdesc);

    void setFrozen(bool initializeLookupInfo = true) const;

    std::string merge(ProductRegistry const& other,
        std::string const& fileName,
        BranchDescription::MatchMode parametersMustMatch = BranchDescription::Permissive,
        BranchDescription::MatchMode branchesMustMatch = BranchDescription::Permissive);

    void updateFromInput(ProductList const& other);

    void updateFromInput(std::vector<BranchDescription> const& other);

    ProductList const& productList() const {
      //throwIfNotFrozen();
      return productList_;
    }

    ProductList& productListUpdator() {
      throwIfFrozen();
      return productList_;
    }

    // Return all the branch names currently known to *this.  This
    // does a return-by-value of the vector so that it may be used in
    // a colon-initialization list.
    std::vector<std::string> allBranchNames() const;

    // Return pointers to (const) BranchDescriptions for all the
    // BranchDescriptions known to *this.  This does a
    // return-by-value of the vector so that it may be used in a
    // colon-initialization list.
    std::vector<BranchDescription const*> allBranchDescriptions() const;

    //NOTE: this is not const since we only want items that have non-const access to this class to be
    // able to call this internal iteration
    template<typename T>
    void callForEachBranch(T const& iFunc)  {
      //NOTE: If implementation changes from a map, need to check that iterators are still valid
      // after an insert with the new container, else need to copy the container and iterate over the copy
      for(ProductRegistry::ProductList::const_iterator itEntry = productList_.begin(),
          itEntryEnd = productList_.end();
          itEntry != itEntryEnd; ++itEntry) {
        iFunc(itEntry->second);
      }
    }
    ProductList::size_type size() const {return productList_.size();}

    void print(std::ostream& os) const;

    bool anyProducts(BranchType const brType) const;

    ConstProductList& constProductList() const {
       //throwIfNotFrozen();
       return transients_.get().constProductList_;
    }

    TransientProductLookupMap& productLookup() const {return transients_.get().productLookup_;}

    TransientProductLookupMap& elementLookup() const {return transients_.get().elementLookup_;}

    //returns the appropriate ProductTransientIndex else 0xFFFFFFFF if no BranchID is available
    static ProductTransientIndex const kInvalidIndex = 0xFFFFFFFF;
    ProductTransientIndex indexFrom(BranchID const& iID) const;
    struct Transients {
      Transients();
      bool frozen_;
      ConstProductList constProductList_;
      // Is at least one (run), (lumi), (event) product produced this process?
      boost::array<bool, NumBranchTypes> productProduced_;
      bool anyProductProduced_;

      // indices used to quickly find a group in the vector groups_
      // by type, first one by the type of the EDProduct and the
      // second by the type of object contained in a sequence in
      // an EDProduct
      TransientProductLookupMap productLookup_;
      TransientProductLookupMap elementLookup_;

      std::map<BranchID, ProductTransientIndex> branchIDToIndex_;

      BranchListIndex producedBranchListIndex_;
    };

    bool productProduced(BranchType branchType) const {return transients_.get().productProduced_[branchType];}
    bool anyProductProduced() const {return transients_.get().anyProductProduced_;}
    BranchListIndex producedBranchListIndex() const {return transients_.get().producedBranchListIndex_;}

    void setProducedBranchListIndex(BranchListIndex blix) const{
      transients_.get().producedBranchListIndex_ = blix;
    }

  private:
    void setProductProduced(BranchType branchType) const {
      transients_.get().productProduced_[branchType] = true;
      transients_.get().anyProductProduced_ = true;
    }

    bool& frozen() const {return transients_.get().frozen_;}

    void initializeLookupTables() const;
    virtual void addCalled(BranchDescription const&, bool iFromListener);
    void throwIfNotFrozen() const;
    void throwIfFrozen() const;

    ProductList productList_;
    mutable Transient<Transients> transients_;
  };

  inline
  bool
  operator==(ProductRegistry const& a, ProductRegistry const& b) {
    return a.productList() == b.productList();
  }

  inline
  bool
  operator!=(ProductRegistry const& a, ProductRegistry const& b) {
    return !(a == b);
  }

  inline
  std::ostream&
  operator<<(std::ostream& os, ProductRegistry const& pr) {
    pr.print(os);
    return os;
  }

} // edm

#endif
