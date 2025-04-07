#include "modelcreator.h"
#include "System.h"
#include "QString"
#include <QDir>
#include <QDebug>

ModelCreator::ModelCreator()
{

}


bool ModelCreator::Create(ModelParameters mp, System *system)
{
#ifdef _windows
    system->GetQuanTemplate("C:/Projects/OpenHydroQual/resources/main_components.json");
    system->AppendQuanTemplate("C:/Projects/OpenHydroQual/resources/unsaturated_soil.json");
    system->AppendQuanTemplate("C:/Projects/OpenHydroQual/resources/Well.json");
    system->ReadSystemSettingsTemplate("C:/Projects/OpenHydroQual/resources/settings.json");
#elif Arash
    system->GetQuanTemplate("/home/arash/Projects/QAquifolium/resources/main_components.json");
    system->AppendQuanTemplate("/home/arash/Projects/QAquifolium/resources/unsaturated_soil.json");
    system->AppendQuanTemplate("/home/arash/Projects/QAquifolium/resources/Well.json");
    system->ReadSystemSettingsTemplate("/home/arash/Projects/QAquifolium/resources/settings.json");
#elif Arash_home
    system->GetQuanTemplate("/home/arash/Projects/OpenHydroQual/resources/main_components.json");
    system->AppendQuanTemplate("/home/arash/Projects/OpenHydroQual/resources/unsaturated_soil.json");
    system->AppendQuanTemplate("/home/arash/Projects/OpenHydroQual/resources/Well.json");
    system->ReadSystemSettingsTemplate("/home/arash/Projects/QAquifolium/resources/settings.json");
#endif
    system->SetWorkingFolder(mp.Workingfolder().toStdString() + "/" + mp.Outputfolder().toStdString() + "/");
    createFolderIfNotExists(mp.Workingfolder() + "/" + mp.Outputfolder());

    if (mp.Tracer())
    {
        Constituent C;
        C.SetQuantities(system->GetMetaModel(), "Constituent");
        C.SetName("Tracer");
        C.SetType("Constituent");
        system->AddConstituent(C,false);
        //system->AddConstituentRelatePropertiestoMetalModel();
        //system->AddConstituentRelateProperties(system->constituent("Tracer"));
    }

    double dr = (mp.GetValue("RadiousOfInfluence") - mp.GetValue("rw")) / mp.GridSize("r");
    double dz = mp.GetValue("DepthtoGroundWater") / mp.GridSize("z");
    std::cout<<"Main Soil Blocks"<<endl;
    for (int i=0; i<mp.GridSize("r"); i++)
        for (int j=0; j<mp.GridSize("z"); j++)
        {
            Block B;
            B.SetQuantities(system->GetMetaModel(), "Soil");
            double r1 = mp.GetValue("rw") + i * dr;
            double r2 = mp.GetValue("rw") + (i + 1) * dr;
            double area = pi*(r2*r2-r1*r1);

            B.SetName(("Soil (" + QString::number(i+1) + "$" + QString::number(j) + ")").toStdString());
            B.SetType("Soil");
            if (mp.property_generator && mp.GetMode() == Mode::heterogeneous)
            {   B.SetVal("K_sat_original",mp.property_generator->val("K_sat",j));
                B.SetVal("alpha",mp.property_generator->val("alpha",j));
                B.SetVal("n",mp.property_generator->val("n",j));
            }
            else
            {   B.SetVal("K_sat_original",mp.GetValue("K_sat"));
                B.SetVal("alpha",mp.GetValue("alpha"));
                B.SetVal("n",mp.GetValue("n"));
            }

            B.SetVal("area",area);
            B.SetVal("_width",dr*3000);
            B.SetVal("_height",dr*3000);
            B.SetVal("bottom_elevation",-(j+1)*dz);
            B.SetVal("depth",dz);
            B.SetVal("theta",mp.GetValue("initial_theta"));
            B.SetVal("theta_sat",mp.GetValue("theta_sat"));
            B.SetVal("theta_res",mp.GetValue("theta_r"));
            B.SetVal("x",(i*dr+mp.GetValue("rw")) * 4000);
            B.SetVal("y",(j*dz)*4000);
            B.SetVal("act_X",(i+0.5)*dr+mp.GetValue("rw"));
            B.SetVal("act_Y",-(j+0.5)*dz);
            system->AddBlock(B,false);
        }

    std::cout<<"Soil Blocks under well"<<endl;
    for (int j=0; j<mp.GridSize("z"); j++)
    {
        if (j*dz>mp.GetValue("DepthofWell"))
        {   Block B;
            B.SetQuantities(system->GetMetaModel(), "Soil");

            double area = pi*(mp.GetValue("rw") * mp.GetValue("rw"));

            B.SetName(("Soil (" + QString::number(0) + "$" + QString::number(j) + ")").toStdString());
            B.SetType("Soil");
            if (mp.property_generator && mp.GetMode() == Mode::heterogeneous)
            {   B.SetVal("K_sat_original",mp.property_generator->val("K_sat",j));
                B.SetVal("alpha",mp.property_generator->val("alpha",j));
                B.SetVal("n",mp.property_generator->val("n",j));
            }
            else
            {   B.SetVal("K_sat_original",mp.GetValue("K_sat"));
                B.SetVal("alpha",mp.GetValue("alpha"));
                B.SetVal("n",mp.GetValue("n"));
            }
            B.SetVal("area",area);
            B.SetVal("_width",dr*3000);
            B.SetVal("_height",dr*3000);
            B.SetVal("bottom_elevation",-(j+1)*dz);
            B.SetVal("depth",dz);
            B.SetVal("theta",mp.GetValue("initial_theta"));
            B.SetVal("theta_sat",mp.GetValue("theta_sat"));
            B.SetVal("theta_res",mp.GetValue("theta_r"));
            B.SetVal("x",-mp.GetValue("rw") * 4000);
            B.SetVal("y",(j*dz)*4000);
            B.SetVal("act_X",0);
            B.SetVal("act_Y",-(j+0.5)*dz);
            system->AddBlock(B,false);
        }
    }


    std::cout<<"Horizontal links"<<endl;
    for (int i=0; i<mp.GridSize("r") - 1; i++)
        for (int j=0; j<mp.GridSize("z"); j++)
        {
            Link L;
            L.SetQuantities(system->GetMetaModel(), "soil_to_soil_H_link");


            L.SetName(("Soil (" + QString::number(i+1) + "$" + QString::number(j) + "-" + QString::number(i+2) + "$" + QString::number(j)+ ")").toStdString());
            L.SetType("soil_to_soil_H_link");

            L.SetVal("area",2*pi*((i+0.5)*dr+mp.GetValue("rw")));
            L.SetVal("length",dr);

            system->AddLink(L, ("Soil (" + QString::number(i+1) + "$" + QString::number(j) + ")").toStdString(), ("Soil (" + QString::number(i+2) + "$" + QString::number(j) + ")").toStdString(), false);
        }

    std::cout<<"Horizontal links for central soils"<<endl;
    for (int j=0; j<mp.GridSize("z"); j++)
    {

        if (j*dz>mp.GetValue("DepthofWell"))
        {   Link L;
            L.SetQuantities(system->GetMetaModel(), "soil_to_soil_H_link");


            L.SetName(("Soil (" + QString::number(0) + "$" + QString::number(j) + "-" + QString::number(1) + "$" + QString::number(j)+ ")").toStdString());
            L.SetType("soil_to_soil_H_link");

            L.SetVal("area",2*pi*mp.GetValue("rw"));
            L.SetVal("length",dr);

            system->AddLink(L, ("Soil (" + QString::number(0) + "$" + QString::number(j) + ")").toStdString(), ("Soil (" + QString::number(1) + "$" + QString::number(j) + ")").toStdString(), false);
        }
    }
    int well_layer=0;
    std::cout<<"Vertical links for central soils"<<endl;
    for (int j=0; j<mp.GridSize("z") - 1; j++)
    {

        if (j*dz>mp.GetValue("DepthofWell"))
        {   Link L;
            L.SetQuantities(system->GetMetaModel(), "soil_to_soil_link");
            if (well_layer==0) well_layer = j;

            L.SetName(("Soil (" + QString::number(0) + "$" + QString::number(j) + "-" + QString::number(0) + "$" + QString::number(j+1)+ ")").toStdString());
            L.SetType("soil_to_soil_link");

            system->AddLink(L, ("Soil (" + QString::number(0) + "$" + QString::number(j) + ")").toStdString(), ("Soil (" + QString::number(0) + "$" + QString::number(j+1) + ")").toStdString(), false);
        }
    }
    std::cout<<"Vertical links"<<endl;
    for (int i=0; i<mp.GridSize("r"); i++)
        for (int j=0; j<mp.GridSize("z") - 1; j++)
        {
            Link L;
            L.SetQuantities(system->GetMetaModel(), "soil_to_soil_link");


            L.SetName(("Soil (" + QString::number(i+1) + "$" + QString::number(j) + "-" + QString::number(i) + "$" + QString::number(j+1)+ ")").toStdString());
            L.SetType("soil_to_soil_link");

            //L.SetVal("length",dr);

            system->AddLink(L, ("Soil (" + QString::number(i+1) + "$" + QString::number(j) + ")").toStdString(), ("Soil (" + QString::number(i+1) + "$" + QString::number(j+1) + ")").toStdString(), false);
        }



    std::cout<<"Well"<<endl;
    Block well;
    well.SetQuantities(system->GetMetaModel(), "Well_aggregate");
    well.SetName("Well");
    well.SetType("Well_aggregate");
    well.SetVal("_height",mp.GetValue("DepthofWell") * 4000);
    well.SetVal("_width",mp.GetValue("rw") * 4000);
    well.SetVal("bottom_elevation",-mp.GetValue("DepthofWell"));
    well.SetVal("diameter",mp.GetValue("rw") * 2);
    well.SetVal("depth",mp.GetValue("initial_water_depth"));
    well.SetVal("porosity",1);
    well.SetVal("x",-mp.GetValue("rw") * 4000);
    well.SetVal("y",0);
    if (mp.Tracer())
        well.SetVal("Tracer:concentration",mp.GetValue("initial_concentration"));
    if (mp.GetInflowMode() == FlowMode::periodic)
    {
        CTimeSeries<double> inflow = CTimeSeries<double>();
        inflow.CreatePeriodicStepFunction(0, mp.GetValue("t_end"), 0.5, 1.5, 0.5);
        well.Variable("inflow")->SetTimeSeries(inflow);
    }
    
    system->AddBlock(well,false);

    std::cout<<"Well to soil"<<endl;
    for (int j=0; j<mp.GridSize("z"); j++)
    {
        if (j*dz<mp.GetValue("DepthofWell"))
        {   Link L;
            L.SetQuantities(system->GetMetaModel(), "Well2soil horizontal link");
            L.SetName(("Well to Soil (" + QString::number(j) + ")").toStdString());
            L.SetType("Well2soil horizontal link");
            L.SetVal("length",dr/2);

            system->AddLink(L, "Well", ("Soil (" + QString::number(1) + "$" + QString::number(j) + ")").toStdString(), false);
        }
    }

    std::cout<<"Well to bottom"<<endl;
    Link well_to_bottom;
    well_to_bottom.SetQuantities(system->GetMetaModel(), "Well2soil vertical link");
    well_to_bottom.SetName("Well_to_bottom");
    well_to_bottom.SetType("Well2soil vertical link");
    system->AddLink(well_to_bottom,"Well",("Soil (" + QString::number(0) + "$" + QString::number(well_layer) + ")").toStdString(), false);

    std::cout<<"Groundwater"<<endl;
    Block gw;
    gw.SetQuantities(system->GetMetaModel(), "fixed_head");
    gw.SetName("Groundwater");
    gw.SetType("fixed_head");
    gw.SetVal("_height",200);
    gw.SetVal("_width",mp.GetValue("RadiousOfInfluence") * 4000);
    gw.SetVal("head",-mp.GetValue("DepthtoGroundWater"));
    gw.SetVal("Storage",100000);
    gw.SetVal("x",0);
    gw.SetVal("y",mp.GetValue("DepthtoGroundWater") * 4000 + 400);
    system->AddBlock(gw,false);

    cout<<"Soil to Groundwater"<<endl;
    for (int i=0; i<mp.GridSize("r") + 1; i++)
    {
        Link L;
        L.SetQuantities(system->GetMetaModel(), "soil_to_fixedhead_link");
        L.SetName(("Soil to Groundwater (" + QString::number(i) + ")").toStdString());
        L.SetType("soil_to_fixedhead_link");

        system->AddLink(L, ("Soil (" + QString::number(i) + "$" + QString::number(mp.GridSize("z") - 1) + ")").toStdString(), "Groundwater", false);

    }

    cout<<"Populate functions"<<endl;
    system->PopulateOperatorsFunctions();
    cout<<"Variable parents"<<endl;
    system->SetVariableParents();
    return true;
}

void createFolderIfNotExists(const QString& folderPath) {
    QDir dir(folderPath);
    if (!dir.exists()) {
        if (dir.mkpath(".")) {
            qDebug() << "Folder created successfully:" << folderPath;
        }
        else {
            qDebug() << "Failed to create folder:" << folderPath;
        }
    }
    else {
        qDebug() << "Folder already exists:" << folderPath;
    }
}
