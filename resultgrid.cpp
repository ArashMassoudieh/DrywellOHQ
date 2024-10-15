#include "resultgrid.h"
#include <QString>
#include <QStringList>
#include <string>
#include <System.h>
#include "vtk.h"

using namespace std;

ResultGrid::ResultGrid():CTimeSeriesSet<double>()
{

}

ResultGrid::ResultGrid(const ResultGrid& rhs):CTimeSeriesSet<double>(rhs)
{
    Positions = rhs.Positions;
}
ResultGrid::~ResultGrid()
{

}

ResultGrid& ResultGrid::operator=(const ResultGrid &rhs)
{
    CTimeSeriesSet::operator=(rhs);
    Positions = rhs.Positions;
}


ResultGrid::ResultGrid(const CTimeSeriesSet<double> &cts, const string &quantity, System *system)
{
    for (int i=0; i<cts.nvars; i++)
    {
        string block_name = QString::fromStdString(cts.names[i]).split("_")[0].toStdString();
        string quan = QString::fromStdString(cts.names[i]).split("_")[1].toStdString();
        if (quan==quantity)
        {
            point pt;
            if (system->block(block_name))
            {
                pt.x = system->block(block_name)->GetVal("act_X");
                pt.y = system->block(block_name)->GetVal("act_Y");
                Positions.push_back(pt);
                append(cts.BTC[i],block_name);
            }

        }
    }
}

#ifdef use_VTK
void ResultGrid::WriteToVTP(const std::string &quanname, const std::string &filename, int i, const double &scale) const
{
    vtkSmartPointer<vtkPoints> points_3 =
            vtkSmartPointer<vtkPoints>::New();

        double xx, yy, zz;
        vtkSmartPointer<vtkFloatArray> values =
            vtkSmartPointer<vtkFloatArray>::New();

        values->SetNumberOfComponents(1);

        values->SetName(quanname.c_str());
        //cout<<quanname<<endl;



        for (unsigned int j = 0; j < nvars; j++)
        {
            cout<<"Positions "<<j<<endl;
            yy = Positions[j].y;
            xx = Positions[j].x;
            zz = BTC[j].GetC(i)*scale;
            //cout<<"Positions "<<j<<":"<<xx<<":"<<yy<<":"<<zz<<" done!"<<endl;
            float tt = float(BTC[j].GetC(i));
            float t[1] = { tt };
            //cout<<"0.1"<<endl;
            points_3->InsertNextPoint(xx, yy, zz);
            //cout<<"0.2"<<endl;
            //cout<<tt<<":"<<t<<endl;
            values->InsertNextTupleValue(t);
            //cout<<"1"<<endl;
        }


        // Add the grid points to a polydata object
        vtkSmartPointer<vtkPolyData> inputPolyData =
            vtkSmartPointer<vtkPolyData>::New();
        inputPolyData->SetPoints(points_3);

        // Triangulate the grid points
        vtkSmartPointer<vtkDelaunay2D> delaunay =
            vtkSmartPointer<vtkDelaunay2D>::New();
    #if VTK_MAJOR_VERSION <= 5
        delaunay->SetInput(inputPolyData);
    #else
        delaunay->SetInputData(inputPolyData);
    #endif
        delaunay->Update();
        vtkPolyData* outputPolyData = delaunay->GetOutput();

        double bounds[6];
        outputPolyData->GetBounds(bounds);

        // Find min and max z
        double minz = bounds[4];
        double maxz = bounds[5];
        //cout<<"2"<<endl;
        // Create the color map
        vtkSmartPointer<vtkLookupTable> colorLookupTable =
            vtkSmartPointer<vtkLookupTable>::New();
        colorLookupTable->SetTableRange(minz, maxz);
        colorLookupTable->Build();

        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors_2 =
            vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors_2->SetNumberOfComponents(3);
        colors_2->SetName("Colors");

        //cout<<"3"<<endl;
        for (int i = 0; i < outputPolyData->GetNumberOfPoints(); i++)
        {
            double p[3];
            outputPolyData->GetPoint(i, p);

            double dcolor[3];
            colorLookupTable->GetColor(p[2], dcolor);

            unsigned char color[3];
            for (unsigned int j = 0; j < 3; j++)
            {
                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
            }
            //std::cout << "color: "
            //	<< (int)color[0] << " "
            //	<< (int)color[1] << " "
            //	<< (int)color[2] << std::endl;

            colors_2->InsertNextTupleValue(color);
        }
        //cout<<"4"<<endl;
        outputPolyData->GetPointData()->SetScalars(values);


        //Append the two meshes
        vtkSmartPointer<vtkAppendPolyData> appendFilter =
            vtkSmartPointer<vtkAppendPolyData>::New();
    #if VTK_MAJOR_VERSION <= 5
        appendFilter->AddInputConnection(input1->GetProducerPort());
        appendFilter->AddInputConnection(input2->GetProducerPort());
    #else
        //appendFilter->AddInputData(polydata);
        //appendFilter->AddInputData(polydata_1);
        appendFilter->AddInputData(outputPolyData);
    #endif
        appendFilter->Update();
        //cout<<"5"<<endl;

        // Visualization
        vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
        mapper->SetInputConnection(polydata->GetProducerPort());
    #else
        mapper->SetInputConnection(appendFilter->GetOutputPort());
        //mapper->SetInputData(polydata_1);
    #endif

        vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetPointSize(5);

        vtkSmartPointer<vtkXMLPolyDataWriter> writer =
            vtkSmartPointer<vtkXMLPolyDataWriter>::New();
        //cout<<"6"<<endl;
        writer->SetFileName(filename.c_str());
        writer->SetInputData(mapper->GetInput());
        // This is set so we can see the data in a text editor.
        writer->SetDataModeToAscii();
        writer->Write();
        //cout<<"7"<<endl;

}

void ResultGrid::WriteToVTP(const std::string &quanname, const std::string &filename, const double &scale) const
{
    for (unsigned k=0; k<BTC[0].n; k++)
    {
        cout<<"snapshot: "<<k<<endl;
        std::string name = aquiutils::split(filename,'.')[0]+"_"+aquiutils::numbertostring(k+1,4)+".vtp";
        cout<<name<<endl;
        WriteToVTP(quanname, name, k, scale);
    }
}
#endif
