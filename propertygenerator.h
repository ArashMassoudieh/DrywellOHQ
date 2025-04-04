#ifndef PROPERTYGENERATOR_H
#define PROPERTYGENERATOR_H

#include <vector>
#include <Matrix.h>
#include <Vector.h>
#include <gsl/gsl_rng.h>
#include "BTC.h"
#include <QString> 


//using namespace std;

class ModelParameters; 

struct correl_mat_vec
{
#ifdef  _arma
    CMatrix_arma M_22;
    CVector_arma V_21;
    CVector_arma V_RHS;
#else
    CMatrix M_22;
    CVector V_21;
    CVector V_RHS;
#endif //  arma
};

struct ival
{
    int i;
    double val;
};

struct Prop
{
    double K_sat;
    double alpha;
    double n;
};

struct Propfull
{
    Prop realvalues;
    Prop normal_scores;
    bool k_det = false;
};

enum class params {K_sat, alpha, n};

class PropertyGenerator: public std::vector<Propfull>
{
public:
    PropertyGenerator(int seed=0);
    PropertyGenerator(unsigned int n, int seed);
    double dx;
    double correlation_length_scale;
    void assign_K_gauss();
    bool write(const std::string &quan, const std::string &filename) const;
    void Normalize_Ksat_normal_scores(const double &mean, const double &std);
    double mean(const std::string &quan, bool log=false) const;
    double std(const std::string &quan, bool log=false) const;
    std::vector<double> vals(const std::string &quan) const;
    void SetMarginalDistribution(const std::string &quan, const CTimeSeries<double> series);
    CTimeSeries<double> MarginalDistribution(const std::string &quan);
    void PopulateRealValue(const std::string &quan, const std::string &quanfrom);
    double val(const std::string &quan, int i) const;
    bool SetVal(const std::string &quan, int i, const double &value);
    void SetCorr(params, const double &value);
    void Populate_Alpha_n_normal_scores(params p);
    void Normalize(const std::string &quan, const double &denominator);
    bool Generate(ModelParameters* mp);
    void SetWorkingFolder(const string& path) { working_folder = path; }
private:
    CMatrix K_alpha_n_corr_matrix;
    double K_sat_normal_score_mean;
    double K_sat_normal_score_std;
    correl_mat_vec get_correll_matrix_vec(int i);
    std::vector<ival> get_top_n(const std::vector<ival> &vec);
    std::vector<ival> get_closest_K_dets(unsigned int i);
    void assign_K_gauss(unsigned int i);
    unsigned int GetNumberOfPointsDetermined();
    std::vector<int> Determined();
    std::map<std::string,CTimeSeries<double>> marginal_distributions;
    double K_sat_alpha_correlation = 1;
    double K_sat_n_correlation = 1;
    const gsl_rng_type * T;
    gsl_rng * r = gsl_rng_alloc (gsl_rng_taus);
    string working_folder;

};

#endif // PROPERTYGENERATOR_H
