#include "ModelParameters.h"
#include <QMap>

// Default Constructor
ModelParameters::ModelParameters() = default;

// Copy Constructor
ModelParameters::ModelParameters(const ModelParameters& other)
    : DepthtoGroundWater(other.DepthtoGroundWater),
    DepthofWell(other.DepthofWell),
    RadiousOfInfluence(other.RadiousOfInfluence),
    nr(other.nr),
    nz(other.nz),
    K_sat(other.K_sat),
    alpha(other.alpha),
    n(other.n),
    rw(other.rw),
    theta_sat(other.theta_sat),
    theta_r(other.theta_r),
    initial_theta(other.initial_theta),
    Correlation_Length_Scale(other.Correlation_Length_Scale),
    alpha_Ksat_Cor(other.alpha_Ksat_Cor),
    n_Ksat_Cor(other.n_Ksat_Cor),
    K_sat_stdev(other.K_sat_stdev),
    mode(other.mode),
    tracer(other.tracer),
    initial_concentration(other.initial_concentration),
    initial_water_depth(other.initial_water_depth),
    t_end(other.t_end),
    dt0(other.dt0),
    workingfolder(other.workingfolder),
    K_sat_marginal_CDF(other.K_sat_marginal_CDF),
    alpha_marginal_CDF(other.alpha_marginal_CDF),
    n_marginal_CDF(other.n_marginal_CDF)
{
    if (other.property_generator) {
        property_generator = new PropertyGenerator(*other.property_generator);
    }
}

// Assignment Operator
ModelParameters& ModelParameters::operator=(const ModelParameters& other) {
    if (this == &other) return *this; // Handle self-assignment

    // Copy values
    DepthtoGroundWater = other.DepthtoGroundWater;
    DepthofWell = other.DepthofWell;
    RadiousOfInfluence = other.RadiousOfInfluence;
    nr = other.nr;
    nz = other.nz;
    K_sat = other.K_sat;
    alpha = other.alpha;
    n = other.n;
    rw = other.rw;
    theta_sat = other.theta_sat;
    theta_r = other.theta_r;
    initial_theta = other.initial_theta;
    Correlation_Length_Scale = other.Correlation_Length_Scale;
    alpha_Ksat_Cor = other.alpha_Ksat_Cor;
    n_Ksat_Cor = other.n_Ksat_Cor;
    K_sat_stdev = other.K_sat_stdev;
    mode = other.mode;
    tracer = other.tracer;
    initial_concentration = other.initial_concentration;
    initial_water_depth = other.initial_water_depth;
    t_end = other.t_end;
    dt0 = other.dt0;
    workingfolder = other.workingfolder;
    K_sat_marginal_CDF = other.K_sat_marginal_CDF;
    alpha_marginal_CDF = other.alpha_marginal_CDF;
    n_marginal_CDF = other.n_marginal_CDF;

    // Deep copy of property_generator
    if (property_generator) {
        delete property_generator;
    }
    property_generator = other.property_generator ? new PropertyGenerator(*other.property_generator) : nullptr;

    return *this;
}

// Destructor
ModelParameters::~ModelParameters() {
    delete property_generator;
}

// Setter method for dynamically updating parameters
bool ModelParameters::SetValue(const QString& parameterName, const double& value) {
    static const QMap<QString, double*> parameterMap = {
        {"DepthtoGroundWater", &DepthtoGroundWater},
        {"DepthofWell", &DepthofWell},
        {"RadiousOfInfluence", &RadiousOfInfluence},
        {"K_sat", &K_sat},
        {"alpha", &alpha},
        {"n", &n},
        {"rw", &rw},
        {"theta_sat", &theta_sat},
        {"theta_r", &theta_r},
        {"initial_theta", &initial_theta},
        {"Correlation_Length_Scale", &Correlation_Length_Scale},
        {"alpha_Ksat_Cor", &alpha_Ksat_Cor},
        {"n_Ksat_Cor", &n_Ksat_Cor},
        {"K_sat_stdev", &K_sat_stdev},
        {"initial_concentration", &initial_concentration},
        {"initial_water_depth", &initial_water_depth},
        {"t_end", &t_end},
        {"dt0", &dt0}
    };

    if (parameterMap.contains(parameterName)) {
        *(parameterMap[parameterName]) = value;
        return true;
    }
    else {
        return false; // Parameter not found
    }
}

double ModelParameters::GetValue(const QString& parameterName) const {
    static const QMap<QString, const double*> parameterMap = {
        {"DepthtoGroundWater", &DepthtoGroundWater},
        {"DepthofWell", &DepthofWell},
        {"RadiousOfInfluence", &RadiousOfInfluence},
        {"K_sat", &K_sat},
        {"alpha", &alpha},
        {"n", &n},
        {"rw", &rw},
        {"theta_sat", &theta_sat},
        {"theta_r", &theta_r},
        {"initial_theta", &initial_theta},
        {"Correlation_Length_Scale", &Correlation_Length_Scale},
        {"alpha_Ksat_Cor", &alpha_Ksat_Cor},
        {"n_Ksat_Cor", &n_Ksat_Cor},
        {"K_sat_stdev", &K_sat_stdev},
        {"initial_concentration", &initial_concentration},
        {"initial_water_depth", &initial_water_depth},
        {"t_end", &t_end},
        {"dt0", &dt0}
    };

    if (parameterMap.contains(parameterName)) {
        return *(parameterMap[parameterName]);
    }
    else {
        throw std::invalid_argument("Invalid parameter name: " + parameterName.toStdString());
    }
}

bool ModelParameters::SetMarginalCDF(const QString& parameterName, const QString& filePath) {
    if (parameterName == "K_sat") {
        return K_sat_marginal_CDF.readfile(filePath.toStdString());
    }
    else if (parameterName == "alpha") {
        return alpha_marginal_CDF.readfile(filePath.toStdString());
    }
    else if (parameterName == "n") {
        return n_marginal_CDF.readfile(filePath.toStdString());
    }
    else {
        return false; // Invalid parameter name
    }
}