#ifndef DiElecEffHandler_Ele_DiEle_IJ_h
#define DiElecEffHandler_Ele_DiEle_IJ_h

#include "DiElecEffHandler.h"

namespace ElectronAnalysis {
    namespace DiElectronEventProperties {
        //the efficiency, based on Luc idea !
        //typedef DiElecEfficiency DiElecEfficiencyLucian;

        template<int i> //the i for what the class does is not important. but to be able to store an EleReader it's neccessary
        class DiElecEfficiencyLucian : public EfficiencyH1<ElectronAnalysis::DiElectronEvent> {
        public:
            typedef ElectronAnalysis::DiElectronEvent T;
            typedef EfficiencyH1<T> base;

            EleReader<i>* theProp__;

            DiElecEfficiencyLucian(ObjectProperty<T>* theProp, std::string name) :
            base(theProp, name) {
                theProp__ = (EleReader<i>*)(theProp);
                if (theProp__ == 0) {
                    cout << "DiElecEfficiencyLucianSays: theProp should be an EleReader -- " << theProp->PropertyName << endl;
                    throw std::exception();
                }
            };

            virtual ~DiElecEfficiencyLucian() {
            };

            virtual int Fill(const T* theObject, double weight = 1) {
                //int ret = base::Fill(theObject , weight);
                if (theObject == 0) {
                    cout << "DiElecEfficiencyLucian : theObject is NULL !!!" << endl;
                    throw std::exception();
                } else {
                    bool allPassed = true;
                    if (AllCondition != 0)
                        allPassed = AllCondition(theObject);

                    //cout << " Fill : " << allPassed ;

                    if (allPassed) {
                        //cout << this->theProp__->PropertyName << endl;
                        double val1 = this->theProp__->reader->ReadValue(theObject->FirstElectron_.get());
                        double val2 = this->theProp__->reader->ReadValue(theObject->SecondElectron_.get());

                        //cout << "OK" ;

                        if (Condition(theObject)) {
                            //cout << " :)" << endl ;
                            TH1D::Fill(val1, weight);
                            return TH1D::Fill(val2, weight);
                        } else {
                            //cout << " :(" << endl ;
                            hAll->Fill(val1, weight);
                            return hAll->Fill(val2, weight);
                        }
                    }
                }

                //cout << endl;

                return -10;

            };

            virtual void WriteAll(TDirectory* dir, bool mkdir = true) {
                double W = double( TH1D::GetEntries()) / (2.0 * double(base::GetHAll()->GetEntries()));

                string axis_title(this->theProp__->reader->PropertyTitle);
                TH1D::GetXaxis()->SetTitle(axis_title.c_str());
                base::GetHAll()->GetXaxis()->SetTitle(axis_title.c_str());

                TH1D h_SS(*this); // a copy to be written
                TH1D h_OS(* (base::GetHAll())); // a copy to be written
                double val, newVal, all, newAll;

                int nxbins = TH1D::GetXaxis()->GetNbins();
                for (int jj = 1; jj < (nxbins + 1); jj++) {
                    val = TH1D::GetBinContent(jj);
                    all = base::GetHAll()->GetBinContent(jj);
                    newVal = val - W*all;
                    //newAll = all*(1 + 2*W);
                    newAll = all + val;

                    TH1D::SetBinContent(jj, 0.0);
                    base::GetHAll()->SetBinContent(jj, newAll);
                    if (newVal > 0.)
                        TH1D::SetBinContent(jj, newVal);
                }

                base::WriteAll(dir, true);

                dir->cd(this->theProperty->PropertyName.c_str());
                h_OS.Write((string(h_OS.GetName()) + "__ALL_OS_").c_str());
                h_SS.Write((string(h_SS.GetName()) + "__ALL_SS_").c_str());
                dir->cd();
            };
        };

        template<int i> //the i for what the class does is not important. but to be able to store an EleReader it's neccessary
        class DiElecEfficiencyLucian2D : public EfficiencyH2<ElectronAnalysis::DiElectronEvent> {
        public:
            typedef ElectronAnalysis::DiElectronEvent T;
            typedef EfficiencyH2<T> base;

            EleReader<i>* theProp_1;
            EleReader<i>* theProp_2;

            DiElecEfficiencyLucian<i> EffProp1;
            DiElecEfficiencyLucian<i> EffProp2;

            DiElecEfficiencyLucian2D(ObjectProperty<T>* theProp1, ObjectProperty<T>* theProp2, std::string name) :
            base(theProp1, theProp2, name),
            EffProp1(theProp1, name + "1"),
            EffProp2(theProp2, name + "2") {
                theProp_1 = (EleReader<i>*)(theProp1);
                theProp_2 = (EleReader<i>*)(theProp2);
                if (theProp_1 == 0 || theProp_2 == 0) {
                    cout << "DiElecEfficiencyLucianSays: theProp should be an EleReader -- " << theProp_1->PropertyName << endl;
                    throw std::exception();
                }
            };

            virtual ~DiElecEfficiencyLucian2D() {
            };

            virtual int Fill(const T* theObject, double weight = 1) {
                //int ret = base::Fill(theObject , weight);
                if (theObject == 0) {
                    cout << "DiElecEfficiencyLucian : theObject is NULL !!!" << endl;
                    throw std::exception();
                } else {
                    EffProp1.Condition = EffProp2.Condition = this->Condition;
                    EffProp1.AllCondition = EffProp2.AllCondition = this->AllCondition;
                    EffProp2.Fill(theObject, weight);
                    EffProp1.Fill(theObject, weight);

                    bool allPassed = true;
                    if (AllCondition != 0)
                        allPassed = AllCondition(theObject);

                    if (allPassed) {
                        double val1_1 = this->theProp_1->reader->ReadValue(theObject->FirstElectron_.get());
                        double val2_1 = this->theProp_2->reader->ReadValue(theObject->FirstElectron_.get());

                        double val1_2 = this->theProp_1->reader->ReadValue(theObject->SecondElectron_.get());
                        double val2_2 = this->theProp_2->reader->ReadValue(theObject->SecondElectron_.get());
                        if (Condition(theObject)) {
                            TH2D::Fill(val1_1, val2_1, weight);
                            return TH2D::Fill(val1_2, val2_2, weight);
                        } else {
                            hAll->Fill(val1_1, val2_1, weight);
                            return hAll->Fill(val1_2, val2_2, weight);
                        }
                    }
                }
                return -10;

            };

            virtual void WriteAll(TDirectory* dir, bool mkdir = true) {
                double W = double( TH2D::GetEntries()) / (2.0 * double(base::GetHAll()->GetEntries()));

                string axis_title(this->theProp_1->reader->PropertyTitle);
                TH2D::GetXaxis()->SetTitle(axis_title.c_str());
                base::GetHAll()->GetXaxis()->SetTitle(axis_title.c_str());
                GetPoissonianEff()->SetXTitle(axis_title.c_str());

                string axis_title2(this->theProp_2->reader->PropertyTitle);
                TH2D::GetYaxis()->SetTitle(axis_title2.c_str());
                base::GetHAll()->GetYaxis()->SetTitle(axis_title2.c_str());
                GetPoissonianEff()->SetYTitle(axis_title2.c_str());

                TH2D h_SS(*this); // a copy to be written
                TH2D h_OS(* (base::GetHAll())); // a copy to be written
                double val, newVal, all, newAll;

                int nxbins = TH2D::GetXaxis()->GetNbins();
                int nybins = TH2D::GetYaxis()->GetNbins();

                for (int jj = 1; jj < nxbins; jj++)
                    for (int ii = 1; ii < nybins; ii++) {

                        val = TH2D::GetBinContent(jj, ii);
                        all = base::GetHAll()->GetBinContent(jj, ii);

                        newVal = val - W*all;
                        //newAll = all*(1 + 2*W);
                        newAll = all + val;

                        TH2D::SetBinContent(jj, ii, 0.0);
                        base::GetHAll()->SetBinContent(jj, ii, newAll);
                        if (newVal > 0.)
                            TH2D::SetBinContent(jj, ii, newVal);
                    }

                base::WriteAll(dir, true);

                dir->cd(GetMixedName().c_str());
                EffProp1.WriteAll(gDirectory);
                dir->cd(GetMixedName().c_str());
                EffProp2.WriteAll(gDirectory);
                dir->cd(GetMixedName().c_str());

                TH1* hp1 = EffProp1.GetPoissonianEff();
                TH1* hp2 = EffProp2.GetPoissonianEff();

                TAxis* ax1 = hp1->GetXaxis();
                TAxis* ax2 = hp2->GetXaxis();

                TH2D hp12("Multiplication", "Multiplication",
                        hp1->GetNbinsX(), ax1->GetXmin(), ax1->GetXmax(),
                        hp2->GetNbinsX(), ax2->GetXmin(), ax2->GetXmax());

                hp12.GetXaxis()->SetTitle(ax1->GetTitle());
                hp12.GetYaxis()->SetTitle(ax2->GetTitle());
                TH2D hdiff(*GetPoissonianEff());

                double multi_val, diff_val;
                for (int i1 = 1; i1 < hp1->GetNbinsX(); i1++)
                    for (int i2 = 1; i2 < hp2->GetNbinsX(); i2++) {
                        multi_val = hp1->GetBinContent(i1) * hp2->GetBinContent(i2);
                        hp12.SetBinContent(i1, i2, multi_val);

                        if (hdiff.GetBinContent(i1, i2) != 0)
                            diff_val = fabs(multi_val / hdiff.GetBinContent(i1, i2)); //it should be equal to P(bad) in case of independence of two variables
                        else
                            diff_val = 0.;
                        hdiff.SetBinContent(i1, i2, diff_val);
                    }

                hp12.Write();
                hdiff.Write("Differences");

                h_OS.Write((string(h_OS.GetName()) + "__ALL_OS_").c_str());
                h_SS.Write((string(h_SS.GetName()) + "__ALL_SS_").c_str());
                dir->cd();
            };
        };

        using namespace ElectronAnalysis::DiElectronEventProperties;

        template<int charge>
        class CERL_CHARGE {
        public:
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 0, charge > ceph_deel0;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 1, charge > ceph_deel1;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 2, charge > ceph_deel2;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 5, charge > ceph_deel5;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 6, charge > ceph_deel6;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 7, charge > ceph_deel7;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 8, charge > ceph_deel8;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 9, charge > ceph_deel9;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 10, charge > ceph_deel10;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 11, charge > ceph_deel11;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 12, charge > ceph_deel12;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 13, charge > ceph_deel13;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 14, charge > ceph_deel14;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 19, charge > ceph_deel19;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 15, charge > ceph_deel15;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 16, charge > ceph_deel16;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 17, charge > ceph_deel17;
            typedef ChargeErrorProbabilityHistograms<DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 >, 18, charge > ceph_deel18;

            string Name;

            ceph_deel0 chargeErrorRecoLucian0;
            ceph_deel1 chargeErrorRecoLucian1;
            ceph_deel2 chargeErrorRecoLucian2;
            ceph_deel5* chargeErrorRecoLucian5;
            ceph_deel6 chargeErrorRecoLucian6;
            ceph_deel7 chargeErrorRecoLucian7;
            ceph_deel8 chargeErrorRecoLucian8;
            ceph_deel9 chargeErrorRecoLucian9;
            ceph_deel10 chargeErrorRecoLucian10;
            ceph_deel11 chargeErrorRecoLucian11;
            ceph_deel12 chargeErrorRecoLucian12;
            ceph_deel13 chargeErrorRecoLucian13;
            ceph_deel14 chargeErrorRecoLucian14;
            ceph_deel19 chargeErrorRecoLucian19;
            ceph_deel15 chargeErrorRecoLucian15;
            ceph_deel16 chargeErrorRecoLucian16;
            ceph_deel17 chargeErrorRecoLucian17;
            ceph_deel18 chargeErrorRecoLucian18;

            std::map< string, EfficiencyHandler<ElectronAnalysis::DiElectronEvent, DiElecEfficiencyLucian < 0 >, DiElecEfficiencyLucian2D < 0 > >* > allCEPHs;

            CERL_CHARGE(string name) : Name(name),
            chargeErrorRecoLucian0("All"),
            chargeErrorRecoLucian1("D0"),
            chargeErrorRecoLucian2("D0CTF"),
            chargeErrorRecoLucian6("D0Conv"),
            chargeErrorRecoLucian7("OnlyCTF"),
            chargeErrorRecoLucian8("OnlyConv"),
            chargeErrorRecoLucian9("CTFConv"),
            chargeErrorRecoLucian10("D0CTF1Conv"),
            chargeErrorRecoLucian11("CTF1"),
            chargeErrorRecoLucian12("ConvGeo"),
            chargeErrorRecoLucian13("ConvTight"),
            chargeErrorRecoLucian14("CTFConvTightD0"),
            chargeErrorRecoLucian15("ConvHit1"),
            chargeErrorRecoLucian16("ConvHit0"),
            chargeErrorRecoLucian17("CTFConvGeo"),
            chargeErrorRecoLucian18("CTFConvTight"),
            chargeErrorRecoLucian19("D0ConvTight"){
                allCEPHs[chargeErrorRecoLucian0.GetName()] = &chargeErrorRecoLucian0;
                allCEPHs[chargeErrorRecoLucian1.GetName()] = &chargeErrorRecoLucian1;
                allCEPHs[chargeErrorRecoLucian2.GetName()] = &chargeErrorRecoLucian2;
                allCEPHs[chargeErrorRecoLucian6.GetName()] = &chargeErrorRecoLucian6;
                allCEPHs[chargeErrorRecoLucian7.GetName()] = &chargeErrorRecoLucian7;
                allCEPHs[chargeErrorRecoLucian8.GetName()] = &chargeErrorRecoLucian8;
                allCEPHs[chargeErrorRecoLucian9.GetName()] = &chargeErrorRecoLucian9;
                allCEPHs[chargeErrorRecoLucian10.GetName()] = &chargeErrorRecoLucian10;
                allCEPHs[chargeErrorRecoLucian11.GetName()] = &chargeErrorRecoLucian11;
                allCEPHs[chargeErrorRecoLucian12.GetName()] = &chargeErrorRecoLucian12;
                allCEPHs[chargeErrorRecoLucian13.GetName()] = &chargeErrorRecoLucian13;
                allCEPHs[chargeErrorRecoLucian14.GetName()] = &chargeErrorRecoLucian14;
                allCEPHs[chargeErrorRecoLucian15.GetName()] = &chargeErrorRecoLucian15;
                allCEPHs[chargeErrorRecoLucian16.GetName()] = &chargeErrorRecoLucian16;
                allCEPHs[chargeErrorRecoLucian17.GetName()] = &chargeErrorRecoLucian17;
                allCEPHs[chargeErrorRecoLucian18.GetName()] = &chargeErrorRecoLucian18;
                allCEPHs[chargeErrorRecoLucian19.GetName()] = &chargeErrorRecoLucian19;

                double AllArgs[] = {1., 1., 1., 2., 2., 2., 4., 4., 4., 5., 5., 5., 6., 6., 6., 7., 7., 7., 8., 8., 8., 10., 10., 10., 12., 12., 12.};
                chargeErrorRecoLucian5 = new ceph_deel5("D0CTFConv", new vector<double>(AllArgs, AllArgs + 27));
                allCEPHs[chargeErrorRecoLucian5->GetName()] = chargeErrorRecoLucian5;
            }

            void Fill(const ElectronAnalysis::DiElectronEvent* ev) {
                chargeErrorRecoLucian0.Fill(ev, ev->Weight);
                chargeErrorRecoLucian1.Fill(ev, ev->Weight);
                chargeErrorRecoLucian2.Fill(ev, ev->Weight);
                chargeErrorRecoLucian5->Fill(ev, ev->Weight);
                chargeErrorRecoLucian6.Fill(ev, ev->Weight);
                chargeErrorRecoLucian7.Fill(ev, ev->Weight);
                chargeErrorRecoLucian8.Fill(ev, ev->Weight);
                chargeErrorRecoLucian9.Fill(ev, ev->Weight);
                chargeErrorRecoLucian10.Fill(ev, ev->Weight);
                chargeErrorRecoLucian11.Fill(ev, ev->Weight);
                chargeErrorRecoLucian12.Fill(ev, ev->Weight);
                chargeErrorRecoLucian13.Fill(ev, ev->Weight);
                chargeErrorRecoLucian14.Fill(ev, ev->Weight);
                chargeErrorRecoLucian15.Fill(ev, ev->Weight);
                chargeErrorRecoLucian16.Fill(ev, ev->Weight);
                chargeErrorRecoLucian17.Fill(ev, ev->Weight);
                chargeErrorRecoLucian18.Fill(ev, ev->Weight);
                chargeErrorRecoLucian19.Fill(ev, ev->Weight);
            }

            std::vector<string> PrintOrder;

            void WriteAll(TDirectory* _file) {
                TDirectory* newDir = _file->mkdir(Name.c_str());
                newDir->cd();

                cout << "- " << Name << endl;
                cout << "|  PMM | Cut        | Killed SS |    #SS | Killed OS |     #OS | Total|Barrel|Endcap" << endl;
                cout << "|-" << endl;
                chargeErrorRecoLucian0.WriteAll(newDir);
                chargeErrorRecoLucian0.Print();

                for (std::vector<string>::const_iterator itr = PrintOrder.begin();
                        itr != PrintOrder.end(); itr++) {

                    this->allCEPHs[*itr]->WriteAll(newDir);
                    this->allCEPHs[*itr]->Print();
                }

                cout << "#+TBLFM: $1=100*$4/(2*($4+$6));%.2f::$3=100-100*$4/@2$4;%.1f::$5=100-100*$6/@2$6;%.1f" << endl;

                cout << endl;
            }
        };

    }
}

#endif
