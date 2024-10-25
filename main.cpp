#include "System.h"
#include "Script.h"
#include "qfileinfo.h"
#include "modelcreator.h"
#include "resultgrid.h"
#include "vtk.h"
#include <QTime>

int main(int argc, char *argv[])
{
    QTime start_time = QTime::currentTime();
    cout<<"Start time = " + start_time.toString().toStdString()<<endl;
    model_parameters mp;
    mp.nr = 6;
    mp.nz = 6;
    mp.K_sat = 1;
    mp.alpha = 20;
    mp.n = 1.8;
    mp.rw = 0.03;
    mp.theta_sat = 0.4;
    mp.theta_r = 0.05;
    mp.initial_theta = 0.2;
    mp.mode = model_parameters::_mode::homogeneous;
    mp.Correlation_Length_Scale = 1;
    mp.alpha_Ksat_Cor = 0.378;
    mp.K_sat_stdev = 0.5;
    mp.n_Ksat_Cor = 0.2;
    mp.tracer = true;
    mp.t_end = 30;
    mp.dt0 = 0.1;
    int i=0;

    string working_folder = "C:/Projects/DryWellModels/";
    if (mp.mode == model_parameters::_mode::heterogeneous)
    {
        PropertyGenerator *P = new PropertyGenerator(mp.nz,i*505);
        P->correlation_length_scale = mp.Correlation_Length_Scale;
        P->dx = mp.DepthtoGroundWater/mp.nz;
        P->assign_K_gauss(); // Assigns normal scores for K_sat
        P->Normalize_Ksat_normal_scores(0,mp.K_sat_stdev);
        P->write("K_sat_normal_score", working_folder + "K_sat_score_1.txt");

        CTimeSeries<double> K_sat_marginal_CDF(working_folder + "K_sat_Marginal_Distribution.csv"); // Loading Cummulative Distributions
        CTimeSeries<double> alpha_marginal_CDF(working_folder + "alpha_Marginal_Distribution.csv");
        CTimeSeries<double> n_marginal_CDF(working_folder + "n_Marginal_Distribution.csv");
        P->SetCorr(params::alpha, mp.alpha_Ksat_Cor); //Correlation between alpha and K_sat normal scores
        P->SetCorr(params::n, mp.n_Ksat_Cor); //Correlation between n and K_sat normal scores

        P->SetMarginalDistribution("K_sat",K_sat_marginal_CDF); //Assigning CDFs to properties

        P->SetMarginalDistribution("alpha",alpha_marginal_CDF);

        P->SetMarginalDistribution("n",n_marginal_CDF);

        P->PopulateRealValue("K_sat","K_sat_normal_score"); //Assign the actual K_sat

        P->Normalize("K_sat",P->mean("K_sat",true)); //Normalize K_sat by the geometrical mean of K_sat so the geometrical mean is zero

        P->Populate_Alpha_n_normal_scores(params::alpha); //Creates normal scores for alpha
        P->Populate_Alpha_n_normal_scores(params::n); //Creates normal scores for n
        P->PopulateRealValue("alpha","alpha_normal_score"); //Assign the actual values of alpha
        P->Normalize("alpha",0.05);
        P->PopulateRealValue("n","n_normal_score"); //.. n
        P->write("K_sat", working_folder + "K_sat.txt");
        P->write("alpha", working_folder + "alpha.txt");
        P->write("n", working_folder + "n.txt");
        mp.property_generator = P;
    }

    System *system=new System();
    ModelCreator ModCreate;
    cout<<"Creating model ..." <<endl;
    ModCreate.Create(mp,system);
    cout<<"Creating model done..." <<endl;
    system->SetWorkingFolder(working_folder);

    system->SetSilent(false);
    cout<<"Saving"<<endl;
    system->SetProp("tend",10);
    system->SetProp("initial_time_step",0.1);
    system->block("Well")->Variable("inflow")->TimeSeries()->writefile(working_folder + "inflow.csv");
    system->block("Well")->Variable("inflow")->TimeSeries()->filename = working_folder + "inflow.csv";
    system->SavetoScriptFile(working_folder +"CreatedModel.ohq");

    cout<<"Solving ..."<<endl;
    system->SetProp("tend",mp.t_end);
    system->SetProp("initial_time_step",mp.dt0);
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

    if (mp.tracer)
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
    for (int i=0; i<mp.nr+1; i++)
    {
        GWRechargeBlocks.push_back(("Soil to Groundwater (" + QString::number(i) + ")").toStdString());
    }
    ResultGrid GW_recharge = ResultGrid(output,GWRechargeBlocks,"flow");
    GW_recharge.Sum().writefile(system->GetWorkingFolder() + "GW_recharge.csv");


    vector<string> InfiltrationBlocks;
    for (int i=0; i<mp.nz; i++)
    {
        if (i*mp.DepthtoGroundWater/mp.nz<mp.DepthofWell)
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
    (TTD.Sum()/(mp.DepthofWell*mp.rw*mp.rw*3.141521*mp.initial_concentration*mp.initial_water_depth)).writefile(system->GetWorkingFolder() + "TTD_Cum.csv");

    (TTD.Sum()/(mp.DepthofWell*mp.rw*mp.rw*3.141521*mp.initial_concentration*mp.initial_water_depth)).derivative().writefile(system->GetWorkingFolder() + "TTD.csv");
    
    QTime end_time = QTime::currentTime();
    cout<<"End time = " + end_time.toString().toStdString();
    cout<<"Simulation Time = "<<(start_time.secsTo(end_time))/60.0<<endl;
    return 0;

}
