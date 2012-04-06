#include "WPolarization/interface/BTagWeight.h"

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

void BTagWeight::GetEffSF_TCHEL(double pt, double eta, double discriminator_value, double& eff, double& sf) {
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


    //double eff, sf;

    if (discriminator_value > 1.7)//it is b        
    {
        sf = BTagScaleFactor.Eval(pt);
        eff = EffB.Eval(discriminator_value);
    }
        /*else if (discriminator_value > 1.0) //???? is it C????
        {
            sf = BTagScaleFactor.Eval(pt);
            eff = EffC.Eval(disc);
        }*/
    else // it is a light quark
    {
        if (fabs(eta) < 0.5) {
            eff = MisTag_Eta0_5.Eval(pt);
            sf = SFlight_Eta0_5.Eval(pt);
        } else if (fabs(eta) < 1.0) {
            eff = MisTag_Eta5_1.Eval(pt);
            sf = SFlight_Eta5_1.Eval(pt);
        } else if (fabs(eta) < 1.5) {
            eff = MisTag_Eta1_15.Eval(pt);
            sf = SFlight_Eta1_15.Eval(pt);
        } else if (fabs(eta) < 2.4) {
            eff = MisTag_Eta15_24.Eval(pt);
            sf = SFlight_Eta15_24.Eval(pt);
        }
    }
}
