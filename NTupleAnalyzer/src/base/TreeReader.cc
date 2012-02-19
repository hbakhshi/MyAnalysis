#include "base/TreeReader.hh"
using namespace std;

TreeReader::TreeReader(TTree *tree) : TreeClassBase(tree), fIsChain(0) {
	if( tree == 0 ) cout << "TreeReader ==> No tree!" << endl;
        else if ( tree->InheritsFrom(TChain::Class()) ) fIsChain = true;
}

TreeReader::~TreeReader(){
	//if(!fChain) cout << "TreeReader ==> No chain!" << endl;
}

int TreeReader::GetEntry(long long entry)
{
// Read contents of entry.
    current_entry = entry;
    return TreeClassBase::GetEntry(entry);
}