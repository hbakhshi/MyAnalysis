/* 
 * File:   PmmCalculator.cc
 * Author: hbakhshi
 * 
 * Created on November 25, 2010, 1:01 PM
 */

#include "ElectronCharge/include/PmmCalculator.h"

void PMMCalculator::OnChangeFile(FileInfo* fi, DataSet* ds) {
    Histograms< ElectronAnalysis::DiElectronEvent >* tmpVar = new ElectronAnalysis::DiElectronEventProperties::AllHistograms(fi->Name);
    AllHistosForEvents.push_back(tmpVar);
    CurrentEventHistos = tmpVar;
}

bool PMMCalculator::Run(ElectronAnalysis::DiElectronEvent* ev) {

    current_int_lum += xsec_inverse;
    if (ev == NULL) {
        return false;
    }

    NNN++;


    //CurrentEventHistos->Fill(ev, ev->Weight);

    hIsoEle1vsIsoEle2->Fill(ev->FirstElectron->ElRelIso04, ev->SecondElectron->ElRelIso04);
    hD0Ele1vsD0Ele2->Fill(ev->FirstElectron->D0PV / ev->FirstElectron->D0E, ev->SecondElectron->D0PV / ev->SecondElectron->D0E);

    if (true) {
        CERL_EleCharge.Fill(ev);
        CERL_CTFCharge.Fill(ev);
        CERL_GsfCharge.Fill(ev);
        CERL_AllCharge.Fill(ev);

        if (this->UseGen) {
            if (abs(ev->FirstElectron->GenID) == 11)
                chargeErrorGen1.Fill(ev->FirstElectron_.get(), /*.2,*/ 1);
            if (abs(ev->SecondElectron->GenID) == 11)
                chargeErrorGen1.Fill(ev->SecondElectron_.get(), /* .2,*/ 1);
        }

        return true;
    } else {
        return false;
    }
}

void PMMCalculator::End() {

    TFile* file = info::TheInfo->OutFile;
    file->cd();
    hIsoEle1vsIsoEle2->Write();
    hD0Ele1vsD0Ele2->Write();

    cout << "- " << DirName << endl;
    CERL_EleCharge.WriteAll(file);
    CERL_GsfCharge.WriteAll(file);
    CERL_CTFCharge.WriteAll(file);
    CERL_AllCharge.WriteAll(file);

    if (this->UseGen)
        chargeErrorGen1.WriteAll(file);

    file->Close();

    string pwd(gSystem->pwd());
    gSystem->cd((pwd + "/" + this->DirName).c_str());
    AllHistosForEvents.DrawAll();
    AllHistosForEvents.DrawSTACKAll();

    gSystem->cd((pwd).c_str());
    cout << " Total Number of events passed to pmmcalc : " << NNN << endl;

    //    gSystem->cd(pwd.c_str());
}
