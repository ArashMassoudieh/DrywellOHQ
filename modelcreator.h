#ifndef MODELCREATOR_H
#define MODELCREATOR_H

#include "propertygenerator.h"

class System;

struct model_parameters
{
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
    PropertyGenerator *property_generator = nullptr;
    enum class _mode {homogeneous, heterogeneous} mode;
};

class ModelCreator
{
public:
    ModelCreator();
    bool Create(model_parameters mp, System *system);

private:
    const double pi = 3.141521;
};

#endif // MODELCREATOR_H
