#include "System.h"
#include "Script.h"
#include "qfileinfo.h"
#include "modelcreator.h"
#include "resultgrid.h"
#include "vtk.h"


int main(int argc, char *argv[])
{

    model_parameters mp;
    mp.nr = 20;
    mp.nz = 20;
    mp.K_sat = 1;
    mp.alpha = 20;
    mp.n = 1.8;
    mp.rw = 0.03;
    mp.theta_sat = 0.4;
    mp.theta_r = 0.05;
    mp.initial_theta = 0.2;

    System *system=new System();
    ModelCreator ModCreate;
    cout<<"Creating model ..." <<endl;
    ModCreate.Create(mp,system);
    cout<<"Creating model done..." <<endl;

    string defaulttemppath = "/home/arash/Projects/QAquifolium/resources/";
    cout << "Default Template path = " + defaulttemppath +"\n";
    system->SetDefaultTemplatePath(defaulttemppath);
    system->SetWorkingFolder("/home/arash/Projects/DryWellModels/");
    string settingfilename = "/home/arash/Projects/QAquifolium/resources/settings.json";
    system->SetSilent(false);
    cout<<"Saving"<<endl;
    system->SavetoScriptFile("/home/arash/Projects/DryWellModels/CreatedModel.ohq");

    cout<<"Solving ..."<<endl;
    system->SetProp("tend",10);
    system->SetProp("initial_time_step",0.1);
    system->Solve();
    cout<<"Writing outputs in '"<< system->GetWorkingFolder() + system->OutputFileName() +"'"<<endl;
    CTimeSeriesSet<double> output = system->GetOutputs();
    output.writetofile(system->GetWorkingFolder() + system->OutputFileName());
    cout<<"Getting results into grid"<<endl;
    ResultGrid resgrid(output,"theta",system);
    cout<<"Writing VTPs"<<endl;
    resgrid.WriteToVTP("Moisture_content",system->GetWorkingFolder()+"moisture.vtp");

    vector<string> well_block; well_block.push_back("Well");
    ResultGrid well_depth = ResultGrid(output,well_block,"depth");
    well_depth.Sum().writefile(system->GetWorkingFolder()+"WaterDepth.csv");

    vector<string> GWRechargeBlocks;
    for (int i=0; i<mp.nr+1; i++)
    {
        GWRechargeBlocks.push_back(("Soil to Groundwater (" + QString::number(i) + ")").toStdString());
    }
    ResultGrid GW_recharge = ResultGrid(output,GWRechargeBlocks,"flow");
    GW_recharge.Sum().writefile(system->GetWorkingFolder() + "GW_recharge.csv");
    return 0;

}
