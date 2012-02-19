/* 
 * File:   HistoManager.h
 * Author: hbakhshi
 *
 * Created on November 3, 2008, 2:32 PM
 */

#ifndef _HISTOMANAGER_H
#define	_HISTOMANAGER_H

#include "TObject.h"
#include "TObjArray.h"
#include "stdio.h"
#include "TCanvas.h"
#include <map>
#include "TPaveStats.h"
#include "TSystem.h"
#include "TLegend.h"
#include "THStack.h"
#include "TRandom.h"
#include "TH2.h"
#include "TLegendEntry.h"
#include "TDirectory.h"


#include "ObjectProperty.h"
#include "StyleManager.h"
using namespace std;

template<class T>
class Histograms {
public:

  Histograms(string name, string title = "") : _CollectionName(name),
					       _Title(title) {
    th1s = new std::vector<TH1Ext<T> >();
    th2s = new std::vector<TH2Ext<T> >();
  };

  virtual ~Histograms() {
  };

  virtual void Fill(const T* theObj, double w = 1) {
    for (uint i = 0; i < GetSizeH1(); i++)
      GetHistogram1(i)->Fill(theObj, w);

    for (uint j = 0; j < GetSizeH2(); j++)
      GetHistogram2(j)->Fill(theObj, w);
  };

  template<class Property>
  Property* AddHisto1();

  ObjectProperty<T>* AddHisto1(ObjectProperty<T>*);

  template<class Property1st, class Property2nd>
  void AddHisto2();

  virtual std::string CollectionName() {
    return _CollectionName;
  };

  virtual void Write(TDirectory* currentDir = 0) {

      cout << "\t" << CollectionName() << endl;
    if (CollectionName() != "" && currentDir != 0) {
      currentDir->mkdir(CollectionName().c_str())->cd();
    }

    for (uint i = 0; i < GetSizeH1(); i++) {
      TH1* th = GetHistogram1(i);
      th->Write();
    }

    for (uint j = 0; j < GetSizeH2(); j++) {
      TH2* th2 = GetHistogram2(j);
      th2->Write();
    }

    DuringWrite();

    if (CollectionName() != "" && currentDir != 0) {
      currentDir->cd();
    }
  }

  virtual void Write(TDirectory* currentDir , int n_probs, int prop_ids_toBePlotted[] , std::string x_name = "") {

    if (CollectionName() != "" && currentDir != 0) {
      TDirectory* dir = currentDir->GetDirectory(CollectionName().c_str());
      if (dir == 0 && dir == NULL)
	currentDir->mkdir(CollectionName().c_str())->cd();
      else
	dir->cd();
    }

    std::map<double, TH1*, greater_equal<double > > histos;
    for (int h = 0; h < n_probs; h++) {
      int id = prop_ids_toBePlotted[h];
      int histo_id = -2;
      for (uint k = 0; k < GetSizeH1(); k++)
	if (GetHistogram1(k)->GetPropertyID() == id) {
	  histo_id = k;
	  break;
	}

      if (histo_id > -1) {
	TH1* f = GetHistogram1(histo_id);
	double max = f->GetBinContent(f->GetMaximumBin());
	histos.insert(std::pair<double, TH1* >(max, f));
      }
    }

    TCanvas* c = new TCanvas();
    std::map<double, TH1*, greater_equal<double> >::const_iterator itr = histos.begin();
    for (uint k = 0; k < histos.size(); k++) {
      TH1* h = (*itr).second;

      h->SetTitle(h->GetXaxis()->GetTitle());
      StyleManager::SetStyle(h , k , 2);

      if (k == 0) {
	h->SetXTitle(x_name.c_str());
	h->Draw();
      } else {
	h->Draw("sames");
      }
      itr++;
    }
    c->SetLogy();
    c->BuildLegend();
    c->Write("Canvas__");
    //c->Close();


    itr = histos.begin();
    for (uint k = 0; k < histos.size(); k++) {
      TH1* h = (*itr).second;
      h->SetXTitle(h->GetTitle());
      h->SetTitle(_Title.c_str());
    }

    // if (CollectionName() != "" && currentDir != 0) {
    //   currentDir->cd();
    // }
  }

  TH1Ext<T>* GetHistogram1(uint i) {
    return &(th1s->at(i));
  };

  TH2Ext<T>* GetHistogram2(uint i) {
    return &(th2s->at(i));
  };

  uint GetSizeH1() {
    return th1s->size();
  };

  uint GetSizeH2() {
    return th2s->size();
  };
protected:
  std::vector<TH1Ext<T> >* th1s;
  std::vector<TH2Ext<T> >* th2s;
  string _CollectionName;
  string _Title;

  virtual void DuringWrite() {
  };
};

template<class _T >
template<class _Property>
_Property* Histograms<_T>::AddHisto1() {
  ObjectProperty<_T>* _prop = new _Property();
  TH1Ext<_T>* h = _prop->GetH1(CollectionName(), _Title);
  th1s->push_back(*h);
  return (_Property*) _prop;
}

template<class _T >
ObjectProperty<_T>* Histograms<_T>::AddHisto1(ObjectProperty<_T>* op) {

  TH1Ext<_T>* h = op->GetH1(CollectionName(), _Title);
  th1s->push_back(*h);
  return op;
}

template<class _T>
template<class _Property1st, class _Property2nd>
void Histograms<_T>::AddHisto2() {
  ObjectProperty<_T> * prop1 = new _Property1st();
  ObjectProperty<_T> * prop2 = new _Property2nd();
  TH2Ext<_T> * h = prop1->GetH2(prop2, CollectionName(), _Title);
  th2s->push_back(*h);
}



#endif	/* _HISTOMANAGER_H */

