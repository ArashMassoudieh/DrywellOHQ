#include "modelcreator.h"
#include "System.h"
#include "QString"

ModelCreator::ModelCreator()
{

}


bool ModelCreator::Create(model_parameters mp, System *system)
{
    system->GetQuanTemplate("/home/arash/Projects/QAquifolium/resources/main_components.json");
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
            B.SetVal("x",((i+0.5)*dr+mp.rw)*300);
            B.SetVal("y",-((j+0.5)*dz)*300);

            system->AddBlock(B,false);
        }

    for (int i=0; i<mp.nr-1; i++)
        for (int j=0; j<mp.nz; j++)
        {
            Link L;
            L.SetQuantities(system->GetMetaModel(), "soil_to_soil_H_link");


            L.SetName(("Soil (" + QString::number(i) + "$" + QString::number(j) + "-" + QString::number(i+1) + "$" + QString::number(j)+ ")").toStdString());
            L.SetType("soil_to_soil_H_link");

            L.SetVal("area",2*pi*((i+0.5)*dr+mp.rw));
            L.SetVal("length",dr);

            system->AddLink(L, ("Soil (" + QString::number(i) + "$" + QString::number(j) + ")").toStdString(), ("Soil (" + QString::number(i+1) + "$" + QString::number(j) + ")").toStdString(), false);
        }


    for (int i=0; i<mp.nr; i++)
        for (int j=0; j<mp.nz-1; j++)
        {
            Link L;
            L.SetQuantities(system->GetMetaModel(), "soil_to_soil_link");


            L.SetName(("Soil (" + QString::number(i) + "$" + QString::number(j) + "-" + QString::number(i) + "$" + QString::number(j+1)+ ")").toStdString());
            L.SetType("soil_to_soil_link");

            //L.SetVal("length",dr);

            system->AddLink(L, ("Soil (" + QString::number(i) + "$" + QString::number(j) + ")").toStdString(), ("Soil (" + QString::number(i) + "$" + QString::number(j+1) + ")").toStdString(), false);
        }
    system->PopulateOperatorsFunctions();
    system->SetVariableParents();
    return true;
}
