#ifndef MODELCREATOR_H
#define MODELCREATOR_H

#include "propertygenerator.h"
#include "ModelParameters.h"

class System;


class ModelCreator
{
public:
    ModelCreator();
    bool Create(ModelParameters mp, System *system);

private:
    const double pi = 3.141521;
};

#endif // MODELCREATOR_H
