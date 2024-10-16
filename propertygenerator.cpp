#include "propertygenerator.h"
#include "gsl/gsl_randist.h"
#include "gsl/gsl_cdf.h"
#ifndef  _WINDOWS
#include <sys/time.h>
#endif // ! _WINDOWS


PropertyGenerator::PropertyGenerator(int seed):std::vector<Propfull>()
{
#ifndef _WINDOWS
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec+seed;
#else
    long int mySeed = static_cast<long int>(time(NULL)+seed);
#endif // !_WINDOWS
gsl_rng_set(r, mySeed);
}


PropertyGenerator::PropertyGenerator(unsigned int n,int seed):std::vector<Propfull>(n)
{
#ifndef _WINDOWS
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec+seed;
#else
    long int mySeed = static_cast<long int>(time(NULL)+seed);
#endif // !_WINDOWS
    gsl_rng_set(r, mySeed);

}


correl_mat_vec PropertyGenerator::get_correll_matrix_vec(int i)
{
    correl_mat_vec Correl_Matrix_Vector;
    int num_determined = GetNumberOfPointsDetermined();
    std::vector<int> determined = Determined();
#ifdef _arma
    Correl_Matrix_Vector.M_22 = CMatrix_arma(num_determined);
    Correl_Matrix_Vector.V_21 = CVector_arma(num_determined);
    Correl_Matrix_Vector.V_RHS = CVector_arma(num_determined);
#else
    Correl_Matrix_Vector.M_22 = CMatrix(num_determined);
    Correl_Matrix_Vector.V_21 = CVector(num_determined);
    Correl_Matrix_Vector.V_RHS = CVector(num_determined);
#endif //  arma
    for (int ii = 0; ii < num_determined; ii++)
    {
        Correl_Matrix_Vector.V_21[ii] = exp(-fabs(determined[ii]-i)*dx/correlation_length_scale);
        Correl_Matrix_Vector.V_RHS[ii] = at(determined[ii]).normal_scores.K_sat;
        for (int jj = 0; jj < num_determined; jj++)
        {
#ifdef _arma
            Correl_Matrix_Vector.M_22(ii,jj) = exp(-fabs(determined[ii]-determined[jj])*dx/correlation_length_scale);
#else
            Correl_Matrix_Vector.M_22[ii][jj] = exp(-fabs(determined[ii]-determined[jj])*dx/correlation_length_scale);
#endif // arma
        }
    }
    return Correl_Matrix_Vector;
}

void PropertyGenerator::assign_K_gauss(unsigned int i)
{
    correl_mat_vec M = get_correll_matrix_vec(i);
    double mu;
    double sigma;

#ifdef _arma
        CMatrix_arma M_inv = inv(M.M_22);
#else
        CMatrix M_inv = Invert(M.M_22);
#endif
        CVector_arma prod = M_inv*M.V_21;
        mu = dotproduct(prod, M.V_RHS);
        sigma = 1.0 - dotproduct(prod, M.V_21);

        double u = gsl_ran_ugaussian(r);
    double K_gauss = mu + u*sigma;
    at(i).k_det = true;
    at(i).normal_scores.K_sat = K_gauss;

}


void PropertyGenerator::assign_K_gauss()
{
    unsigned int n_filled = 0;
    srand(time(NULL));
    int i = gsl_rng_uniform(r)*(size()-1) + 0.5;
    at(i).normal_scores.K_sat = gsl_ran_ugaussian(r);
    at(i).k_det = true;
    n_filled++;
    while (n_filled<size())
    {
        i = gsl_rng_uniform(r)*(size()-1) + 0.5;
        if (!at(i).k_det)
        {
            assign_K_gauss(i);
            n_filled++;
        }
    }

}


unsigned int PropertyGenerator::GetNumberOfPointsDetermined()
{
    int num_determined = 0;
    for (unsigned int i=0; i<size(); i++)
        if (at(i).k_det)
            num_determined++;
    return num_determined;
}

std::vector<int> PropertyGenerator::Determined()
{
    std::vector<int> determined;
    for (unsigned int i=0; i<size(); i++)
        if (at(i).k_det)
            determined.push_back(i);
    return determined;
}


void PropertyGenerator::Normalize_Ksat_normal_scores(const double &new_mean, const double &new_std)
{
    double m = mean("K_sat_normal_score");
    double s= std("K_sat_normal_score");
    for (unsigned int i=0; i<size(); i++)
    {
        at(i).normal_scores.K_sat = (at(i).normal_scores.K_sat - m)/s*new_std + new_mean;
    }
}

double PropertyGenerator::mean(const std::string &quan, bool log) const
{
    CVector V(vals(quan));
    if (!log)
        return V.mean();
    else
        return exp(V.Log().mean());
}

double PropertyGenerator::std(const std::string &quan, bool log) const
{
    CVector V(vals(quan));
    if (!log)
        return V.stdev();
    else
        return V.Log().stdev();
}

void PropertyGenerator::Normalize(const std::string &quan,const double &denominator)
{
    for (int i=0; i<size(); i++)
    {
        SetVal(quan,i,val(quan,i)/denominator);
    }
}

bool PropertyGenerator::write(const std::string &quan, const std::string &filename) const
{
    CVector V(vals(quan));
    V.writetofile(filename);
    return true;
}


std::vector<double> PropertyGenerator::vals(const std::string &quan) const
{
    std::vector<double> out;
    for (unsigned int i=0; i<size(); i++)
    {
        if (quan == "K_sat_normal_score")
            out.push_back(at(i).normal_scores.K_sat);
        else if (quan == "alpha_normal_score")
            out.push_back(at(i).normal_scores.alpha);
        else if (quan == "n_normal_score")
            out.push_back(at(i).normal_scores.n);
        if (quan == "K_sat")
            out.push_back(at(i).realvalues.K_sat);
        else if (quan == "alpha")
            out.push_back(at(i).realvalues.alpha);
        else if (quan == "n")
            out.push_back(at(i).realvalues.n);

    }

    return out;
}

double PropertyGenerator::val(const std::string &quan, int i) const
{

    if (quan == "K_sat_normal_score")
        return at(i).normal_scores.K_sat;
    else if (quan == "alpha_normal_score")
        return at(i).normal_scores.alpha;
    else if (quan == "n_normal_score")
        return at(i).normal_scores.n;
    if (quan == "K_sat")
        return at(i).realvalues.K_sat;
    else if (quan == "alpha")
        return at(i).realvalues.alpha;
    else if (quan == "n")
        return at(i).realvalues.n;
    else
        return -999;


}

bool PropertyGenerator::SetVal(const std::string &quan, int i, const double &value)
{

    if (quan == "K_sat_normal_score")
        at(i).normal_scores.K_sat = value;
    else if (quan == "alpha_normal_score")
        at(i).normal_scores.alpha = value;
    else if (quan == "n_normal_score")
        at(i).normal_scores.n = value;
    if (quan == "K_sat")
        at(i).realvalues.K_sat = value;
    else if (quan == "alpha")
        at(i).realvalues.alpha = value;
    else if (quan == "n")
        at(i).realvalues.n = value;
    else
        return false;

    return true;


}

void PropertyGenerator::SetMarginalDistribution(const std::string &quan, const CTimeSeries<double> series)
{
    marginal_distributions[quan] = series;
}
CTimeSeries<double> PropertyGenerator::MarginalDistribution(const std::string &quan)
{
    if (marginal_distributions.count(quan)==1)
    {
        return marginal_distributions[quan];
    }
    else
        return CTimeSeries<double>();
}

void PropertyGenerator::PopulateRealValue(const std::string &quan, const std::string &quanfrom)
{
    //cout<<"PopulateRealValue-1"<<std::endl;
    CTimeSeries<double> LogTransformed = marginal_distributions[quan].LogTransformX();
    //cout<<"LogTransformed"<<std::endl;
    CTimeSeries<double> inverseCDF = LogTransformed.inverse_cumulative_uniform(100);
    //cout<<"PopulateRealValue-2"<<std::endl;
    for (unsigned int i=0; i<size(); i++)
    {
        //cout<<i<<std::endl;
        double score = gsl_cdf_ugaussian_P(val(quanfrom,i));
        double value = exp(inverseCDF.interpol(score));
        SetVal(quan,i,value);
    }
}

void PropertyGenerator::SetCorr(params p, const double &value)
{
    if (p==params::alpha)
        K_sat_alpha_correlation = value;
    else if (p==params::n)
        K_sat_n_correlation = value;
}

void PropertyGenerator::Populate_Alpha_n_normal_scores(params p)
{
    for (unsigned int i=0; i<size(); i++)
    {
        if (p == params::alpha)
            at(i).normal_scores.alpha = K_sat_alpha_correlation + gsl_ran_ugaussian(r)*sqrt(1-pow(K_sat_alpha_correlation,2));
        if (p== params::n)
            at(i).normal_scores.n = K_sat_n_correlation + gsl_ran_ugaussian(r)*sqrt(1-pow(K_sat_n_correlation,2));
    }
}
