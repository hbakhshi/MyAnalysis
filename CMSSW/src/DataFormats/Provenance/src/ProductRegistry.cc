/**
   \file
   class impl

   \Original author Stefano ARGIRO
   \Current author Bill Tanenbaum
   \date 19 Jul 2005
*/


#include "DataFormats/Provenance/interface/ProductRegistry.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/ReflexTools.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/WrappedClassName.h"

#include <algorithm>
#include <limits>
#include <sstream>

namespace edm {
  namespace {
    void checkDicts(BranchDescription const& productDesc) {
      if(productDesc.transient()) {
        checkDictionaries(productDesc.fullClassName(), true);
      } else {
        checkDictionaries(wrappedClassName(productDesc.fullClassName()), false);
      }
    }
  }

  ProductRegistry::ProductRegistry() :
      productList_(),
      transients_() {
  }

  ProductRegistry::Transients::Transients() :
      frozen_(false),
      constProductList_(),
      productProduced_(),
      anyProductProduced_(false),
      productLookup_(),
      elementLookup_(),
      branchIDToIndex_(),
      producedBranchListIndex_(std::numeric_limits<BranchListIndex>::max()) {
    for(size_t i = 0; i < productProduced_.size(); ++i) productProduced_[i] = false;
  }

  ProductRegistry::ProductRegistry(ProductList const& productList, bool toBeFrozen) :
      productList_(productList),
      transients_() {
    frozen() = toBeFrozen;
  }

  void
  ProductRegistry::addProduct(BranchDescription const& productDesc,
                              bool fromListener) {
    assert(productDesc.produced());
    throwIfFrozen();
    checkDicts(productDesc);
    std::pair<ProductList::iterator, bool> ret =
         productList_.insert(std::make_pair(BranchKey(productDesc), productDesc));
    if(!ret.second) {
      throw Exception(errors::Configuration, "Duplicate Process")
        << "The process name " << productDesc.processName() << " was previously used on these products.\n"
        << "Please modify the configuration file to use a distinct process name.\n";
    }
    addCalled(productDesc, fromListener);
  }

  void
  ProductRegistry::copyProduct(BranchDescription const& productDesc) {
    assert(!productDesc.produced());
    throwIfFrozen();
    productDesc.init();
    BranchKey k = BranchKey(productDesc);
    ProductList::iterator iter = productList_.find(k);
    if(iter == productList_.end()) {
      productList_.insert(std::make_pair(k, productDesc));
    } else {
      assert(combinable(iter->second, productDesc));
      iter->second.merge(productDesc);
    }
  }

  bool
  ProductRegistry::anyProducts(BranchType brType) const {
    throwIfNotFrozen();
    for(ProductList::const_iterator it = productList_.begin(), itEnd = productList_.end();
        it != itEnd; ++it) {
      if(it->second.branchType() == brType) {
        return true;
      }
    }
    return false;
  }

  void
  ProductRegistry::setFrozen(bool initializeLookupInfo) const {
    if(frozen()) return;
    frozen() = true;
    if(initializeLookupInfo) {
      initializeLookupTables();
    }
  }

  void
  ProductRegistry::throwIfFrozen() const {
    if(frozen()) {
      throw cms::Exception("ProductRegistry", "throwIfFrozen")
        << "cannot modify the ProductRegistry because it is frozen\n";
    }
  }

  void
  ProductRegistry::throwIfNotFrozen() const {
    if(!frozen()) {
      throw cms::Exception("ProductRegistry", "throwIfNotFrozen")
        << "cannot read the ProductRegistry because it is not yet frozen\n";
    }
  }

  void
  ProductRegistry::addCalled(BranchDescription const&, bool) {
  }

  std::vector<std::string>
  ProductRegistry::allBranchNames() const {
    std::vector<std::string> result;
    result.reserve(productList().size());

    ProductList::const_iterator it = productList().begin();
    ProductList::const_iterator end = productList().end();

    for(; it != end; ++it) result.push_back(it->second.branchName());

    return result;
  }

  std::vector<BranchDescription const*>
  ProductRegistry::allBranchDescriptions() const {
    std::vector<BranchDescription const*> result;
    result.reserve(productList().size());

    ProductList::const_iterator it = productList().begin();
    ProductList::const_iterator end = productList().end();

    for(; it != end; ++it) result.push_back(&(it->second));
    return result;
  }

  void
  ProductRegistry::updateFromInput(ProductList const& other) {
    for(ProductList::const_iterator it = other.begin(), itEnd = other.end();
        it != itEnd; ++it) {
      copyProduct(it->second);
    }
  }

  void
  ProductRegistry::updateFromInput(std::vector<BranchDescription> const& other) {
    for(std::vector<BranchDescription>::const_iterator it = other.begin(), itEnd = other.end();
        it != itEnd; ++it) {
      copyProduct(*it);
    }
  }

  std::string
  ProductRegistry::merge(ProductRegistry const& other,
        std::string const& fileName,
        BranchDescription::MatchMode parametersMustMatch,
        BranchDescription::MatchMode branchesMustMatch) {
    std::ostringstream differences;

    ProductRegistry::ProductList::iterator j = productList_.begin();
    ProductRegistry::ProductList::iterator s = productList_.end();
    ProductRegistry::ProductList::const_iterator i = other.productList().begin();
    ProductRegistry::ProductList::const_iterator e = other.productList().end();

    // Loop over entries in the main product registry.
    while(j != s || i != e) {
      if(j != s && j->second.produced()) {
        // Ignore branches just produced (i.e. not in input file).
        ++j;
      } else if(j == s || (i != e && i->first < j->first)) {
        if(i->second.present()) {
          differences << "Branch '" << i->second.branchName() << "' is in file '" << fileName << "'\n";
          differences << "    but not in previous files.\n";
        } else {
          productList_.insert(*i);
        }
        ++i;
      } else if(i == e || (j != s && j->first < i->first)) {
        if(j->second.present() && branchesMustMatch == BranchDescription::Strict) {
          differences << "Branch '" << j->second.branchName() << "' is in previous files\n";
          differences << "    but not in file '" << fileName << "'.\n";
        }
        ++j;
      } else {
        std::string difs = match(j->second, i->second, fileName, parametersMustMatch);
        if(difs.empty()) {
          if(parametersMustMatch == BranchDescription::Permissive) j->second.merge(i->second);
        } else {
          differences << difs;
        }
        ++i;
        ++j;
      }
    }
    initializeLookupTables();
    return differences.str();
  }

  static void
  fillLookup(Reflex::Type const& type,
             ProductTransientIndex const& index,
             ConstBranchDescription const* branchDesc,
             TransientProductLookupMap::FillFromMap& oMap) {
    oMap[std::make_pair(TypeInBranchType(TypeID(type.TypeInfo()),
                                         branchDesc->branchType()),
                                         branchDesc)] = index;
  }

  void ProductRegistry::initializeLookupTables() const {
    constProductList().clear();
    transients_.get().branchIDToIndex_.clear();
    ProductTransientIndex index = 0;

    //NOTE it might be possible to remove the need for this temporary map because the productList is ordered by the
    // BranchKey and for the same C++ class type the BranchKey will sort just like CompareTypeInBranchTypeConstBranchDescription
    typedef TransientProductLookupMap::FillFromMap TempLookupMap;
    TempLookupMap tempProductLookupMap;
    TempLookupMap tempElementLookupMap;

    typedef std::set<std::string> UsedProcessNames;
    UsedProcessNames usedProcessNames;
    for(ProductList::const_iterator i = productList_.begin(), e = productList_.end(); i != e; ++i, ++index) {
      if(i->second.produced()) {
        setProductProduced(i->second.branchType());
      }

      //insert returns a pair<iterator, bool> and we want the address of the ConstBranchDescription that was created in the map
      // this is safe since items in a map always retain their memory address
      ConstBranchDescription const* pBD = &(constProductList().insert(std::make_pair(i->first, ConstBranchDescription(i->second))).first->second);

      transients_.get().branchIDToIndex_[i->second.branchID()] = index;

      usedProcessNames.insert(pBD->processName());

      Reflex::Type type(Reflex::Type::ByName(i->second.className()));
      //only do the following if the data is supposed to be available in the event
      if(i->second.present()) {
        if(not bool(type)) {

          LogWarning("Missing Dictionary") << "Could not find a Reflex dictionary for class '" << i->second.className()
          << "'.  This class was registered as one which is supposed to be held by an Event, LuminosityBlock, or Run but will not be available. "
          "Please check\n"
          " 1) was a Reflex dictionary created for the class,\n"
          " 2) if so was the package with the dictionary linked with all plugins that use that class,\n"
          " 3) the file is from an old release and this data type has been removed from the present release.";
          continue;
        }
        fillLookup(type, index, pBD, tempProductLookupMap);

        if(bool(type)) {
          // Here we look in the object named "type" for a typedef
          // named "value_type" and get the Reflex::Type for it.
          // Then check to ensure the Reflex dictionary is defined
          // for this value_type.
          // I do not throw an exception here if the check fails
          // because there are known cases where the dictionary does
          // not exist and we do not need to support those cases.
          Reflex::Type valueType;
          if((is_RefVector(type, valueType) ||
              is_PtrVector(type, valueType) ||
              is_RefToBaseVector(type, valueType) ||
              value_type_of(type, valueType))
              && bool(valueType)) {

            fillLookup(valueType, index, pBD, tempElementLookupMap);

            // Repeat this for all public base classes of the value_type
            std::vector<Reflex::Type> baseTypes;
            public_base_classes(valueType, baseTypes);

            for(std::vector<Reflex::Type>::iterator iter = baseTypes.begin(),
                iend = baseTypes.end();
                iter != iend;
                ++iter) {
              fillLookup(*iter, index, pBD, tempElementLookupMap);
            }
          }
        }
      }
    }
    productLookup().fillFrom(tempProductLookupMap);
    elementLookup().fillFrom(tempElementLookupMap);
  }

  ProductTransientIndex ProductRegistry::indexFrom(BranchID const& iID) const {
    std::map<BranchID, ProductTransientIndex>::iterator itFind = transients_.get().branchIDToIndex_.find(iID);
    if(itFind == transients_.get().branchIDToIndex_.end()) {
      return kInvalidIndex;
    }
    return itFind->second;
  }

  void ProductRegistry::print(std::ostream& os) const {
    for(ProductList::const_iterator i = productList_.begin(), e = productList_.end(); i != e; ++i) {
      os << i->second << "\n-----\n";
    }
  }
}
