
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

#define multimapTest
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
 
    EfficiencyHandlerReader<FakeInfo>* effReader = new EfficiencyHandlerReader<FakeInfo>(f,"FakeRates_Full80_Loose95");
    
    FakeInfo::PT_Property pt("El");
    FakeInfo::Eta_Property eta("El", true,true);

    Efficiency1DObjects eff_pt(effReader->Get(&pt));
    eff_pt.Print();

//    Efficiency1DObjects eff_eta(effReader->Get(&eta));
//    eff_eta.Print();

    Efficiency2DObjects eff_eta_pt(effReader->Get(&eta,&pt));
    eff_eta_pt.Print();

}

#endif