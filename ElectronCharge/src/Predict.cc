#include "../include/Predict.h"
#include "DataFormats/interface/DiElectronEvent.h"

numberHolder::numberHolder(string name) :
Name(name),
nEvents(0),
nPos(0),
nPosGen(0),
nNeg(0),
nNegGen(0),
nGoods(0),
nBads(0) {

}

void numberHolder::Add(const ElectronAnalysis::DiLepton * ev) {
    nEvents++;
    //    if (ev->FirstElectron->NewCharge(ElectronAnalysis::Electron::GSF_CTF) > 0) nPos++;
    //    else nNeg++;
    //
    //    if (ev->FirstElectron->HasGenElectronStat3 && ev->FirstElectron->GenElectronStat3.charge() != 0) {
    //        if (ev->FirstElectron->GenElectronStat3.charge() > 0) nPosGen++;
    //        else nNegGen++;
    //
    //        if (ev->FirstElectron->IsBadCharge(use_match1))
    //            nGoods++;
    //        else nBads++;
    //    }
    //
    //    if (ev->SecondElectron->NewCharge(ElectronAnalysis::Electron::GSF_CTF) > 0) nPos++;
    //    else nNeg++;
    //
    //    if (ev->SecondElectron->HasGenElectronStat3 && ev->SecondElectron->GenElectronStat3.charge() != 0) {
    //        if (ev->SecondElectron->GenElectronStat3.charge() > 0) nPosGen++;
    //        else nNegGen++;
    //
    //        if (ev->SecondElectron->IsBadCharge(use_match1))
    //            nGoods++;
    //        else nBads++;
    //    }
}

void numberHolder::Print() {
    cout << "-----------------------------"
            << "\n" << Name << "---------------"
            << "\nNEvents : " << nEvents
            << "\nNPos : " << nPos << " -- NNeg : " << nNeg
            << "\nNPosGen : " << nPosGen << " -- NNegGen : " << nNegGen
            << "\nNGood: " << nGoods << " -- NBad : " << nBads << endl;
}

Predictor1D::Predictor1D(string name, std::vector<double>* args) : Predictor1D::base(name, 0., false),
ErrorCoeff(1),
sumProbs(50),
sumErrs(50),
sumFittedProbs(50),
sumFittedErrs(50),
NBadCharges(0),
NAllElectrons(0) {

};

Predictor1D::~Predictor1D() {
};

double Predictor1D::Predict(const T* obj) {
    for (int i_h = 0; i_h < NHistograms(); i_h++) {
        ObjectProperty<T>* prop(0);
        TH1* hProp = GetHisto(i_h, &prop);

        double value = prop->ReadValue(obj);

        int nBin = hProp->GetXaxis()->FindBin(value);
        double prob = hProp->GetBinContent(nBin);
        double prob_err = hProp->GetBinError(nBin);
        sumErrs[i_h] += fabs(ErrorCoeff * prob_err);
        sumProbs[i_h] += prob;

        //the same values but from interpolation
        double prob_inter = hProp->Interpolate(value);
        double prob_err_inter;

        double x = value;
        Int_t xbin = nBin;
        Double_t x0, x1, y0, y1;

        if (x <= hProp->GetBinCenter(1)) {
            prob_err_inter = hProp->GetBinError(1);
        } else if (x >= hProp->GetBinCenter(hProp->GetNbinsX())) {
            prob_err_inter = hProp->GetBinError(hProp->GetNbinsX());
        } else {
            if (x <= hProp->GetBinCenter(xbin)) {
                y0 = ErrorCoeff * hProp->GetBinError(xbin - 1);
                x0 = hProp->GetBinCenter(xbin - 1);
                y1 = ErrorCoeff * hProp->GetBinError(xbin);
                x1 = hProp->GetBinCenter(xbin);
            } else {
                y0 = ErrorCoeff * hProp->GetBinError(xbin);
                x0 = hProp->GetBinCenter(xbin);
                y1 = ErrorCoeff * hProp->GetBinError(xbin + 1);
                x1 = hProp->GetBinCenter(xbin + 1);
            }
            prob_err_inter = (y0 + (x - x0)*((y1 - y0) / (x1 - x0)));
        }
        sumFittedErrs[i_h] += fabs(prob_err_inter);
        sumFittedProbs[i_h] += prob_inter;

        if (obj->IsBadCharge(true) && fabs(obj->GenID) == 11)
            NBadCharges++;
    }

    NAllElectrons++;
    return 0.0;
};

void Predictor1D::SetProp(ObjectProperty<T>* op, TH1* h) {
    props_histos[op].push_back(h);
};

void Predictor1D::AddProp(ObjectProperty<T>* op, std::vector<TH1*> h) {
    props_histos[op] = h;
}

void Predictor1D::SetErrorCoeff(double coeff) {
    ErrorCoeff = coeff;
};

void Predictor1D::Finish(bool print, TDirectory* old_dir) {
    if (print) {
        cout << "- " << base::Name << " :" << endl;
        cout << "|Name | Value|Error| " << endl;
        for (int i = 0; i < NVariables(); i++)
            cout << "|" << GetVariableName(i) << "|" << 100.0 * Value(i) << "|" << 100.0 * Error(i) << "|" << endl;

        cout << "|NAllElectrons|" << NAllElectrons << "||" << endl;
        //        cout << "|NAllEvents|" << NSS << "||" << endl;
    }
    //        base::use_fit ? hExt->Multiply(f1) : hExt->Multiply(hProp);
    //        if (print) cout << "\tThe results from histogram      :" << hExt->GetSum() << endl;
    //
    //        //TDirectory* old_dir = gDirectory;
    //        TDirectory* dir = old_dir->mkdir(base::Name.c_str());
    //        dir->cd();
    //        ((TH1*) hExt)->Write();
    //        old_dir->cd();
};

void Predictor1D::Finish() {
    Finish(true, gDirectory);
};

void Predictor1D::WriteAll(TDirectory* dir) {
    Finish(false, dir);
};

double Predictor1D::Value(int VariableID) {
    if (VariableID == 0)
        return -100;
    else if (VariableID == 1)
        return NBadCharges;
    else if (VariableID < NVariables()) {
        int histoId = (VariableID - 2) / 4;
        int typeId = (VariableID - 2 - 4 * histoId);

        return ((typeId == 0) ? sumProbs[histoId] : (typeId == 1) ? sumErrs[histoId] : (typeId == 2) ? sumFittedProbs[histoId] : (typeId == 3) ? sumFittedErrs[histoId] : -1);
    }

    return NAN;
};

double Predictor1D::Error(int VariableID) {

    if (VariableID == 0)
        return -100;
    else if (VariableID == 1)
        return sqrt(NBadCharges);
    else if (VariableID < NVariables()) {
        int histoId = (VariableID - 2) / 4;
        int typeId = (VariableID - 2 - 4 * histoId);

        return ((typeId == 0) ? sumErrs[histoId] : (typeId == 1) ? 0.0 : (typeId == 2) ? sumFittedErrs[histoId] : (typeId == 3) ? 0.0 : -1);
    }

    return NAN;
};

string Predictor1D::GetVariableName(int VariableID) {
    if (VariableID == 0)
        return "NSameSign";
    else if (VariableID == 1)
        return "NBadElectrons";
    else if (VariableID < NVariables()) {
        int histoId = (VariableID - 2) / 4;
        int typeId = (VariableID - 2 - 4 * histoId);

        ObjectProperty<T>* objjj(0);
        GetHisto(histoId, &objjj)->GetName();
        string histoName(objjj->PropertyName);
        string typeName = (typeId == 0) ? "PredictedSS" : (typeId == 1) ? "SysErrPredictedSS" : (typeId == 2) ? "InterpolatedPredictedSS" : (typeId == 3) ? "SysErrInterpolatedPredictedSS" : "";

        return (histoName + "_" + typeName);
    }

    return "";
};

int Predictor1D::NVariables() {
    return 2 + NHistograms()*4;
};

int Predictor1D::NHistograms() {
    int ret = 0;
    for (map_prop_histo::iterator itr = props_histos.begin(); itr != props_histos.end(); itr++) {
        ret += itr->second.size();
    }

    return ret;
}

TH1* Predictor1D::GetHisto(int histoId, ObjectProperty<T>** obj) {
    if (histoId < NHistograms()) {
        for (map_prop_histo::iterator itr = props_histos.begin(); itr != props_histos.end(); itr++) {
            if (histoId >= int(itr->second.size()))
                histoId -= itr->second.size();
            else {
                if (obj != NULL)
                    (*obj) = itr->first;
                return itr->second.at(histoId);
            }
        }
        return NULL;
    } else
        return NULL;
}

void Predictor1D::Fill(const ElectronAnalysis::DiLepton* evt) {

    if (evt->FirstElectron->isElectron())
        this->Predict(dynamic_cast<ElectronAnalysis::Electron*> (evt->FirstElectron));

    if (evt->SecondElectron->isElectron())
        this->Predict(dynamic_cast<ElectronAnalysis::Electron*> (evt->SecondElectron));
};

PMMPredict::PMMPredict(const edm::ParameterSet& ps) : PMMPredict::BASE(ps),
reader_all(ps.getParameter<string>("PMMFileName"), (PMMReader::ChargeSource)ps.getParameter<int>("ChargeSource"), ps.getParameter<int>("Cuts"), false),
reader_BarrelEndCap(ps.getParameter<string>("PMMFileName"), (PMMReader::ChargeSource)ps.getParameter<int>("ChargeSource"), ps.getParameter<int>("Cuts"), true),
predictor_EE(BASE::Name + "_EE", NULL),
predictor_EM(BASE::Name + "_EM", NULL),
ChargeSource((PMMReader::ChargeSource)ps.getParameter<int>("ChargeSource")),
CTFGSFMatche(ps.getParameter<int>("Cuts") & int(PMMReader::CTFMatch)),
ApplyD0(ps.getParameter<int>("Cuts") & int(PMMReader::D0)),
NSS("SS"), NOS("OS"), NFailed("Failed") {
    cout << CTFGSFMatche << " -- " << ApplyD0 << endl;

    ElectronAnalysis::ElectronProperties::Eta* etaProp = new ElectronAnalysis::ElectronProperties::Eta(true);
    predictor_EE.SetProp(etaProp, &(reader_BarrelEndCap.hPMM));
    predictor_EM.SetProp(etaProp, &(reader_BarrelEndCap.hPMM));

    ElectronAnalysis::DiElectronEventProperties::AlwaysOne* oneProp = new ElectronAnalysis::DiElectronEventProperties::AlwaysOne();
    predictor_EE.SetProp(oneProp, &(reader_all.hPMM));
    predictor_EM.SetProp(oneProp, &(reader_all.hPMM));
};

void PMMPredict::OnChangeFile(FileInfo* fi, DataSet* ds) {
};

bool PMMPredict::Run(ElectronAnalysis::DiLepton* ev) {
    int charge1 = 0;
    int charge2 = 0;

    int NMuons = 0;

    for (int i = 0; i < 2; i++) {
        Lepton* lep;
        int* charge_val;
        switch (i) {
            case 0:
                lep = ev->FirstElectron;
                charge_val = &(charge1);
                break;
            case 1:
                lep = ev->SecondElectron;
                charge_val = &(charge2);
                break;
        }

        if (lep->isMuon()) {
            *charge_val = lep->Charge;
            NMuons++;
        } else if (lep->isElectron()) {
            ElectronAnalysis::Electron* elec__ = dynamic_cast<ElectronAnalysis::Electron*> (lep);
            switch (ChargeSource) {
                case PMMReader::AllChargesMatch:
                    if (elec__->ElCInfoIsGsfCtfScPixCons != 0)
                        *charge_val = lep->Charge;
                    else
                        *charge_val = 0;
                    break;
                case PMMReader::GSFCharge:
                    *charge_val = int(elec__->NewCharge(ElectronAnalysis::Electron::GSF));
                    if (CTFGSFMatche && (elec__->ElCInfoIsGsfCtfCons == 0 || elec__->ElClosestCtfTrackPt < 0))
                        *charge_val = 0;
                    break;
                case PMMReader::MajorityCharge:
                    *charge_val = lep->Charge;
                    if (CTFGSFMatche && (elec__->ElCInfoIsGsfCtfCons == 0 || elec__->ElClosestCtfTrackPt < 0))
                        *charge_val = 0;
                    break;
            }

            if (ApplyD0) {
                if (elec__->D0BS > 0.02)
                    *charge_val = 0;
            }
        }
    }

    int charge_state = -100;
    if (charge1 == 0 || charge2 == 0) {
        charge_state = 1;
        NFailed.Add(NMuons);
    } else if (charge1 == charge2) {
        charge_state = 100;
        NSS.Add(NMuons,ev->RunID,ev->EventNumber);
    } else if (charge2 != charge1) {
        charge_state = 10000;
        if (NOS.Add(NMuons,ev->RunID,ev->EventNumber)) {
            if (NMuons == 0)
                this->predictor_EE.Fill(ev);
            else if (NMuons == 1)
                this->predictor_EM.Fill(ev);
        }
    }
};

std::ostream & operator<<(std::ostream& output, const std::multimap<int, int>& p) {
    output << "(";

    for(std::multimap<int, int>::const_iterator itr = p.begin() ; itr != p.end() ; itr++)
        output << itr->first << ":" << itr->second << "-" ;

    output << ")";
    return output; // for multiple << operators.
}

void PMMPredict::End() {
    cout << "|EE : " << Name << "|" << NOS.EE << "|" << NSS.EE << "|" << predictor_EE.sumProbs[0] << "±" << predictor_EE.sumErrs[0] << "|" << predictor_EE.sumProbs[1] << "±" << predictor_EE.sumErrs[1] << "|" << NSS.EventNumbersEE << "|" << endl;
    cout << "|EM : " << Name << "|" << NOS.EM << "|" << NSS.EM << "|" << predictor_EM.sumProbs[0] << "±" << predictor_EM.sumErrs[0] << "|" << predictor_EM.sumProbs[1] << "±" << predictor_EM.sumErrs[1] << "|" << NSS.EventNumbersEM << "|" << endl;
    //    this->predictor.Finish(false, NULL);
    //    NSS.PrintHeader();
    //    NSS.Print();
    //    NOS.Print();
    //    NFailed.Print();
};
