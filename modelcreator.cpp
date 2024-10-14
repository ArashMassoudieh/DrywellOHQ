#include "modelcreator.h"
#include "System.h"
#include "QString"

ModelCreator::ModelCreator()
{

}


bool ModelCreator::Create(model_parameters mp, System *system)
{
    system->AppendQuanTemplate("/home/arash/Projects/QAquifolium/resources/unsaturated_soil.json");
    system->AppendQuanTemplate("/home/arash/Projects/QAquifolium/resources/Well.json");
    system->ReadSystemSettingsTemplate("/home/arash/Projects/QAquifolium/resources/settings.json");
    double dr = (mp.RadiousOfInfluence-mp.rw)/mp.nr;
    double dz = mp.DepthtoGroundWater/mp.nz;
    for (int i=0; i<mp.nr; i++)
        for (int j=0; j<mp.nz; j++)
        {
            Block B;
            B.SetQuantities(system->GetMetaModel(), "Soil");
            double r1 = mp.rw + i*dr;
            double r2 = mp.rw + (i+1)*dr;
            double area = pi*(r2*r2-r1*r1);

            B.SetName(("Soil (" + QString::number(i) + "$" + QString::number(j) + ")").toStdString());
            B.SetType("Soil");
            B.SetVal("K_sat_original",mp.K_sat);
            B.SetVal("alpha",mp.alpha);
            B.SetVal("area",area);
            B.SetVal("bottom_elevation",-(j+1)*dz);
            B.SetVal("depth",dz);
            B.SetVal("theta",mp.initial_theta);
            B.SetVal("theta_sat",mp.theta_sat);
            B.SetVal("theta_res",mp.theta_r);
            B.SetVal("x",(i+0.5)*dr+mp.rw);
            B.SetVal("y",-(j+0.5)*dz);

            system->AddBlock(B,false);


        }

    system->SavetoScriptFile("/home/arash/Projects/DryWellModels/CreatedModel.ohq");
    return true;
}
