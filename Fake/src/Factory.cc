#include "../include/Factory.h"

vector<BaseAlgorithm*> Factory::allFR;
Factory::FakeRate_Factory_Exception Factory::myex;
TDirectory* Factory::theFile(NULL);

BaseAlgorithm* Factory::FakeRate_Factory(TDirectory* dir, TreeReader* tree) {
    std::string dirName = dir->GetName();

    if (dirName.find("JET") != string::npos) {
        return new EleJetFR(dir, tree);
    } else if (dirName.find("ELE") != string::npos) {
        return new EleIsoTemplate(dir, tree);
    } else
        throw myex;
}

vector<BaseAlgorithm*>* Factory::GetAllFakeRates(TDirectory* mainDir) {
    Factory::theFile = mainDir;
    Factory::allFR.clear();

    TIter next(mainDir->GetListOfKeys());
    while (TObject * obj = next()) {
        if (mainDir->GetKey(obj->GetName())->IsFolder() )
            try {
                Factory::allFR.push_back(FakeRate_Factory(mainDir->GetDirectory(obj->GetName()), NULL));
            } catch (FakeRate_Factory_Exception ex) {
                cout << obj->GetName() << " : " << ex.what() << endl;
            }
    }
    return &(Factory::allFR);
}
