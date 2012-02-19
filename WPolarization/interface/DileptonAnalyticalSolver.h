/**
    @file DileptonAnalyticalSolver.h

    @author Lars Sonnenschein <Lars.Sonnenschein@cern.ch>

    @brief Solve the kinematics of \f$t\bar{t}\f$ dilepton events analytically.
    Based on the two following publications:

    - Lars Sonnenschein, <i>Algebraic approach to solve ttbar dilepton
    equations</i>,
    <a href = "http://prd.aps.org/abstract/PRD/v72/i9/e095020">
    Phys. Rev. <b>D72</b>, 095020 (2005)
    </a>,
    <a href="http://arxiv.org/abs/hep-ph/0510100">arXiv:hep-ph/0510100</a>.

    - Lars Sonnenschein, <i>Analytical solution of ttbar dilepton
    equations</i>,
    <a href = "http://prd.aps.org/abstract/PRD/v73/i5/e054015">
    Phys. Rev. <b>D73</b>, 054015 (2006)
    </a>,
    <a href="http://arxiv.org/abs/hep-ph/0603011">arXiv:hep-ph/0603011</a>.

    @par Adapted to CMSSW by Haryo Sumowidagdo <Suharyo.Sumowidagdo@cern.ch>

    @date Tue Aug 23 12:51:04 CEST 2011

    @version $Id: DileptonAnalyticalSolver.h,v 1.1 2011/08/23 12:34:26 haryo Exp $

 */


#ifndef DileptonAnalyticalSolver_h
#define DileptonAnalyticalSolver_h


#include <vector>
#include <cmath>


namespace llsolver {

using std::vector;
typedef int size_t;
const double PI      = acos(-1.0);

/**
    @class DileptonAnalyticalSolver

    @brief Solve the kinematics of \f$t\bar{t}\f$ dilepton events analytically.
    Based on the two following publications:

    - Lars Sonnenschein, <i>Algebraic approach to solve ttbar dilepton
    equations</i>,
    <a href = "http://prd.aps.org/abstract/PRD/v72/i9/e095020">
    Phys. Rev. <b>D72</b>, 095020 (2005)
    </a>,
    <a href="http://arxiv.org/abs/hep-ph/0510100">arXiv:hep-ph/0510100</a>.

    - Lars Sonnenschein, <i>Analytical solution of ttbar dilepton
    equations</i>,
    <a href = "http://prd.aps.org/abstract/PRD/v73/i5/e054015">
    Phys. Rev. <b>D73</b>, 054015 (2006)
    </a>,
    <a href="http://arxiv.org/abs/hep-ph/0603011">arXiv:hep-ph/0603011</a>.

    @author Lars Sonnenschein <Lars.Sonnenschein@cern.ch>

    @par Adapted to CMSSW by Haryo Sumowidagdo <Suharyo.Sumowidagdo@cern.ch>

    @date Tue Aug 23 12:51:04 CEST 2011

    @version $Id: DileptonAnalyticalSolver.h,v 1.1 2011/08/23 12:34:26 haryo Exp $

 */
class DileptonAnalyticalSolver {

public:

    /**
        @brief Constructor
     */
    DileptonAnalyticalSolver()
        : epsilon(1.e-6)
    {
    }

    /**
        @brief Constructor with a different value of epsilon.
     */
    DileptonAnalyticalSolver(double x)
        : epsilon(x)
    {
    }

    /**
        @brief Destructor.
     */
    ~DileptonAnalyticalSolver()
    {
    };

    /**
        @brief 

        @param ETmiss

        @param b

        @param bb

        @param lp

        @param lm

        @param mWp

        @param mWm

        @param mt

        @param mtb

        @param mnu

        @param mnub

     */
    void solve(double* ETmiss,
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
               int& cubic_single_root_cmplx);

    /**
        @brief 

        @param
     */
    void quartic(vector<double> polx,
                 vector<double> *pnux,
                 int& cubic_single_root_cmplx);

    /**
        @brief 

        @param
     */
    void cubic(vector<double> polx,
               vector<double> *pnux);

    /**
        @brief 

        @param
     */
    void quadratic(vector<double> polx,
                   vector<double> *pnux);

    /**
        @brief 

        @param
     */
    int algebraic_pz(double* b,
                     double* lp,
                     double mWp,
                     double mt,
                     double mb,
                     double mlp,
                     double mnu,
                     double pnux,
                     double pnuy,
                     double* pnuz);

    /**
        @brief 

        @param
     */
    double evalterm1(std::vector<double> *a1,
                     double pnux,
                     double pnuy);

    /**
        @brief 

        @param
     */
    double evalterm2(std::vector<double> *a2,
                     double pnux,
                     double pnuy);

    /**
        @brief 

        @param
     */
    inline double sqr(double x)
    {
        return x*x;
    }

    /**
        @brief 

        @param
     */
    inline double sign(double a)
    {
        return (a < 0) ? -1 : (a > 0) ? 1 : 0;
    }

    /**
        @brief 

        @param
     */
    inline double quad(double x)
    {
        return x*x*x*x;
    }


private:

    /**
        @brief 

        @param
     */
    const double epsilon;

}; // class DileptonAnalyticalSolver


} // namespace llsolver

#endif
