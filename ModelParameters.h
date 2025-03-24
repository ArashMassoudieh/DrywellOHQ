#ifndef MODELPARAMETERS_H
#define MODELPARAMETERS_H

#include <QString>
#include <unordered_map>
#include "propertygenerator.h"
#include "BTC.h"


enum class Mode { homogeneous, heterogeneous };
enum class FlowMode {custom, no_flow, periodic };
class ModelParameters {
public:
    // Default Constructor
    ModelParameters();

    // Copy Constructor
    ModelParameters(const ModelParameters& other);

    // Assignment Operator
    ModelParameters& operator=(const ModelParameters& other);

    // Destructor
    ~ModelParameters();

    // Setter method to update parameters dynamically
    bool SetValue(const QString& parameterName, const double& value);
    double GetValue(const QString& parameterName) const;
    void SetTracer(bool val) { tracer = val; }
    void SetWorkingfolder(const QString & val) { workingfolder = val; }
    void SetOutputfolder(const QString& val) { outputFolder = val; }
    QString Outputfolder() { return outputFolder; }
    void SetMode(Mode val) { mode = val; }
    void SetInflowMode(FlowMode val) { flowmode = val; }
    bool SetMarginalCDF(const QString& parameterName, const QString& filePath);
    bool Tracer() { return tracer; }
    QString Workingfolder() { return workingfolder; }
    Mode GetMode() { return mode; }
    FlowMode GetInflowMode() { return flowmode; }
    CTimeSeries<double> MarginalCDF(const QString& parameterName)
    {
        if (parameterName == "K_sat") {
            return K_sat_marginal_CDF;
        }
        else if (parameterName == "alpha") {
            return alpha_marginal_CDF;
        }
        else if (parameterName == "n") {
            return n_marginal_CDF;
        }
        else
            return CTimeSeries<double>(); 
    }
    void SetGridSize(const int& _nr, const int& _nz) { nr = _nr; nz = _nz; }
    int GridSize(const QString& direction)
    {
        if (direction == "r")
            return nr;
        else if (direction == "z")
            return nz;
        else
            return -1; 
    }
    PropertyGenerator* property_generator = nullptr;
private:
    double DepthtoGroundWater = 2;
    double DepthofWell = 1;
    double RadiousOfInfluence = 1.5;
    int nr = 10;
    int nz = 10;
    double K_sat;
    double alpha;
    double n;
    double rw;
    double theta_sat;
    double theta_r;
    double initial_theta;
    double Correlation_Length_Scale;
    double alpha_Ksat_Cor;
    double n_Ksat_Cor;
    double K_sat_stdev;
    double inflow_duration; 
    double inflow_gap; 
    double inflow_magnitude; 
    
    Mode mode; 
    FlowMode flowmode; 
    bool tracer = false;
    double initial_concentration = 1;
    double initial_water_depth = 1;
    double t_end = 10;
    double dt0 = 0.01;
    QString workingfolder;
    CTimeSeries<double> K_sat_marginal_CDF;
    CTimeSeries<double> alpha_marginal_CDF;
    CTimeSeries<double> n_marginal_CDF;
    QString outputFolder;
};

#endif // MODELPARAMETERS_H


