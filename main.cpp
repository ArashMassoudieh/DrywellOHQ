#include "System.h"
#include "Script.h"
#include "qfileinfo.h"
#include "modelcreator.h"
#include "resultgrid.h"
#include "vtk.h"
#include <QTime>
#include "ModelParameters.h"

int main(int argc, char *argv[])
{
    QTime start_time = QTime::currentTime();
    cout<<"Start time = " + start_time.toString().toStdString()<<endl;
    ModelParameters mp;
    mp.SetGridSize(20, 20);
    
    mp.SetValue("K_sat", 1);
    mp.SetValue("alpha", 20);
    mp.SetValue("n", 1.8);
    mp.SetValue("rw" , 0.03);
    mp.SetValue("theta_sat" , 0.4);
    mp.SetValue("theta_r" , 0.05);
    mp.SetValue("initial_theta" , 0.2);
    mp.SetMode (Mode::heterogeneous);
    mp.SetInflowMode(FlowMode::no_flow);
    mp.SetValue("Correlation_Length_Scale", 1);
    mp.SetValue("initial_water_depth", 1);
    mp.SetValue("alpha_Ksat_Cor", 0.378);
    mp.SetValue("K_sat_stdev" , 0.5);
    mp.SetValue("n_Ksat_Cor" , 0.2);
    mp.SetTracer(true);
    mp.SetValue("t_end" , 1);
    mp.SetValue("dt0" , 0.01);
    mp.SetWorkingfolder("/home/arash/Projects/DryWellModels/Heterogeneous");
    mp.SetMarginalCDF("K_sat", mp.Workingfolder() + "/K_sat_Marginal_Distribution.csv");
    mp.SetMarginalCDF("alpha", mp.Workingfolder() + "/alpha_Marginal_Distribution.csv");
    mp.SetMarginalCDF("n", mp.Workingfolder() + "/n_Marginal_Distribution.csv");
    mp.SetOutputfolder("Case 1");

    
    int i = 0; 
    if (mp.GetMode() == Mode::heterogeneous)
    {
        PropertyGenerator *P = new PropertyGenerator(mp.GridSize("z"), i * 505);
        P->Generate(&mp);
        mp.property_generator = P;
    }

    System *system=new System();
    ModelCreator ModCreate;
    cout<<"Creating model ..." <<endl;
    ModCreate.Create(mp,system);
    cout<<"Creating model done..." <<endl;
    
    system->SetSilent(false);
    cout<<"Saving"<<endl;
    system->SetProp("tend",10);
    system->SetProp("initial_time_step",0.1);
    if (mp.GetInflowMode() == FlowMode::periodic)
    {
        system->block("Well")->Variable("inflow")->TimeSeries()->writefile(mp.Workingfolder().toStdString() + "inflow.csv");
        system->block("Well")->Variable("inflow")->TimeSeries()->filename = mp.Workingfolder().toStdString() + "inflow.csv";
    }
    system->SavetoScriptFile(mp.Workingfolder().toStdString() + "/" + mp.Outputfolder().toStdString() + "/CreatedModel.ohq");

    cout<<"Solving ..."<<endl;
    system->SetProp("tend",mp.GetValue("t_end"));
    system->SetProp("initial_time_step",mp.GetValue("dt0"));
    system->Solve();
    cout<<"Writing outputs in '"<< system->GetWorkingFolder() + system->OutputFileName() +"'"<<endl;
    CTimeSeriesSet<double> output = system->GetOutputs();
    output.writetofile(system->GetWorkingFolder() + system->OutputFileName());
    cout<<"Getting results into grid"<<endl;
    ResultGrid resgrid(output,"theta",system);
    
    cout<<"Writing VTPs"<<endl;
    resgrid.WriteToVTP("Moisture_content",system->GetWorkingFolder()+"moisture.vtp");

    cout<<"Getting storage into grid"<<endl;
    ResultGrid resgridstorage(output,"Storage",system,false);
    cout<<"Writing TimeSeries"<<endl;
    (resgridstorage.Sum()-100000).writefile(system->GetWorkingFolder()+"storage.csv");

    if (mp.Tracer())
    {   cout<<"Getting concentration results into grid"<<endl;
        ResultGrid resgridconcentration(output,"Tracer:concentration",system);
        cout<<"Writing VTPs"<<endl;
        resgridconcentration.WriteToVTP("Tracer Concentration",system->GetWorkingFolder()+"tracer.vtp");

        cout<<"Getting mass results into grid"<<endl;
        ResultGrid resgridmass(output,"Tracer:mass",system, false);
        cout<<"Writing TimeSeries"<<endl;
        resgridmass.Sum().writefile(system->GetWorkingFolder()+"mass.csv");
    }

    vector<string> well_block; well_block.push_back("Well");
    ResultGrid well_depth = ResultGrid(output,well_block,"depth");
    well_depth.Sum().writefile(system->GetWorkingFolder()+"WaterDepth.csv");

    vector<string> GWRechargeBlocks;
    for (int i=0; i<mp.GridSize("r") ; i++)
    {
        GWRechargeBlocks.push_back(("Soil to Groundwater (" + QString::number(i) + ")").toStdString());
    }
    ResultGrid GW_recharge = ResultGrid(output,GWRechargeBlocks,"flow");
    GW_recharge.Sum().writefile(system->GetWorkingFolder() + "GW_recharge.csv");


    vector<string> InfiltrationBlocks;
    for (int i=0; i<mp.GridSize("z"); i++)
    {
        if (i*mp.GetValue("DepthtoGroundWater") / mp.GridSize("z")<mp.GetValue("DepthofWell"))
        InfiltrationBlocks.push_back(("Well to Soil (" + QString::number(i) + ")").toStdString());
    }
    InfiltrationBlocks.push_back("Well_to_bottom");
    ResultGrid infiltration_resgrid = ResultGrid(output,InfiltrationBlocks,"flow");
    infiltration_resgrid.Sum().writefile(system->GetWorkingFolder() + "Infiltration.csv");

    vector<string> InflowBlock;
    InflowBlock.push_back("Well");
    ResultGrid inflow_resgrid = ResultGrid(output,InfiltrationBlocks,"inflow");
    infiltration_resgrid.Sum().writefile(system->GetWorkingFolder() + "Inflow.csv");



    ResultGrid Ksat_grid = ResultGrid("K_sat_original",system);
    Ksat_grid.WriteToVTP("K_sat",system->GetWorkingFolder()+ "K_sat.vtp",1.0/Ksat_grid.maxval());

    ResultGrid alpha_grid = ResultGrid("alpha",system);
    alpha_grid.WriteToVTP("alpha",system->GetWorkingFolder()+ "alpha.vtp",1.0/alpha_grid.maxval());

    ResultGrid n_grid = ResultGrid("n",system);
    n_grid.WriteToVTP("n",system->GetWorkingFolder()+ "n.vtp",1.0/n_grid.maxval());


    vector<string> TTD_blocks;
    TTD_blocks.push_back("Groundwater");

    ResultGrid TTD = ResultGrid(output,TTD_blocks,"Tracer:mass");
    (TTD.Sum()/(mp.GetValue("DepthofWell") * mp.GetValue("rw") * mp.GetValue("rw") * 3.141521 * mp.GetValue("initial_concentration") * mp.GetValue("initial_water_depth"))).writefile(system->GetWorkingFolder() + "TTD_Cum.csv");

    (TTD.Sum()/(mp.GetValue("DepthofWell") * mp.GetValue("rw") * mp.GetValue("rw") * 3.141521 * mp.GetValue("initial_concentration") * mp.GetValue("initial_water_depth"))).derivative().writefile(system->GetWorkingFolder() + "TTD.csv");
    
    QTime end_time = QTime::currentTime();
    cout<<"End time = " + end_time.toString().toStdString();
    cout<<"Simulation Time = "<<(start_time.secsTo(end_time))/60.0<<endl;
    return 0;

}
