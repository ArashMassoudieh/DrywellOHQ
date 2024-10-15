#include "resultgrid.h"
#include <QString>
#include <QStringList>
#include <string>
#include <System.h>

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
                append(cts.BTC[i],block_name);
            }

        }
    }
}
