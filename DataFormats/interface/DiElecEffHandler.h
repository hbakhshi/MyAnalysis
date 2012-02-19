#ifndef DiElecEffHandler_Ele_DiEle_h
#define DiElecEffHandler_Ele_DiEle_h

#include "DiElectronEventProperties.h"

namespace ElectronAnalysis {
    namespace DiElectronEventProperties {

        class AlwaysOne : public ObjectProperty<ElectronAnalysis::Electron> {
        public:

            AlwaysOne() : ObjectProperty<ElectronAnalysis::Electron>("Total", "Total", 0, 2, 2, "Electron", 1288) {
            };

            virtual ~AlwaysOne() {
            };

            virtual double ReadValue(const ElectronAnalysis::Electron* t) const {
                //cout << "AAAA" << endl;
                return 0.5;
            };
        };

        template<int i >
        class AlwaysOneEvent : public EleReader<i> {
        public:

            AlwaysOneEvent() : EleReader<i>(new AlwaysOne()) {
            };

            ~AlwaysOneEvent() {
            };
        };

        template<int CutType, int chStat_>
        bool All(const ElectronAnalysis::DiElectronEvent* evt) {

            //to avoid ambiguities from electron class, I added the number of ChargeState by 100,
            int chStat = chStat_;
            if (chStat < 100)
                chStat += 100;
            ElectronAnalysis::Electron::ChargeState ChStat = (ElectronAnalysis::Electron::ChargeState)(chStat);
            switch (CutType) {
                case 0: //all events
                    return evt->passNewCuts(10000, ChStat, false, false, 0);
                case 1: //cut on d_0
                    return evt->passNewCuts(0.02, ChStat, false, false, 0);
                case 2: //cut on d_0, ctf track
                    return evt->passNewCuts(0.02, ChStat, true, false, 0);
                case 3: //cut on d_0, sc_innermost
                    return evt->passNewCuts(0.02, ChStat, false, true, 0);
                case 4: //cut on all of them
                    return evt->passNewCuts(0.02, ChStat, true, true, 0);
                case 5:
                    return evt->passNewCuts(0.02, ChStat, true, false, 2);
                case 6:
                    return evt->passNewCuts(0.02, ChStat, false, false, 2);
                case 7:
                    return evt->passNewCuts(1000, ChStat, true, false, 0);
                case 8:
                    return evt->passNewCuts(1000, ChStat, false, false, 2);
                case 9:
                    return evt->passNewCuts(1000, ChStat, true, false, 2);
                case 10:
                    return evt->passNewCuts(0.02, ElectronAnalysis::Electron::GSF_CTF1, false, false, 2);
                case 11:
                    return evt->passNewCuts(10000., ElectronAnalysis::Electron::GSF_CTF1, false, false, 0);
                case 12:
                    return evt->passNewCuts(10000, ChStat, false, false, 1);
                case 13:
                    return evt->passNewCuts(10000, ChStat, false, false, 3);
                case 14:
                    return evt->passNewCuts(0.02, ChStat, true, false, 3);
                case 15:
                    return evt->passNewCuts(10000, ChStat, false, false, 4);
                case 16:
                    return evt->passNewCuts(10000, ChStat, false, false, 5);
                case 17:
                    return evt->passNewCuts(10000, ChStat, true, false, 1);
                case 18:
                    return evt->passNewCuts(10000, ChStat, true, false, 3);
                case 19:
                    return evt->passNewCuts(0.02, ChStat, false, false, 3);
            }

            return false;
        }

        template<int Charge>
        bool Accept(const ElectronAnalysis::DiElectronEvent* evt) {
            return evt->IsSameSign((Electron::ChargeState)Charge) == 1;
        }

        template<class prop>
        bool All_(const ElectronAnalysis::DiElectronEvent* evt) {
            prop p;
            float val = p.ReadValue(evt);
            // if( val == p.MinValue - 100000. )
            //   cout << "NAN" << endl;
            // else
            //   cout << val << "    " << p.MinValue - 100000. << endl;
            //cout << "ALL:" << p.PropertyName << endl;

            //bool res = val != NAN_2;
            //cout << "ALL:" << NAN_2 << "  " << val << "  "  << res << endl;
            return ( val != NAN_2 /*p.MinValue - 100000.*/);
        }

        template<class prop>
        bool Accept_(const ElectronAnalysis::DiElectronEvent* evt) {
            prop p;
            //cout << "ACCEPT:" << p.PropertyName << endl;
            if (evt->IsSameSign()) {
                //cout << "it's ss" << endl;
                return ( All_<prop > (evt));
            } else {
                //cout << "opp rejected" << endl;
                return false;
            }
        }

        class DiElecEfficiency : public EfficiencyH1<ElectronAnalysis::DiElectronEvent> {
        public:
            typedef ElectronAnalysis::DiElectronEvent T;
            typedef EfficiencyH1<T> base;

            DiElecEfficiency(ObjectProperty<T>* theProp, std::string name) : base(theProp, name) {
            };

            virtual ~DiElecEfficiency() {
            };

            virtual int Fill(const T* theObject, double weight = 1) {
                int ret = base::Fill(theObject, weight);
                bool allPassed = true;
                if (base::AllCondition != 0)
                    allPassed = base::AllCondition(theObject);
                if (allPassed) {
                    double val = base::theProperty->ReadValue(theObject);
                    base::hAll->Fill(val, weight);
                }

                //cout << "Fill is called" << endl;

                return ret;
            }
        };

        template<int type, class RET>
        void MAKE(RET* ret, vector<double>* args = NULL);

        template<class RET, int type>
        RET* make(string name);

        template<class Eff, class Eff2D = EfficiencyH2<ElectronAnalysis::DiElectronEvent>, int CutType = 0, int ChargeType = 0 >
        class ChargeErrorProbabilityHistograms : public EfficiencyHandler<ElectronAnalysis::DiElectronEvent, Eff, Eff2D> {
        public:
            typedef ElectronAnalysis::DiElectronEvent T;
            typedef EfficiencyHandler<ElectronAnalysis::DiElectronEvent, Eff, Eff2D> base;

            string GetName() const {
                return base::Name;
            }

            virtual ~ChargeErrorProbabilityHistograms() {
            };

            ChargeErrorProbabilityHistograms(string name, vector<double>* args = NULL) : base(name) {
                base::AllCondition = All<CutType, ChargeType>;

                base::Condition = Accept<ChargeType>;
                if (CutType == 10 || CutType == 11)
                    base::Condition = Accept<ElectronAnalysis::Electron::GSF_CTF1>;

                MAKE < 0 > (this, args);
            }

            virtual void Print() {
                EfficiencyH1<T>* alwayss = TheAlwayOne();
                alwayss->ComputeIntegral();
                alwayss->GetHAll()->ComputeIntegral();

                double all = (alwayss->GetHAll()->Integral(0,alwayss->GetNbinsX()));
                double passed = (alwayss->Integral(0,alwayss->GetNbinsX()));

                cout << "| |" << base::Name << "| |" << passed << "| |"
                        << (all - 2*passed)/2.0  ;
                PMM.Print();
                PMM_Barrel.Print();
                PMM_Endcap.Print();
                cout << endl;

//                alwayss->Print();
//                alwayss->GetHAll()->Print();
            };

            virtual void WriteAll(TDirectory* dir) {
                base::WriteAll(dir);

                EfficiencyH1<T>* e = TheAlwayOne();
                PMM.value = e->GetPoissonianEff()->GetBinContent(1);
                PMM.error = e->GetPoissonianEff()->GetBinError(1);

                e = TheEta(true);
                PMM_Barrel.value = e->GetPoissonianEff()->GetBinContent(1);
                PMM_Barrel.error = e->GetPoissonianEff()->GetBinError(1);
                PMM_Endcap.value = e->GetPoissonianEff()->GetBinContent(3);
                PMM_Endcap.error = e->GetPoissonianEff()->GetBinError(3);

                HISTOS.push_back(ThePt()->GetPoissonianEff());
                HISTOS.push_back(TheEta(true)->GetPoissonianEff());
                HISTOS.push_back(TheEta(false)->GetPoissonianEff());
                HISTOS.push_back(TheFBrem()->GetPoissonianEff());
            };

            struct value_error{
                double value;
                double error;
                void Print(){
                    cout << "|" << 100.0*value << "%(" << 100.0*error/value << "%)" ;
                }
            };
            value_error PMM_Endcap;
            value_error PMM_Barrel;
            value_error PMM;

            double Value(int VariableID) {
                if (VariableID == 0)
                    return PMM.value;

                return NAN;
            }

            double Error(int VariableID) {
                if (VariableID == 0)
                    return PMM.error;

                return NAN;
            }

            string GetVariableName(int VariableID) {
                if (VariableID == 0)
                    return "TotalPMM";

                return "";
            }

            int NVariables() {
                return 1; //only tota PMM is to calculated by this class, for IntLumiHandler
            }

            std::vector<TH1*> HISTOS;
        private:

            EfficiencyH1<T>* TheAlwayOne() {
                for (unsigned int i = 0; i < base::GetProperties(); i++) {
                    EfficiencyH1<T>* e = base::GetEffH1(i);
                    if (e->CheckPropType<ElectronAnalysis::DiElectronEventProperties::AlwaysOneEvent < 0 > > ()) {
                        return e;
                    }
                }

                return NULL;
            }

            EfficiencyH1<T>* ThePt() {
                for (unsigned int i = 0; i < base::GetProperties(); i++) {
                    EfficiencyH1<T>* e = base::GetEffH1(i);
                    if (e->CheckPropType<ElectronAnalysis::DiElectronEventProperties::PtOFElectron < 0 > > ()) {
                        return e;
                    }
                }

                return NULL;
            }

            EfficiencyH1<T>* TheEta(bool EndcapBarrel) {
                for (unsigned int i = 0; i < base::GetProperties(); i++) {
                    EfficiencyH1<T>* e = base::GetEffH1(i);
                    if (e->CheckPropType<ElectronAnalysis::DiElectronEventProperties::EtaOFElectron < 0 > > ()) {
                        if(e->GetPropertyID() == 11111 && EndcapBarrel)
                            return e;
                        else if(e->GetPropertyID() != 11111 && !EndcapBarrel)
                            return e;
                    }
                }

                return NULL;
            }

            EfficiencyH1<T>* TheFBrem() {
                for (unsigned int i = 0; i < base::GetProperties(); i++) {
                    EfficiencyH1<T>* e = base::GetEffH1(i);
                    if (e->CheckPropType<ElectronAnalysis::DiElectronEventProperties::BremOFElectron < 0 > > ()) {
                        return e;
                    }
                }

                return NULL;
            }
        };

    }
}

template<class RET, int type>
RET* ElectronAnalysis::DiElectronEventProperties::make(string name) {
    RET* ret = new RET(name);

    ElectronAnalysis::DiElectronEventProperties::MAKE<type, RET > (ret);

    return ret;
}

template<int type, class RET> // the args should contain 3 numbers : #PtBins, #EtaBins and #FBremBins
void ElectronAnalysis::DiElectronEventProperties::MAKE(RET* ret, vector<double>* args) {
    if (type == 2) {
        ret->AddProp(new PtOFElectron < 2 > (), Accept_<PtOFElectron < 2 > >, All_<PtOFElectron < 2 > >);
        ret->AddProp(new EtaOFElectron < 2 > (), Accept_<EtaOFElectron < 2 > >, All_<EtaOFElectron < 2 > >);
        ret->AddProp(new BremOFElectron < 2 > (), Accept_<BremOFElectron < 2 > >, All_<BremOFElectron < 2 > >);
        ret->AddProp(new TIPOFElectron < 2 > (), Accept_<TIPOFElectron < 2 > >, All_<TIPOFElectron < 2 > >);
        ret->AddProp(new CaloErrorOFElectron < 2 > (), Accept_<CaloErrorOFElectron < 2 > >, All_<CaloErrorOFElectron < 2 > >);
    } else {

        if (args == NULL) {
            args = new vector<double>();
            args->push_back(10);
            args->push_back(10);
            args->push_back(10);
        }

        for (uint ll = 0; ll < args->size(); ll++) {
            double value = args->at(ll);
            int What = ll % 3;

            ObjectProperty<ElectronAnalysis::DiElectronEvent>* prop = NULL;
            if (value > 0)
                switch (What) {
                    case 0:
                        prop = new PtOFElectron<type > ();
                        prop->PropertyName = "Pt";
                        prop->MinValue = 7;
                        prop->MaxValue = 207;
                        break;
                    case 1:
                        prop = new EtaOFElectron<type > ();
                        prop->PropertyName = "eta";
                        prop->MaxValue = 2.4;
                        break;
                    case 2:
                        prop = new BremOFElectron<type > ();
                        prop->PropertyName = "brem";
                        break;
                }

            if (prop != NULL) {
                prop->NumberOfBins = int(value);
                std::stringstream ss;
                ss << prop->PropertyName << "_" << prop->NumberOfBins;
                prop->PropertyName = ss.str().c_str();
                ret->AddProp(prop);
            }
        }

        AlwaysOneEvent<type>* a1 = new ElectronAnalysis::DiElectronEventProperties::AlwaysOneEvent<type > ();
        ret->AddProp(a1);
        TIPOFElectron<type,1>* tip_ele = new TIPOFElectron<type,1>();
        ret->AddProp(tip_ele);
        TIPOFElectron<type,0>* tip_ele0 = new TIPOFElectron<type,0>();
        ret->AddProp(tip_ele0);

        EtaOFElectron<type>* eta2 = new ElectronAnalysis::DiElectronEventProperties::EtaOFElectron<type > (true);
        eta2->PropertyName = "BarrelEndcap";
        eta2->PropertyID = 11111;
        eta2->NumberOfBins = 3;
        ret->AddProp(eta2);

        ret->AddProp2(eta2, tip_ele);
        ret->AddProp2(eta2, tip_ele0);
    }
}

#endif
