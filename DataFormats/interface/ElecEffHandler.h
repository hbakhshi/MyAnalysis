#ifndef ElecEffHandler_Ele_DiEle_h
#define ElecEffHandler_Ele_DiEle_h

#include "Electron.h"


namespace ElectronAnalysis {
    namespace ElectronProperties {
      
      template<int genStat>
      bool All(const ElectronAnalysis::Electron* ele){
	switch(genStat){
	case 1:
	  return fabs(ele->GenID)==11;
	case 3:
	  return fabs(ele->GenID)==11;
	}
	return false;
      }
      
      template<int genStat>
      bool Accept(const ElectronAnalysis::Electron* ele){
	bool ret = false;

	switch(genStat){
	case 1:
	  if(fabs(ele->GenID)==11)
	    ret = !(ele->IsBadCharge( true ));
	  break;
	case 3:
	  if(fabs(ele->GenID)==11)
	    ret = !(ele->IsBadCharge( false ));
	  break;
	}

	return ret;
      }

     class ChargeErrorProbabilityHistograms : public EfficiencyHandler<ElectronAnalysis::Electron>{
      public:
	typedef EfficiencyHandler<ElectronAnalysis::Electron> base;
	ChargeErrorProbabilityHistograms(string name, int genstat) : base(name),
								     GenStat(genstat)
	{
	  if( genstat == 1){
	    base::AllCondition = All<1> ;
	    base::Condition    = Accept<1>;
	  }
	  else{
	    base::AllCondition = All<3> ;
	    base::Condition    = Accept<3>;
	  }

	  Pt* pt = new Pt();
	  pt->MinValue = 7;
	  pt->MaxValue = 207;
	  pt->NumberOfBins = 12;
	  AddProp(pt);

	  Eta* eta = new Eta();
          eta->NumberOfBins = 12;
          eta->MaxValue = 2.4;
	  AddProp(eta);

	  BremFraction * brem = new BremFraction();
	  brem->NumberOfBins = 12;
	  AddProp(brem);

	  ObjectProperty<ElectronAnalysis::Electron>* tip = abs(new TIP < 0 > ());
	  tip->NumberOfBins = 10;
	  AddProp(tip);

//	  CaloEnergyError * cee = new CaloEnergyError();
//	  cee->NumberOfBins = 20;
//	  AddProp(cee);

	  AddProp2(pt, eta);
	  AddProp2(pt, brem);
//	  AddProp2(pt, cee);
	  AddProp2(pt, tip);

	  AddProp2(eta , brem);
//	  AddProp2(eta , cee);
	  AddProp2(eta , tip);

//	  AddProp2(brem , cee);
	  AddProp2(brem , tip);

//	  AddProp2(cee , tip);
	};
	
	virtual ~ChargeErrorProbabilityHistograms(){};

	int GenStat;
      };
    }
}

#endif
