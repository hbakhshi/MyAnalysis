
#ifdef AAAA
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <iostream>

using namespace std;

class TestThread {
public:
    int II;

    TestThread() : II(0) {
    };
};

struct thread_alarm {

    thread_alarm(int secs) : m_secs(secs), nloops(1) {
    }

    void operator()() {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC);

        for (int i = 0; i < nloops; i++) {
            xt.sec += m_secs;
            boost::thread::sleep(xt);
            ttt->II++;
            std::cout << "alarm sounded..." << i << std::endl;
        }
    }

    int m_secs;
    int nloops;
    TestThread* ttt;
};

int main(int argc, char* argv[]) {
    int secs = 2;
    TestThread ttt;
    std::cout << "setting alarm for 5 seconds..." << std::endl;
    thread_alarm alarm(secs);
    alarm.ttt = &ttt;
    alarm.nloops = 5;
    boost::thread thrd(alarm);

    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += 6;

    boost::thread::sleep(xt);
    ttt.II++;

    std::cout << "main thread ..." << ttt.II << std::endl;

    thrd.join();

    std::cout << ttt.II << endl;
}

#endif

#ifdef ThreadTTT

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "boost/thread/pthread/recursive_mutex.hpp"

class ThreadShared : public boost::recursive_mutex {
public:

    ThreadShared() : boost::recursive_mutex(), II(0) {

    };

    virtual ~ThreadShared() {

    }
    int II;
};

ThreadShared sharedObj;

using namespace std;

struct count__ {
    ThreadShared* obj;

    count__(int MAX, ThreadShared * SharedObj) : max(MAX) {
        lock = new boost::recursive_mutex::scoped_lock(*SharedObj);
        obj = SharedObj;
    }
    //    count__(const count__& ccc) : obj(NULL){
    //        if(ccc.obj != NULL)
    //            this->lock = & (*ccc.lock->move());
    //
    //        this->max = ccc.max;
    //    }

    boost::recursive_mutex::scoped_lock* lock;

    void operator()() {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC);

        for (int i = 0; i < max; ++i) {
            cout << "LOCK : " << lock->owns_lock() << " - " << sharedObj.try_lock() << " - ";
            if (!lock->owns_lock())
                lock->lock();
            cout << lock->owns_lock() << " - " << sharedObj.try_lock() << endl;

            xt.nsec += 10000000 * i;
            boost::thread::sleep(xt);

            sharedObj.II++;
            //std::cout << sharedObj.II << std::endl;

            if (lock->owns_lock())
                lock->unlock();
        }
        //lock->release();
    }
    int max;
};

struct print {

    print(std::string name) : Name(name), lastII(-1) {
    }

    void operator()() {

        while (true) {
            if (lastII != sharedObj.II) {
                lastII = sharedObj.II;
                //std::cout << "AAS : " << " - " << sharedObj.II << std::endl;

                if (sharedObj.try_lock()) {
                    boost::recursive_mutex::scoped_lock lock(sharedObj);

                    if (lastII != sharedObj.II) {
                        lastII = sharedObj.II;
                        std::cout << Name << " - " << sharedObj.II << std::endl;
                    }

                    lock.unlock();
                }
            }
        }
    }

    int lastII;
    std::string Name;
};

int main(int argc, char* argv[]) {
    boost::thread thrd1(count__(20, &sharedObj));
    //boost::thread thrd2(print("test"));
    thrd1.join();
    std::cout << "1ended" << std::endl;
    //thrd2.detach();
    return 0;
}

#endif


#ifdef multimapTest
#include <iostream>
#include <deque>
#include <algorithm>
#include <TFile.h>

#include "BaseAnalysis/interface/EfficiencyHandlerReader.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace std;

struct Object {
    double pt;
    double eta;
    double phi;

    int index;

    Object() {
        pt = 0;
        eta = 0;
        phi = 0;

        index = -1;
    }

    Object(double PT, double Eta, double Phi, int idx) {
        pt = PT;
        eta = Eta;
        phi = Phi;

        index = idx;
    }
};

class FakeInfo {
public:
    Object FakableObject;
    Object Electron;
    bool hasEle;

    Object Probabilities;
    double coeff1;
    double coeff2;
    double coeff3;

    static bool HasEle(const FakeInfo* ff) {
        return ff->hasEle;
    };

    class All_Property : public ObjectProperty<FakeInfo> {
    public:
        typedef FakeInfo T;
        typedef ObjectProperty<T> BASE;

        All_Property(string objName) : BASE("All", "Total", 0, 1.0, 1, objName, 4) {
        };

        virtual ~All_Property() {

        }

        virtual double ReadValue(const T*) const {
            return 0.5;
        }
    };

    class Phi_Property : public ObjectProperty<FakeInfo> {
    public:
        typedef FakeInfo T;
        typedef ObjectProperty<T> BASE;

        Phi_Property(string objName) : BASE("Phi", "#phi", -CLHEP::pi, CLHEP::pi, 21, objName, 3) {
        };

        virtual ~Phi_Property() {

        }

        virtual double ReadValue(const T* obj) const {
            return obj->FakableObject.phi;
        }
    };

    class PT_Property : public ObjectProperty<FakeInfo> {
    public:
        typedef FakeInfo T;
        typedef ObjectProperty<T> BASE;
        vector<double> bins;

        PT_Property(string objName, vector<double>* _bins = NULL) : BASE("Pt", "P_t", 0, 500, 50, objName, 1) {
            if (_bins != NULL) {
                bins.assign(_bins->begin(), _bins->end());
                this->PropertyName = "PtBinned";
                this->PropertyID = 101;
                this->NumberOfBins = bins.size();
            }
        };

        virtual ~PT_Property() {

        }

        virtual double ReadValue(const T* obj) const {
            return obj->FakableObject.pt;
        };

        virtual TH1Ext<T>* GetH1(string name, string title = "") {
            TH1Ext<T>* ret = BASE::GetH1(name, title);
            if (bins.size() > 0) {
                double bins_[100];
                for (int ii = 0; ii < bins.size(); ii++)
                    bins_[ii] = bins[ii];

                ret->SetBins(bins.size() - 1, bins_);
            }
            return ret;
        }

        virtual TH2Ext<FakeInfo>* GetH2(ObjectProperty<FakeInfo>* otherProperty, string name, string title = "") {
            TH2Ext<T>* ret = BASE::GetH2(otherProperty, name, title);
            if (bins.size() > 0) {
                double bins_Y[100];
                TH1* hOther = otherProperty->GetH1(name, title);
                hOther->GetXaxis()->GetLowEdge(bins_Y);
                bins_Y[hOther->GetNbinsX()] = hOther->GetXaxis()->GetXmax();
                int nbins_yyy = hOther->GetNbinsX();
                delete hOther;

                double bins_[100];
                for (int ii = 0; ii < bins.size(); ii++)
                    bins_[ii] = bins[ii];

                ret->SetBins(bins.size() - 1, bins_, nbins_yyy, bins_Y);
            }
            return ret;
        }
    };

    class Eta_Property : public ObjectProperty<FakeInfo> {
    public:
        typedef FakeInfo T;
        typedef ObjectProperty<T> BASE;
        vector<double> bins;
        bool ABSoluteValue;

        Eta_Property(string objName, bool endcap_barrel = false, bool abs = false) : BASE("Eta", "#eta", -2.5, 2.5, 20, objName, 2), ABSoluteValue(abs) {
            if (endcap_barrel) {
                double bins_[] = {-2.5, -1.566, -1.4442, 0.0, 1.4442, 1.566, 2.5};
                if (ABSoluteValue) {
                    bins.assign(bins_ + 3, bins_ + 7);
                    this->PropertyName = "ABS_EndcapBarrel";
                    this->PropertyTitle = "|#eta|";
                    this->PropertyID = 102;
                    this->NumberOfBins = bins.size();
                } else {
                    bins.assign(bins_, bins_ + 7);
                    this->PropertyName = "EndcapBarrel";
                    this->PropertyID = 103;
                    this->NumberOfBins = bins.size();
                }
            } else if (ABSoluteValue) {
                this->PropertyName = "ABS_Eta";
                this->PropertyTitle = "|#eta|";
                this->PropertyID = 104;
                this->NumberOfBins = 10;
                this->MinValue = 0.0;
            }
        };

        virtual ~Eta_Property() {

        }

        virtual double ReadValue(const T* obj) const {
            if (ABSoluteValue)
                return fabs(obj->FakableObject.eta);
            else
                return obj->FakableObject.eta;
        };

        virtual TH1Ext<T>* GetH1(string name, string title = "") {
            TH1Ext<T>* ret = BASE::GetH1(name, title);
            if (bins.size() > 0) {
                double bins_[100];
                for (int ii = 0; ii < bins.size(); ii++)
                    bins_[ii] = bins[ii];

                ret->SetBins(bins.size() - 1, bins_);
            }
            return ret;
        }

        virtual TH2Ext<FakeInfo>* GetH2(ObjectProperty<FakeInfo>* otherProperty, string name, string title = "") {
            TH2Ext<T>* ret = BASE::GetH2(otherProperty, name, title);
            if (bins.size() > 0) {
                double bins_Y[100];
                TH1* hOther = otherProperty->GetH1(name, title);
                hOther->GetXaxis()->GetLowEdge(bins_Y);
                bins_Y[hOther->GetNbinsX()] = hOther->GetXaxis()->GetXmax();
                int nbins_yyy = hOther->GetNbinsX();
                delete hOther;

                double bins_[100];
                for (int ii = 0; ii < bins.size(); ii++)
                    bins_[ii] = bins[ii];

                ret->SetBins(bins.size() - 1, bins_, nbins_yyy, bins_Y);
            }
            return ret;
        }
    };
};

int main() {
    TFile* f = TFile::Open("FR_SingleElectronSelectionALL.root");

    EfficiencyHandlerReader<FakeInfo>* effReader = new EfficiencyHandlerReader<FakeInfo > (f, "FakeRates_Full80_Loose95");

    FakeInfo::PT_Property pt("El");
    FakeInfo::Eta_Property eta("El", true, true);

    Efficiency1DObjects eff_pt(effReader->Get(&pt));
    eff_pt.Print();

    //    Efficiency1DObjects eff_eta(effReader->Get(&eta));
    //    eff_eta.Print();

    Efficiency2DObjects eff_eta_pt(effReader->Get(&eta, &pt));
    eff_eta_pt.Print();

}

#endif

#define BTAGWEIGHT3
#ifdef BTAGWEIGHT3
#include "TF1.h"
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

class BTagWeight {
public:

    struct JetInfo {

        JetInfo(float mceff, float datasf, float t = 0.) : eff(mceff), sf(datasf), tag(t) {
        }
        double eff;
        double sf;
        int tag;
    };

    BTagWeight(int nTaggers) : taggers(nTaggers) {
    }

    virtual bool filter(vector<int> tags);
    double weight(vector<vector<JetInfo> > jets);
private:
    int taggers;

};

double BTagWeight::weight(vector<vector<JetInfo> >jets) {
    int njets = jets.size();
    std::vector<int> comb(jets.size());
    for (int i = 0; i < jets.size(); i++)
        comb[i] = 0;
    int idx = 0;
    int max = taggers + 1; //force sorted tagging //1 << taggers;
    double pMC = 0;
    double pData = 0;
    if (jets.size() == 0) return 0.;
    while (comb[jets.size() - 1] < max) {
        std::vector<int> tags;
        for (int j = 0; j < taggers; j++)
            tags.push_back(0);

        double mc = 1.;
        double data = 1.;
        for (size_t j = 0; j < njets; j++) // loop on jets
        {
            // std::cout << std::endl << "Jet" << j ;

            // if none tagged, take the 1-eff SF for the loosest:
            double tagMc = 1. - jets[j][0].eff;
            double tagData = 1. - jets[j][0].eff * jets[j][0].sf;
            if (comb[j] > 0) //if at least one tagged take the SF for the tightest tagger
            {
                int k = comb[j] - 1;
                tagMc = jets[j][k].eff;
                tagData = jets[j][k].eff * jets[j][k].sf;

                if (comb[j] < taggers) //if at least one tagged take the SF for the tightest tagger
                {
                    int k1 = comb[j];
                    tagMc *= 1 - jets[j][k1].eff / jets[j][k].eff;
                    tagData *= 1 - jets[j][k1].eff / jets[j][k].eff * jets[j][k1].sf / jets[j][k].sf;

                }
            }


            for (size_t k = 0; k < taggers; k++) // loop on taggers
            {
                bool tagged = (comb[j] > k); ///((comb[j] >> k) & 0x1) == 1;
                if (tagged) tags[k]++;
            }
            mc *= tagMc;
            data *= tagData;
        }

        if (filter(tags)) {
            //  std::cout << mc << " " << data << " " << data/mc << " " << pData/pMC << endl;
            pMC += mc;
            pData += data;
            //n    std::cout << std::endl<< "mc, data,ratioThis,ratioTot " <<  mc << " " << data << " " << data/mc << " " << pData/pMC << endl;
        }
        while (comb[idx] == max - 1 && idx + 1 < jets.size()) idx++; // find first jets for which we did not already test all configs 
        // next combination
        comb[idx]++; // test a new config for that jet
        for (int i = 0; i < idx; i++) {
            comb[i] = 0;
        } // reset the tested configs for all previous jets
        idx = 0;
    }
    if (pMC == 0) return 0;
    return pData / pMC;
}

bool BTagWeight::filter(std::vector<int> t) {
    return t[0] >= 1; //&& t[1] >= 1;
    // return (t >= minTags && t <= maxTags);
}

struct simpleJet {
public:

    simpleJet(float PT, float ETA, float disc) : pt(PT), eta(ETA), discriminator_value(disc) {
        TF1 BTagScaleFactor("fSFB", "0.603913*((1.+(0.286361*x))/(1.+(0.170474*x)))", 30, 1000);

        TF1 EffB("EffB", "3.90732786802e-06*x*x*x*x +  -0.000239934437355*x*x*x +  0.00664986827287*x*x +  -0.112578996016*x +  1.00775721404", -100, 100);
        TF1 EffC("EffC", "0.343760640168*exp(-0.00315525164823*x*x*x + 0.0805427315196*x*x + -0.867625139194*x + 1.44815935164 )", -100, 100);

        TF1 MisTag_Eta0_5("MistagEta0_5", "(((-0.0235318+(0.00268868*x))+(-6.47688e-06*(x*x)))+(7.92087e-09*(x*(x*x))))+(-4.06519e-12*(x*(x*(x*x))))", 20., 670.);
        TF1 MisTag_Eta5_1("MistagEta5_1", "(((-0.0257274+(0.00289337*x))+(-7.48879e-06*(x*x)))+(9.84928e-09*(x*(x*x))))+(-5.40844e-12*(x*(x*(x*x))))", 20., 670.);
        TF1 MisTag_Eta1_15("MistagEta1_15", "(((-0.0310046+(0.00307803*x))+(-7.94145e-06*(x*x)))+(1.06889e-08*(x*(x*x))))+(-6.08971e-12*(x*(x*(x*x))))", 20., 670.);
        TF1 MisTag_Eta15_24("MistagEta15_24", "(((-0.0274561+(0.00301096*x))+(-8.89588e-06*(x*x)))+(1.40142e-08*(x*(x*x))))+(-8.95723e-12*(x*(x*(x*x))))", 20., 670.);

        TF1 SFlight_Eta0_5("SFlightEta0_5", "(1.13615*((1+(-0.00119852*x))+(1.17888e-05*(x*x))))+(-9.8581e-08*(x*(x*(x/(1+(0.00689317*x))))))", 20., 670.);
        TF1 SFlight_Eta5_1("SFlightEta5_1", "(1.13277*((1+(-0.00084146*x))+(3.80313e-06*(x*x))))+(-8.75061e-09*(x*(x*(x/(1+(0.00118695*x))))))", 20., 670.);
        TF1 SFlight_Eta1_15("SFlightEta1_15", "(1.17163*((1+(-0.000828475*x))+(3.0769e-06*(x*x))))+(-4.692e-09*(x*(x*(x/(1+(0.000337759*x))))))", 20., 670.);
        TF1 SFlight_Eta15_24("SFlightEta15_24", "(1.14554*((1+(-0.000128043*x))+(4.10899e-07*(x*x))))+(-2.07565e-10*(x*(x*(x/(1+(-0.00118618*x))))))", 20., 670.);


        double eff, sf;

        if (discriminator_value > 1.7)//it is b        
        {
            sf = BTagScaleFactor.Eval(pt);
            eff = EffB.Eval(disc);
        } else if (discriminator_value > 1.0) //???? is it C????
        {
            sf = BTagScaleFactor.Eval(pt);
            eff = EffC.Eval(disc);
        } else // it is a light quark
        {
            if (fabs(eta) < 0.5){
                eff = MisTag_Eta0_5.Eval(pt);
                sf = SFlight_Eta0_5.Eval(pt);
            }
            else if (fabs(eta) < 1.0){
                eff = MisTag_Eta5_1.Eval(pt);
                sf = SFlight_Eta5_1.Eval(pt);
            }
            else if (fabs(eta) < 1.5){
                eff = MisTag_Eta1_15.Eval(pt);
                sf = SFlight_Eta1_15.Eval(pt);
            }
            else if (fabs(eta) < 2.4){
                eff = MisTag_Eta15_24.Eval(pt);
                sf = SFlight_Eta15_24.Eval(pt);
            }
        }
        BTagWeight::JetInfo ji(eff, sf);
        JETINFO.push_back(ji);
    }
    float pt;
    float eta;
    float discriminator_value;

    vector<BTagWeight::JetInfo> JETINFO;
};

int main() {
    BTagWeight b(1);



    vector<simpleJet> simpleJets_b;

    simpleJet bj1(200, 2.2, 2.0);
    simpleJets_b.push_back(bj1);

    simpleJet bj2(110, 1.5, 3.0);
    simpleJets_b.push_back(bj2);

    simpleJet bj3(40, 2.3, 1.9);
    simpleJets_b.push_back(bj3);

    vector<simpleJet> simpleJets_c;

    simpleJet cj1(200, 2.2, 1.0);
    simpleJets_c.push_back(cj1);

    simpleJet cj2(110, 1.5, 1.0);
    simpleJets_c.push_back(cj2);

    simpleJet cj3(40, 2.3, 1.5);
    simpleJets_c.push_back(cj3);

    vector<simpleJet> simpleJets_l;

    simpleJet lj1(200, 2.2, 0.02);
    simpleJets_l.push_back(lj1);

    simpleJet lj2(110, 1.5, 0.01);
    simpleJets_l.push_back(lj2);

    simpleJet lj3(40, 2.3, 0.5);
    simpleJets_l.push_back(lj3);

    for (int ib = 1; ib < 4; ib++)
        for (int ic = 0; ic < 4; ic++)
            for (int il = 0; il < 4; il++) {
                vector<vector<BTagWeight::JetInfo> > j;
                for (int i = 0; i < ib; i++)j.push_back(simpleJets_b[i].JETINFO);
                for (int i = 0; i < ic; i++)j.push_back(simpleJets_c[i].JETINFO);
                for (int i = 0; i < il; i++)j.push_back(simpleJets_l[i].JETINFO);

                std::cout << "b:" << ib << ", c:" << ic << ", l:" << il << ", w: " << b.weight(j) << " +- " << 0 << std::endl;
            }
    return 0;
}
#endif
