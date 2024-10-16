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
    mp.rw = 0.1;
    mp.theta_sat = 0.4;
    mp.theta_r = 0.05;
    mp.initial_theta = 0.2;
    mp.mode = model_parameters::_mode::heterogeneous;
    mp.Correlation_Length_Scale = 1;
    mp.alpha_Ksat_Cor = 0.378;
    mp.K_sat_stdev = 0.5;
    mp.n_Ksat_Cor = 0.2;
    int i=0;

    string working_folder = "/home/arash/Projects/DryWellModels/";
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
    system->SavetoScriptFile(working_folder +"CreatedModel.ohq");

    cout<<"Solving ..."<<endl;
    system->Solve();
    cout<<"Writing outputs in '"<< system->GetWorkingFolder() + system->OutputFileName() +"'"<<endl;
    CTimeSeriesSet<double> output = system->GetOutputs();
    output.writetofile(system->GetWorkingFolder() + system->OutputFileName());
    cout<<"Getting results into grid"<<endl;
    ResultGrid resgrid(output,"theta",system);
    cout<<"Writing VTPs"<<endl;
    resgrid.WriteToVTP("Moisture_content",system->GetWorkingFolder()+"moisture.vtp");

    return 0;

}
