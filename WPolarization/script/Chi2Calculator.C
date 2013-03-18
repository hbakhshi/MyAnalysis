Double_t myfunc(double * x , double * par){
  double a = (1.-x[0])*(1.-x[0]);
  double b = (1.-x[0]*x[0]);
  double c = (1.+x[0])*(1.+x[0]); 
  if(par[3] == 0.0)
    return par[2]*(3./8.*a*par[0]+3./4.*b*par[1]+3./8.*c*(1.-par[0]-par[1]));
  else if(par[3] == 1.0)
    return par[2]*(3./8.*a*par[0]);
  else if(par[3] == 2.0)
    return par[2]*(3./4.*b*par[1]);
  else if(par[3] == 3.0)
    return par[2]*(3./8.*c*(1.-par[0]-par[1]));
  else
    return -2;
}

void Chi2Calculator(){
  TFile * file =  new TFile("newHist.root");

  TH1D * data = Data->Clone();
  double integral = Data->Integral();
  data->SetTitle("data");
  data->Draw();

  double fL = 0.288288;
  double f0 = 0.697838;

  double sysErrFl = 0.05;
  double sysErrF0 = 0.06;
    

  TF1 *ret = new TF1("ret",myfunc,-1,1,4);
  ret->SetTitle("#frac{d#Gamma}{dcos(#theta)} with obtained F Values");
  ret->FixParameter(0,fL);
  ret->FixParameter(1,f0);
  ret->FixParameter(2,integral*0.2);
  ret->FixParameter(3,0.0);
  ret->SetLineColor(5);
  ret->SetLineWidth(3);
  ret->Draw("sames");

  TF1 *ret_FL = new TF1("ret_FL",myfunc,-1,1,4);
  ret_FL->SetTitle("#frac{d#Gamma}{dcos(#theta)} with obtained F Values");
  ret_FL->FixParameter(0,fL);
  ret_FL->FixParameter(1,f0);
  ret_FL->FixParameter(2,integral*0.2);
  ret_FL->FixParameter(3,1);
  ret_FL->SetLineColor(5);
  ret_FL->SetLineWidth(3);
  //ret_FL->Draw("sames");

  TF1 *ret_F0 = new TF1("ret_F0",myfunc,-1,1,4);
  ret_F0->SetTitle("#frac{d#Gamma}{dcos(#theta)} with obtained F Values");
  ret_F0->FixParameter(0,fL);
  ret_F0->FixParameter(1,f0);
  ret_F0->FixParameter(2,integral*0.2);
  ret_F0->FixParameter(3,2);
  ret_F0->SetLineColor(5);
  ret_F0->SetLineWidth(3);
  //ret_F0->Draw("sames");

  TF1 *ret_FR = new TF1("ret_FR",myfunc,-1,1,4);
  ret_FR->SetTitle("#frac{d#Gamma}{dcos(#theta)} with obtained F Values");
  ret_FR->FixParameter(0,fL);
  ret_FR->FixParameter(1,f0);
  ret_FR->FixParameter(2,integral*0.2);
  ret_FR->FixParameter(3,3);
  ret_FR->SetLineColor(5);
  ret_FR->SetLineWidth(3);
  //ret_FR->Draw("sames");

  TF1 *sysUp = new TF1("sysUp",myfunc,-1,1,4);
  sysUp->SetTitle("#frac{d#Gamma}{dcos(#theta)} with F values + systematics");
  sysUp->FixParameter(0,fL+sysErrFl);
  sysUp->FixParameter(1,f0+sysErrF0);
  sysUp->FixParameter(2,integral*0.2);
  sysUp->FixParameter(3,0);
  sysUp->SetLineWidth(2);
  sysUp->SetLineStyle(3);
  sysUp->SetLineColor(1);
  sysUp->Draw("sames");

  TF1 *sysDown = new TF1("sysDown",myfunc,-1,1,4);
  sysDown->SetTitle("#frac{d#Gamma}{dcos(#theta)} with F values - systematics");
  sysDown->FixParameter(0,fL-sysErrFl);
  sysDown->FixParameter(1,f0-sysErrF0);
  sysDown->FixParameter(2,integral*0.2);
  sysDown->FixParameter(3,0.0); 
  sysDown->SetLineStyle(6);
  sysDown->SetLineColor(1);
  sysDown->Draw("sames");


  double sum = 0.;
  //double sysVals[]= {176.72,384.091,346.372,356.414,287.459,291.953,268.327,207.872,166.456,127.62}; 
  double sysVals[]= {90.59697042396071, 192.23064811574682, 226.70406193978965, 229.8165524935053, 200.57849747667385, 154.48757903145494, 164.76872147042945, 106.78847199019181, 100.0665947007292, 61.810640451300976};
  for (int i = 1; i <= data->GetNbinsX(); i++) {
    double dataCont = data->GetBinContent(i);
    double xvalue = data->GetBinCenter(i);
    cout<<">> bin: "<<i<<" ; center of the bin: "<<data->GetBinCenter(i)<<" with error: "<<data->GetBinError(i)<<endl;
    cout<<"		sqrt(N): "<<sqrt(dataCont)<<endl;
    cout<<"		--- contains: "<<dataCont<<" entries"<<endl;
    cout<<"		--- the value of function at the center of the bin: "<<ret->Eval(xvalue)<<endl;
    double funcVal = ret->Eval(xvalue);

    double sysErr = fabs(sysUp->Eval(xvalue) - funcVal);
    //double sysErr =  sysVals[ i-1 ];
    cout<<"			** direct implementation of systematic unc. on the F parameters: "<<sysErr<<endl;

    double chi2 = dataCont-funcVal;
    double err = data->GetBinError(i);
    double sigmaSqrd=err*err+sysErr*sysErr;
    double val = chi2*chi2/sigmaSqrd;
    sum = sum + val;
  }

  cout<<""<<endl;
  cout<<"** sum: "<<sum<<endl;
  cout<<"*** chi2/ndof: "<<sum/6<<endl;
}
