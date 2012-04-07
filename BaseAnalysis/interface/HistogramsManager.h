/* 
 * File:   HistogramsManager.h
 * Author: hbakhshi
 *
 * Created on December 2, 2008, 6:20 PM
 */

#ifndef _HISTOGRAMSMANAGER_H
#define	_HISTOGRAMSMANAGER_H

#include "Histograms.h"
#include <vector>
#include <TSystem.h>
#include <TCanvas.h>
#include <map>
#include "StyleManager.h"
#include <string>

using namespace std;

template<class T>
class HistogramsManager : public vector<Histograms<T>*> {
public:
    typedef vector<Histograms<T>*> BASE;

    HistogramsManager(string Name, bool LOGY = true, bool BuildLegend = true, bool normalized = false, bool enabled = true) {
        name = Name;
        logY = LOGY;
        buildLegend = BuildLegend;
        Normalized = normalized;
        Enabled = enabled;
    };

    void WriteAll(TDirectory* dir) {
        if (!Enabled)
            return;

        cout << name << endl;
        TDirectory* dir__ = dir->mkdir(name.c_str());

        for (uint i = 0; i < this->size(); i++) {
            Histograms<T>* elem = this->at(i);
            elem->Write(dir__);
        }
    };

    void DrawAll() {
        uint k = 0;
        Histograms<T>* elem = this->at(k);

        for (uint i = 0; i < elem->GetSizeH1(); i++) {
            DrawTH1(elem->GetHistogram1(i)->GetPropertyID(), elem->GetHistogram1(i)->GetPropertyName());
        }

        for (uint i = 0; i < elem->GetSizeH2(); i++) {
            DrawTH2(elem->GetHistogram2(i)->GetProp1ID(), elem->GetHistogram2(i)->GetProp2ID(), elem->GetHistogram2(i)->GetMixedName());
        }

    };

    void DrawSAMEAll() {
        uint k = 0;
        Histograms<T>* elem = this->at(k);

        for (uint i = 0; i < elem->GetSizeH1(); i++) {
            DrawSame(elem->GetHistogram1(i)->GetPropertyID(), elem->GetHistogram1(i)->GetPropertyName());
        }
    };

    void DrawSTACKAll() {
        uint k = 0;
        Histograms<T>* elem = this->at(k);

        for (uint i = 0; i < elem->GetSizeH1(); i++) {
            DrawStack(elem->GetHistogram1(i)->GetPropertyID(), elem->GetHistogram1(i)->GetPropertyName());
        }
    };

    Histograms<T>* CreateHistos(string name) {
        Histograms<T>* ret = new Histograms<T > (name);

        this->push_back(ret);

        //        cout << this->size() << endl;

        return ret;
    };

    ObjectProperty<T>* AddHisto1ToAll(ObjectProperty<T>* o) {
        for (int i = 0; i < this->size(); i++)
            this->at(i)->AddHisto1(o);
        
        return o;
    };
    
    void AddHisto2ToAll(ObjectProperty<T>* first,ObjectProperty<T>* second);

    void FillAll(const T* theObj, int JustThis = -1, double w = 1) {
        if (!Enabled)
            return;

        if (JustThis < 0)
            for (int ii = 0; ii < this->size(); ii++)
                this->at(ii)->Fill(theObj, w);
        else
            this->at(JustThis)->Fill(theObj, w);
    };
private:

    void DrawSame(int propID, string propName) {
        std::vector<TH1Ext<T>*>* all = FindAll(propID);
        TCanvas* c = this->MakeACanvas();

        std::map<double, TH1*, greater_equal<double> > maxValueHisto;
        uint k;
        for (k = 0; k < all->size(); k++) {
            TH1* f = all->at(k);
            double max = f->GetBinContent(f->GetMaximumBin());
            maxValueHisto.insert(std::pair<double, TH1* >(max, f));
        }

        std::map<double, TH1*, greater_equal<double> >::const_iterator itr = maxValueHisto.begin();
        for (k = 0; k < maxValueHisto.size(); k++) {
            TH1* h = (*itr).second;

            StyleManager::SetStyle(h, k);
            if (k == 0) {
                if (Normalized)
                    h->DrawNormalized();
                else
                    h->Draw();
            } else {
                if (Normalized)
                    h->DrawNormalized("sames");
                else
                    h->Draw("sames");
            }
            itr++;
        }
        this->SaveTheCanvas(c, propName, "SAME");
    };

    void DrawTH1(int propID, string propName) {
        TCanvas* c = MakeACanvas();

        vector<TH1Ext<T>*>* all = FindAll(propID);
        switch (all->size()) {
            case 1:
                c->Divide(1, 1);
                break;
            case 2:
                c->Divide(1, 2);
                break;
            case 3:
            case 4:
                c->Divide(2, 2);
                break;
            case 5:
            case 6:
                c->Divide(2, 3);
                break;
            case 7:
            case 9:
                c->Divide(3, 3);
                break;
            case 8:
                c->Divide(2, 4);
                break;
            default:
                cout << "Number of HistoManagers are more than 9 and Draw method cannot work.." << endl;
                return;
        }


        for (uint k = 0; k < all->size(); k++) {
            TH1Ext<T>* elem = all->at(k);

            c->cd(k + 1);

            if (Normalized)
                StyleManager::SetStyle(elem, k)->DrawNormalized();
            else
                StyleManager::SetStyle(elem, k)->Draw();
        }
        SaveTheCanvas(c, propName);
    };

    void DrawTH2(int prop1ID, int prop2ID, string Name) {
        TCanvas* c = MakeACanvas();

        vector<TH2Ext<T>*>* all = FindAll(prop1ID, prop2ID);
        switch (all->size()) {
            case 1:
                c->Divide(1, 1);
                break;
            case 2:
                c->Divide(1, 2);
                break;
            case 3:
            case 4:
                c->Divide(2, 2);
                break;
            case 5:
            case 6:
                c->Divide(2, 3);
                break;
            case 7:
            case 9:
                c->Divide(3, 3);
                break;
            case 8:
                c->Divide(2, 4);
                break;
            default:
                cout << "Number of HistoManagers are more than 9 and Draw method cannot work.." << endl;
                return;
        }


        for (uint k = 0; k < all->size(); k++) {
            TH2Ext<T>* elem = all->at(k);

            c->cd(k + 1);

            StyleManager::SetStyle(elem, k)->Draw();
        }
        SaveTheCanvas(c, Name);
    };

    void DrawStack(int propID, string propName) {
        TCanvas* c = MakeACanvas();
        THStack* ths = new THStack(this->name.c_str(), "");

        vector<TH1Ext<T>*>* all = FindAll(propID);
        for (uint k = 0; k < all->size(); k++) {
            TH1Ext<T>* elem = all->at(k);

            StyleManager::SetStyle(elem, k);

            ths->Add(elem);
        }

        c->cd();
        ths->Draw();
        SaveTheCanvas(c, propName, "STACK");
    };

    void mkdir() {
        gSystem->mkdir(name.c_str(), true);
    };

    std::vector<TH1Ext<T>*>* FindAll(int PropertyID) {
        std::vector<TH1Ext<T>*>* ret = new std::vector<TH1Ext<T>*>();
        for (uint HistosID = 0; HistosID < this->size(); HistosID++) {
            TH1Ext<T>* h = Find(HistosID, PropertyID);
            if (h != 0)
                ret->push_back(h);
        }
        return ret;
    };

    std::vector<TH2Ext<T>*>* FindAll(int Prop1ID, int Prop2ID) {
        std::vector<TH2Ext<T>*>* ret = new std::vector<TH2Ext<T>*>();
        for (uint HistosID = 0; HistosID < this->size(); HistosID++) {
            TH2Ext<T>* h = Find(HistosID, Prop1ID, Prop2ID);
            if (h != 0)
                ret->push_back(h);
        }
        return ret;
    };

    TH1Ext<T>* Find(int HistosID, int PropertyID) {
        Histograms<T>* hs = this->at(HistosID);
        for (uint i = 0; i < hs->GetSizeH1(); i++) {
            TH1Ext<T>* h = hs->GetHistogram1(i);
            if (h->IsIt(PropertyID))
                return h;
        }
        return 0;
    };

    TH2Ext<T>* Find(int HistosID, int Prop1ID, int Prop2ID) {
        Histograms<T>* hs = this->at(HistosID);
        for (uint i = 0; i < hs->GetSizeH2(); i++) {
            TH2Ext<T>* h = hs->GetHistogram2(i);
            if (h->IsIt(Prop1ID, Prop2ID) != 0)
                return h;
        }
        return 0;
    };

    TCanvas * MakeACanvas() {
        TCanvas* c = new TCanvas();

        return c;
    };

    enum SaveExtensions {
        C,
        JPG,
        GIF,
        PDF,
        PS
    };

    void SaveTheCanvas(TCanvas* c, string propName, string postFix = "", SaveExtensions extension = C) {
        if (this->logY)
            c->SetLogy(1);

        if (this->buildLegend) {
            c->cd(0);
            TLegend* leg = c->BuildLegend();
            if (leg != NULL) {
                leg->SetX1NDC(0.35);
                leg->SetX2NDC(0.65);
                leg->SetY1NDC(0.9);
                leg->SetY2NDC(0.995);

                TList *lop = leg->GetListOfPrimitives();
                TIter next(lop);
                TObject *o = 0;
                while ((o = next()))
                    if (o->InheritsFrom(TLegendEntry::Class()))
                        ((TLegendEntry*) o)->SetOption("f");
            }
        }
        mkdir();

        string fileName(name);
        fileName.append("/");
        fileName.append(propName);

        if (postFix != "") {
            fileName.append("_");
            fileName.append(postFix);
        }

        switch (extension) {
            case C:
                fileName.append(".C");
                break;
            case JPG:
                fileName.append(".jpg");
                break;
            case GIF:
                fileName.append(".gif");
                break;
            case PDF:
                fileName.append(".pdf");
                break;
            case PS:
                fileName.append(".ps");
                break;
        }

        c->SaveAs(fileName.c_str());
    };
    string name;
    bool logY;
    bool buildLegend;
    bool Normalized;
public:
    bool Enabled;
};

template<class _T>
void HistogramsManager<_T>::AddHisto2ToAll(ObjectProperty<_T>* first,ObjectProperty<_T>* second) {
    for (int i = 0; i < this->size(); i++)
        this->at(i)->AddHisto2(first,second);
}

#endif	/* _HISTOGRAMSMANAGER_H */

