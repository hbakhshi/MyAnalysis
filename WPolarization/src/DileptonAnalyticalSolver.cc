/**
    @file DileptonAnalyticalSolver.cc

    @author Lars Sonnenschein <Lars.Sonnenschein@cern.ch>

    @brief See the header file DileptonAnalyticalSolver.h.

    @par Adapted to CMSSW by Haryo Sumowidagdo <Suharyo.Sumowidagdo@cern.ch>

    @date Tue Aug 23 12:51:04 CEST 2011

    @version $Id: DileptonAnalyticalSolver.cc,v 1.1 2011/08/23 12:34:58 haryo Exp $

 */


#include "WPolarization/interface/DileptonAnalyticalSolver.h"


namespace llsolver {

void
DileptonAnalyticalSolver::solve(double* ETmiss,
                                double* b,
                                double* bb,
                                double* lp,
                                double* lm,
                                double mWp,
                                double mWm,
                                double mt,
                                double mtb,
                                double mnu,
                                double mnub,
                                vector<double> *pnux,
                                vector<double> *pnuy,
                                vector<double> *pnuz,
                                vector<double> *pnubx,
                                vector<double> *pnuby,
                                vector<double> *pnubz,
                                vector<double> *cd_diff,
                                int& cubic_single_root_cmplx)
{

  cubic_single_root_cmplx = 0;

  double radic = sqr(b[0])-sqr(b[1])-sqr(b[2])-sqr(b[3]);
  double mb = 0.;
  if (radic > 0.) mb = sqrt(radic);

  radic = sqr(bb[0])-sqr(bb[1])-sqr(bb[2])-sqr(bb[3]);
  double mbb = 0.;
  if (radic > 0.) mbb = sqrt(radic);

  radic = sqr(lp[0])-sqr(lp[1])-sqr(lp[2])-sqr(lp[3]);
  double mlp = 0.;
  if (radic > 0.) mlp = sqrt(radic);

  radic = sqr(lm[0])-sqr(lm[1])-sqr(lm[2])-sqr(lm[3]);
  double mlm = 0.;
  if (radic > 0.) mlm = sqrt(radic);

  //not needed but nice to have it derived and written down:
  //double mbnu = sqrt(sqr(mt)-sqr(mWp)+sqr(mb)+sqr(mlp)-sqr(mblp));




  double a1 = (b[0]+lp[0])*(sqr(mWp)-sqr(mlp)-sqr(mnu))-lp[0]*(sqr(mt)-sqr(mb)-sqr(mlp)-sqr(mnu))+2.*b[0]*sqr(lp[0])
              -2.*lp[0]*(b[1]*lp[1]+b[2]*lp[2]+b[3]*lp[3]);
  double a2 = 2.*(b[0]*lp[1]-lp[0]*b[1]);
  double a3 = 2.*(b[0]*lp[2]-lp[0]*b[2]);
  double a4 = 2.*(b[0]*lp[3]-lp[0]*b[3]);

  //c00*pnuy^2+(c10*pnux+c11)*pnuy+(c20*pnux^2+c21*pnux+c22)
  double c22 = sqr((sqr(mWp)-sqr(mlp)-sqr(mnu))*a4)-4.*(sqr(lp[0])-sqr(lp[3]))*sqr(a1)
    -4.*(sqr(mWp)-sqr(mlp)-sqr(mnu))*lp[3]*a1*a4; //c1
  double c21 = -8.*(sqr(lp[0])-sqr(lp[3]))*a1*a2+4.*(sqr(mWp)-sqr(mlp)-sqr(mnu))*(lp[1]*sqr(a4)-lp[3]*a2*a4)-8.*lp[1]*lp[3]*a1*a4; //c2
  double c11 = -8.*(sqr(lp[0])-sqr(lp[3]))*a1*a3+4.*(sqr(mWp)-sqr(mlp)-sqr(mnu))*(lp[2]*sqr(a4)-lp[3]*a3*a4)-8.*lp[2]*lp[3]*a1*a4; //c3
  double c20 = -4.*(sqr(lp[0])-sqr(lp[1]))*sqr(a4)-4.*(sqr(lp[0])-sqr(lp[3]))*sqr(a2)-8.*lp[1]*lp[3]*a2*a4; //c4
  double c10 = -8.*(sqr(lp[0])-sqr(lp[3]))*a2*a3+8.*lp[1]*lp[2]*sqr(a4)-8.*lp[1]*lp[3]*a3*a4-8.*lp[2]*lp[3]*a2*a4; //c5
  double c00 = -4.*(sqr(lp[0])-sqr(lp[2]))*sqr(a4)-4.*(sqr(lp[0])-sqr(lp[3]))*sqr(a3)-8.*lp[2]*lp[3]*a3*a4; //c6


  double b1 = (bb[0]+lm[0])*(sqr(mWm)-sqr(mlm)-sqr(mnub))-lm[0]*(sqr(mtb)-sqr(mbb)-sqr(mlm)-sqr(mnub))+2.*bb[0]*sqr(lm[0])
              -2.*lm[0]*(bb[1]*lm[1]+bb[2]*lm[2]+bb[3]*lm[3]);
  double b2 = 2.*(bb[0]*lm[1]-lm[0]*bb[1]);
  double b3 = 2.*(bb[0]*lm[2]-lm[0]*bb[2]);
  double b4 = 2.*(bb[0]*lm[3]-lm[0]*bb[3]);


  double dp22 = sqr((sqr(mWm)-sqr(mlm)-sqr(mnub))*b4)-4.*(sqr(lm[0])-sqr(lm[3]))*sqr(b1)
    -4.*(sqr(mWm)-sqr(mlm)-sqr(mnub))*lm[3]*b1*b4; //d1
  double dp21 = -8.*(sqr(lm[0])-sqr(lm[3]))*b1*b2+4*(sqr(mWm)-sqr(mlm)-sqr(mnub))*(lm[1]*sqr(b4)-lm[3]*b2*b4)-8.*lm[1]*lm[3]*b1*b4; //d2
  double dp11 = -8.*(sqr(lm[0])-sqr(lm[3]))*b1*b3+4*(sqr(mWm)-sqr(mlm)-sqr(mnub))*(lm[2]*sqr(b4)-lm[3]*b3*b4)-8.*lm[2]*lm[3]*b1*b4; //d3
  double dp20 = -4.*(sqr(lm[0])-sqr(lm[1]))*sqr(b4)-4.*(sqr(lm[0])-sqr(lm[3]))*sqr(b2)-8.*lm[1]*lm[3]*b2*b4; //d4
  double dp10 = -8.*(sqr(lm[0])-sqr(lm[3]))*b2*b3+8.*lm[1]*lm[2]*sqr(b4)-8.*lm[1]*lm[3]*b3*b4-8.*lm[2]*lm[3]*b2*b4; //d5
  double dp00 = -4.*(sqr(lm[0])-sqr(lm[2]))*sqr(b4)-4.*(sqr(lm[0])-sqr(lm[3]))*sqr(b3)-8.*lm[2]*lm[3]*b3*b4; //d6



  //d00*pnuy^2+(d10*pnux+d11)*pnuy+(d20*pnux^2+d21*pnux+d22)
  double d22 = dp22+sqr(ETmiss[0])*dp20+sqr(ETmiss[1])*dp00
              +ETmiss[0]*ETmiss[1]*dp10+ETmiss[0]*dp21+ETmiss[1]*dp11;
  double d20 = dp20;
  double d00 = dp00;
  double d10 = dp10;
  double d21 = -dp21-2.*ETmiss[0]*dp20-ETmiss[1]*dp10;
  double d11 = -dp11-2.*ETmiss[1]*dp00-ETmiss[0]*dp10;


  //                  |c0    d0   |
  // resultant(pnuy)= |c1 c0 d1 d0|
  //                  |c2 c1 d2 d1|
  //                  |   c2    d2|
  //
  //expressions c0,1,2,d0,1,2 are polynomials in pnux of degree 2, to be multiplied out below

  vector<double> polx(5);

  //publication formulae
  polx[0] = sqr(c00)*sqr(d22)+c11*d22*(c11*d00-c00*d11)
           +c00*c22*(sqr(d11)-2.*d00*d22)+c22*d00*(c22*d00-c11*d11); //x^0

  polx[1] = c00*d21*(2.*c00*d22-c11*d11)+c00*d11*(2.*c22*d10+c21*d11)
           +c22*d00*(2.*c21*d00-c11*d10)-c00*d22*(c11*d10+c10*d11)
           -2.*c00*d00*(c22*d21+c21*d22)-d00*d11*(c11*c21+c10*c22)
         +c11*d00*(c11*d21+2.*c10*d22); //X^1

  polx[2] = sqr(c00)*(2.*d22*d20+sqr(d21))-c00*d21*(c11*d10+c10*d11)
           +c11*d20*(c11*d00-c00*d11)+c00*d10*(c22*d10-c10*d22)
           +c00*d11*(2.*c21*d10+c20*d11)+sqr(d00)*(2.*c22*c20+sqr(c21))
           -2.*c00*d00*(c22*d20+c21*d21+c20*d22)
           +c10*d00*(2.*c11*d21+c10*d22)-d00*d10*(c11*c21+c10*c22)
       -d00*d11*(c11*c20+c10*c21); //x^2

  polx[3] = c00*d21*(2.*c00*d20-c10*d10)-c00*d20*(c11*d10+c10*d11)
           +c00*d10*(c21*d10+2.*c20*d11)-2.*c00*d00*(c21*d20+c20*d21)
           +c10*d00*(2.*c11*d20+c10*d21)+c20*d00*(2.*c21*d00-c10*d11)
           -d00*d10*(c11*c20+c10*c21); //x^3

  polx[4] = sqr(c00)*sqr(d20)+c10*d20*(c10*d00-c00*d10)+c20*d10*(c00*d10-c10*d00)
           +c20*d00*(c20*d00-2.*c00*d20); //x^4


  vector<double> pnuxt;
  //int cubic_single_root_cmplx;
  quartic(polx, &pnuxt, cubic_single_root_cmplx);

  double c0 = c00;
  double c1, c2;
  double d0 = d00;
  double d1, d2;
  for (size_t i=0; i<pnuxt.size(); ++i) {
    c1 = c10*pnuxt[i]+c11;
    c2 = c20*sqr(pnuxt[i])+c21*pnuxt[i]+c22;
    d1 = d10*pnuxt[i]+d11;
    d2 = d20*sqr(pnuxt[i])+d21*pnuxt[i]+d22;
    double denom = c1*d0-c0*d1;

    (*cd_diff).push_back(denom); //should never happen
    if (fabs(denom) < epsilon) continue;

    double lpbz_diff = lp[0]*b[3]-b[0]*lp[3];
    double lmbbz_diff = lm[0]*bb[3]-bb[0]*lm[3];
    double thispnux = pnuxt[i];
    double thispnuy = (c0*d2-c2*d0)/denom;
    double thispnubx = ETmiss[0]-pnuxt[i];
    double thispnuby = ETmiss[1]-thispnuy;
    double thispnuz, thispnubz;

    int error=0;
    if (fabs(lpbz_diff)<epsilon) { //circumvent singularity
      error = algebraic_pz(b, lp, mWp, mt, mb, mlp, mnu, thispnux, thispnuy, &thispnuz);
      if (error==-1) continue; //next pnux, pnubx solutions
    }
    else {
      thispnuz = (-a1-a2*thispnux-a3*thispnuy)/a4;
    }

    if (fabs(lmbbz_diff)<epsilon) { //circumvent singularity
      error = algebraic_pz(bb, lm, mWm, mtb, mbb, mlm, mnub, thispnubx, thispnuby, &thispnubz);
      if (error == -1) continue; //next pnux, pnubx solutions
    }
    else {
      thispnubz = (-b1-b2*thispnubx-b3*thispnuby)/b4;
    }

    (*pnux).push_back(thispnux);
    (*pnuy).push_back(thispnuy);
    (*pnuz).push_back(thispnuz);
    (*pnubx).push_back(thispnubx);
    (*pnuby).push_back(thispnuby);
    (*pnubz).push_back(thispnubz);


  }


  return;

}

void
DileptonAnalyticalSolver::quartic(vector<double> polx,
                                  vector<double> *pnux,
                                  int& cubic_single_root_cmplx)
{
  vector<double> polxt(polx.size());
  cubic_single_root_cmplx = 0;
  if (polx[4] == 0.)
    cubic(polx, pnux);
  else {
    for (size_t i=0; i<polxt.size(); ++i) {
      polxt[i]=polx[i]/polx.back(); //normilize to coefficient of highest order (=pnux^4)
      //cout << "polxt[" << i << "]=" << polxt[i] << endl;
    }
    if (polxt[0] == 0.) {
      (*pnux).push_back(0.);
      cubic(polxt, pnux);
    }
    else {
      double e=polxt[2]-3.*polxt[3]*polxt[3]/8.;
      double f=polxt[1]+polxt[3]*polxt[3]*polxt[3]/8.-polxt[2]*polxt[3]/2.;
      double g=polxt[0]-3.*polxt[3]*polxt[3]*polxt[3]*polxt[3]/256.
    +polxt[3]*polxt[3]*polxt[2]/16.-polxt[3]*polxt[1]/4.;
      //cout << "e=" << e << " f=" << f << " g=" << g << endl;
      if (g == 0.) {
    (*pnux).push_back(-polxt[3]/4.);
        vector<double> polxt2(4);
    polxt2[0]=f;
    polxt2[1]=e;
    polxt2[2]=0.;
    polxt2[3]=1.;
    cubic(polxt2, pnux);
    for (size_t i=1; i<(*pnux).size(); ++i)
      (*pnux)[i]-=polxt[3]/4.;
      }
      else if (f == 0.) {
    vector<double> polxt2(3);
    polxt2[0]=g;
    polxt2[1]=e;
    polxt2[2]=1.;
    vector<double> polxt3;
    quadratic(polxt2, &polxt3);
    for (size_t i=0; i<polxt3.size(); ++i) {
      if (polxt3[i]>=0.) {
        (*pnux).push_back(sqrt(polxt3[i]-polxt[3]/4.));
        (*pnux).push_back(-sqrt(polxt3[i]-polxt[3]/4.));
      }
    }
      }
      else { //d,f,g != 0, default case
    vector<double> polxt2(4);
    polxt2[0]=-f*f;
    polxt2[1]=(e*e-4.*g);
    polxt2[2]=2.*e;
    polxt2[3]=1.;
    vector<double> polxt3;
    cubic(polxt2, &polxt3);
    if (polxt3.size()==1 && polxt3[0]<0.) {
      //cout <<"Warning: quartic: single cubic h^2 solution is negative! (unique sol=" << polxt3[0] << ") => complex root h" << endl;
      cubic_single_root_cmplx++;
      return;
    }
    double h;
    for (size_t i=0; i<polxt3.size(); ++i) {
      if (polxt3[i]>0.) h=sqrt(polxt3[i]);
    }
    double j=(e+h*h-f/h)/2.;
    vector<double> polxt4(3);
    polxt4[0]=j;
    polxt4[1]=h;
    polxt4[2]=1.;
    vector<double> polxt5;
    quadratic(polxt4,&polxt5);
    for (size_t i=0; i<polxt5.size(); ++i)
      (*pnux).push_back(polxt5[i]-polxt[3]/4.);
    polxt4[0]=g/j;
    polxt4[1]=-h;
    polxt4[2]=1.;
    vector<double> polxt6;
    quadratic(polxt4,&polxt6);
    for (size_t i=0; i<polxt6.size(); ++i)
      (*pnux).push_back(polxt6[i]-polxt[3]/4.);
      }
    }
  }
  return;

}

void
DileptonAnalyticalSolver::cubic(vector<double> polx,
                                vector<double> *pnux)
{
  if (polx[3] == 0.)
    quadratic(polx, pnux);
  else {
    double q=(polx[2]*polx[2]-3.*polx[1])/9.;
    double r=(2.*polx[2]*polx[2]*polx[2]-9.*polx[1]*polx[2]+27.*polx[0])/54.;
    if (q == 0.) { //single root of degree three
      (*pnux).push_back(-polx[2]/3.);
    }
    else if (q*q*q > r*r) { //3 real roots
      double theta=acos(r/sqrt(q*q*q));
      (*pnux).push_back(-2.*sqrt(q)*cos(theta/3.)-polx[2]/3.);
      (*pnux).push_back(-2.*sqrt(q)*cos((theta+2.*PI)/3.)-polx[2]/3.);
      (*pnux).push_back(-2.*sqrt(q)*cos((theta+4.*PI)/3.)-polx[2]/3.);
    }
    else { // 1 real root
      //double powthrd = pow(sqrt(r*r-q*q*q)+fabs(r),1./3.);
      double radicant = -r+sqrt(r*r-q*q*q);
      double powthrd = pow(fabs(radicant),1./3.);
      //double a = -sign(r)*powthrd;
      double a = sign(radicant)*powthrd;
      double b = (a!=0.) ? q/a : 0.;
      (*pnux).push_back(a+b-polx[2]/3.);
    }
  }
  return;
}

void
DileptonAnalyticalSolver::quadratic(vector<double> polx, vector<double> *pnux)
{
  if (polx[2] == 0.) { //linear equation
    (*pnux).push_back(-polx[0]/polx[1]); // a3*x+a4=0 -> x=-a4/a3
  }
  else if (polx[1]*polx[1]==4.*polx[2]*polx[0]) {
    (*pnux).push_back(-0.5*polx[1]/polx[2]); // single root of degree two
  }
  else if (polx[1]*polx[1]>4.*polx[2]*polx[0]) {
    double q=-0.5*(polx[1]+sign(polx[1])*sqrt(polx[1]*polx[1]-4.*polx[2]*polx[0]));
    (*pnux).push_back(q/polx[2]);
    (*pnux).push_back(polx[0]/q);
  }
  return;
}

int
DileptonAnalyticalSolver::algebraic_pz(double* b,
                                       double* lp,
                                       double mWp,
                                       double mt,
                                       double mb,
                                       double mlp,
                                       double mnu,
                                       double pnux,
                                       double pnuy,
                                       double* pnuz)
{


  double mblp=sqrt(sqr(b[0]+lp[0])-sqr(b[1]+lp[1])-sqr(b[2]+lp[2])-sqr(b[3]+lp[3]));

  // a1=a11+a12*nu[1]+a13*nu[2];
  vector<double> a1(3);
  a1[0]=(sqr(mWp)-sqr(mlp)-sqr(mnu))*lp[3]*0.5 / (sqr(lp[0])-sqr(lp[3]));
  a1[1]=lp[1]*lp[3] / (sqr(lp[0])-sqr(lp[3]));
  a1[2]=lp[2]*lp[3] / (sqr(lp[0])-sqr(lp[3]));

  //a2=a21+a22*nu[1]+a23*nu[2]+a24*nu[1]^2+a25**nu[1]*nu[2]+a26*nu[2]^2
  vector<double> a2(6);
  a2[0]=(quad(mWp)+quad(mlp)+quad(mnu)-2.*sqr(mWp)*sqr(mlp)-2.*sqr(mWp)*sqr(mnu)+2.*sqr(mlp)*sqr(mnu))*0.25 / (sqr(lp[0])-sqr(lp[3]));
  a2[3]=-(sqr(lp[0])-sqr(lp[1])) / (sqr(lp[0])-sqr(lp[3]));
  a2[5]=-(sqr(lp[0])-sqr(lp[2])) / (sqr(lp[0])-sqr(lp[3]));
  a2[1]=(sqr(mWp)-sqr(mlp)-sqr(mnu))*lp[1] / (sqr(lp[0])-sqr(lp[3]));
  a2[2]=(sqr(mWp)-sqr(mlp)-sqr(mnu))*lp[2] / (sqr(lp[0])-sqr(lp[3]));
  a2[4]=2.*lp[1]*lp[2] / (sqr(lp[0])-sqr(lp[3]));


  // b1=b11+b12*nu[1]+b13*nu[2];
  vector<double> b1(3);
  b1[0]=(sqr(mt)-sqr(mblp)-sqr(mnu))*(b[3]+lp[3])*0.5 / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b1[1]=(b[1]+lp[1])*(b[3]+lp[3]) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b1[2]=(b[2]+lp[2])*(b[3]+lp[3]) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));

  //b2=b21+b22*nu[1]+b23*nu[2]+b24*nu[1]^2+b25*nu[2]*nu[1]+b26*nu[2]^2
  vector<double> b2(6);
  b2[0]=(quad(mt)+quad(mblp)+quad(mnu)-2.*sqr(mt)*sqr(mblp)-2.*sqr(mt)*sqr(mnu)+2.*sqr(mblp)*sqr(mnu))*0.25 / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b2[3]=-(sqr(b[0]+lp[0])-sqr(b[1]+lp[1])) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b2[5]=-(sqr(b[0]+lp[0])-sqr(b[2]+lp[2])) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b2[1]=(sqr(mt)-sqr(mblp)-sqr(mnu))*(b[1]+lp[1]) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b2[2]=(sqr(mt)-sqr(mblp)-sqr(mnu))*(b[2]+lp[2]) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));
  b2[4]=2.*(b[1]+lp[1])*(b[2]+lp[2]) / (sqr(b[0]+lp[0])-sqr(b[3]+lp[3]));


  //determine first temporary pnuz, pnubz
  double a1val = evalterm1(&a1, pnux, pnuy);
  double a2val = evalterm2(&a2, pnux, pnuy);
  double b1val = evalterm1(&b1, pnux, pnuy);
  double b2val = evalterm2(&b2, pnux, pnuy);

  vector<double> pnuz_a, pnuz_b;
  double radicant=a1val*a1val+a2val;
  if (radicant>=0.) {
    pnuz_a.push_back(a1val+sqrt(radicant));
    pnuz_a.push_back(a1val-sqrt(radicant));
  }
  else if (fabs(radicant)<epsilon) {
    pnuz_a.push_back(a1val);
  }
  else { //negative radicant => no solution, should never happen
      //    cout << "ttdilepsolve: algebraic_pz: pnuz_a radicant=" << radicant
      //     << " should never happen!" << endl;
  }
  radicant=b1val*b1val+b2val;
  if (radicant>=0.) {
    pnuz_b.push_back(b1val+sqrt(radicant));
    pnuz_b.push_back(b1val-sqrt(radicant));
    //cout << "pnuz_b solutions= " << b1val+sqrt(radicant) << " and " << b1val-sqrt(radicant) << endl;
  }
  else if (fabs(radicant)<epsilon) {
      pnuz_b.push_back(b1val);
  }
  else { //negative radicant => no solution, should never happen
      //    cout << "ttdilepsolve: algebraic_pz: pnuz_b radicant=" << radicant
      //     << " should never happen!" << endl;
  }


  if (pnuz_a.size()==0 || pnuz_b.size()==0) return -1; //error

  double pnuzchi;
  double pnuzchimin=fabs(pnuz_a[0]-pnuz_b[0]);
  int a_min_ind=0, b_min_ind=0;
  for (size_t j=0; j<pnuz_a.size(); ++j) {
    for (size_t k=0; k<pnuz_b.size(); ++k) {
      pnuzchi = fabs(pnuz_a[j]-pnuz_b[k]);
      if (pnuzchi < pnuzchimin) {
    pnuzchimin = pnuzchi;
    a_min_ind=j;
    b_min_ind=k;
      }
    }
  }

  if (pnuzchimin<sqrt(epsilon)) {
    *pnuz = 0.5*(pnuz_a[a_min_ind]+pnuz_b[b_min_ind]);
  }
  else return -1; //error

  return 0; //success

}


double
DileptonAnalyticalSolver::evalterm1(std::vector<double> *a1,
                                    double pnux,
                                    double pnuy)
{
  return (*a1)[0]+(*a1)[1]*pnux+(*a1)[2]*pnuy;
}

double
DileptonAnalyticalSolver::evalterm2(std::vector<double> *a2,
                                    double pnux,
                                    double pnuy)
{
  return (*a2)[0]+(*a2)[1]*pnux+(*a2)[2]*pnuy+(*a2)[3]*pnux*pnux+(*a2)[4]*pnux*pnuy+(*a2)[5]*pnuy*pnuy;
}

} // namespace llsolver
