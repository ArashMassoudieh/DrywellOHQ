#include "System.h"
#include "Script.h"
#include <QApplication>
#include "qfileinfo.h"
#include "modelcreator.h"




int main(int argc, char *argv[])
{

    QApplication a(argc,argv);
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

    System *system=new System();
    ModelCreator ModCreate;
    ModCreate.Create(mp,system);


    string defaulttemppath = qApp->applicationDirPath().toStdString() + "/../../resources/";
    cout << "Default Template path = " + defaulttemppath +"\n";
    system->SetDefaultTemplatePath(defaulttemppath);
    system->SetWorkingFolder("/home/arash/Projects/DryWellModels/");
    string settingfilename = qApp->applicationDirPath().toStdString() + "/../../resources/settings.json";
    system->SetSilent(false);
    cout<<"Saving"<<endl;
    system->SavetoScriptFile("/home/arash/Projects/DryWellModels/CreatedModel.ohq");

    cout<<"Solving ..."<<endl;
    system->Solve();
    cout<<"Writing outputs in '"<< system->GetWorkingFolder() + system->OutputFileName() +"'";
    system->GetOutputs().writetofile(system->GetWorkingFolder() + system->OutputFileName());
    return 0;

}
