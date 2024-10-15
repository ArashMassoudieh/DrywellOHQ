#ifndef RESULTGRID_H
#define RESULTGRID_H

#include <BTCSet.h>

struct point
{
    double x;
    double y;
};

class System;

class ResultGrid: public CTimeSeriesSet<double>
{
public:
    ResultGrid();
    ResultGrid(const ResultGrid&);
    virtual ~ResultGrid();
    ResultGrid& operator=(const ResultGrid&);
    ResultGrid(const CTimeSeriesSet<double> &cts, const string &quantity, System *system);
    vector<point> Positions;
};

#endif // RESULTGRID_H
