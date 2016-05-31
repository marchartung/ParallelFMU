/*
 * MatFileCMP.cpp
 *
 *  Created on: 15.02.2016
 *      Author: hartung
 */

#include "MatFileCMP.hpp"
#include <stdexcept>
#include <list>
#include <cmath>
#include <iostream>

MatFileCMP::MatFileCMP(const std::vector<std::string >& leftFiles, const std::vector<std::string >& rightFiles, size_t numCmpPoints)
        : _lFiles(leftFiles),
          _rFiles(rightFiles),
          _numCmpPoints(numCmpPoints),
          _delta(1.0e-7),
          _verboseLevel(0)
{
}

MatFileCMP::~MatFileCMP()
{
}

MatFileCMP::MatFileCMP(const std::string & leftFile, const std::string & rightFile, size_t numCmpPoints)
        : _lFiles(std::vector<std::string >(1, leftFile)),
          _rFiles(std::vector<std::string >(1, rightFile)),
          _numCmpPoints(numCmpPoints),
          _delta(1.0e-7),
          _verboseLevel(0)
{
}

void MatFileCMP::setVerboseLevel(int lvl)
{
    _verboseLevel = lvl;
}

std::pair<std::vector<MatFileCMP::ValRef>, std::vector<MatFileCMP::ValRef> > MatFileCMP::getCompareableVars() const
{
    std::vector<ValRef> tmp, lRefs, rRefs;
    std::pair<std::vector<ValRef>, std::vector<ValRef> > res;
    size_t counter = 0;
    std::list<size_t> notFound;
    bool found;
    for (size_t i = 0; i < _lFiles.size(); ++i)
    {
        tmp = getVarsFromFile(_lFiles[i]);
        lRefs.insert(lRefs.end(), tmp.begin(), tmp.end());
    }

    for (size_t i = 0; i < _rFiles.size(); ++i)
    {
        tmp = getVarsFromFile(_rFiles[i]);
        rRefs.insert(rRefs.end(), tmp.begin(), tmp.end());
    }

    std::vector<ValRef> & r = (lRefs.size() > rRefs.size()) ? rRefs : lRefs, &l = (lRefs.size() > rRefs.size()) ? lRefs : rRefs;

    for (size_t i = 0; i < l.size(); ++i)
    {
        found = false;
        for (size_t j = 0; j < r.size(); ++j)
            if (l[i] == r[j])
            {
                found = true;
                l[i].finalPos = counter;
                r[i].finalPos = counter++;
                res.first.push_back(l[i]);
                res.second.push_back(r[i]);
                continue;
            }
        if (!found)
        {
            notFound.push_back(i);
        }
    }
    if (_verboseLevel && notFound.size() > 0)
    {
        std::cout << "The following variables couldn't be found:\n";
        for (const int & i : notFound)
        {
            std::cout << "[" << l[i].name << "] ";
        }
        std::cout << "\n";
    }
    return res;
}

double MatFileCMP::getMaxError() const
{
    std::pair<std::vector<ValRef>, std::vector<ValRef> > vars = getCompareableVars();
    vector2D l = readVals(vars.first);
    vector2D r = readVals(vars.second);
    return calcMaxRelDiff(l,r);
}


double MatFileCMP::calcMaxRelDiff(const vector2D& l, const vector2D & r) const
{
    double res = 0, tmp;

    for (size_t j = 0; j < l[0].size(); ++j)
    for (size_t i = 0; i < l.size(); ++i)
        {
            tmp = calcMaxRelDiff(l[i][j], r[i][j]);
            //std::cout << i << "x" << j << ": " << l[i][j] << " - " << r[i][j] << " = " << tmp << "\n";
            if (tmp > res)
                res = tmp;
        }

    return res;
}

double MatFileCMP::calcMaxRelDiff(double l, double r) const
{

    return std::abs(l - r) / (std::abs(l) + std::abs(r) + _delta);
}

std::vector<MatFileCMP::ValRef> MatFileCMP::getVarsFromFile(const std::string & file) const
{
    std::vector<ValRef> res;
    ValRef tmp;

    matvar_t * m = getMatVar(file, "dataInfo");

    std::vector<double> dI(m->dims[1]);

    for (size_t i = 0; i < dI.size(); ++i)
        dI[i] = ((double*) m->data)[4 * i];

    Mat_VarFree(m);

    m = getMatVar(file, "name");

    for (size_t i = 0; i < m->dims[1]; ++i)
    {
        if (dI[i] != 2.0)
            continue;  // don't consider parameter
        char * cStart = &((char*)m->data)[i * m->dims[0]];
        tmp.name = std::string (cStart,m->dims[0]);
        tmp.file = file;
        tmp.pos = i;
        res.push_back(tmp);
    }
    Mat_VarFree(m);
    return res;
}

double MatFileCMP::interpolateAtT(double t, double t1, double t2, double a1, double a2) const
{
    return (t - t1) / (t2 - t1) * (a2 - a1) + a1;
}

MatFileCMP::vector2D MatFileCMP::readVals(const std::vector<MatFileCMP::ValRef> & in) const
{
    std::string  curFile;
    vector2D res(in.size());
    size_t offset = 0, range;
    while (offset < in.size())
    {
        curFile = in[offset].file;
        for (range = offset; range < in.size(); ++range)
            if (curFile != in[range].file)
                break;
        readVals(in, res, offset, range);
        offset = range;
    }
    return res;
}

void MatFileCMP::readVals(const std::vector<ValRef>& in, vector2D& res, const size_t offset, const size_t range) const
{
    matvar_t * m = getMatVar(in[offset].file, "data_2");

    double * curLine = (double*) m->data;
    size_t sizeLine = m->dims[0], numLines = m->dims[1], numWrites = 0;
    double tStart = curLine[0]+_delta, tEnd = curLine[sizeLine * (numLines - 1)]+_delta;
    double curCmpPoint = tStart, cmpStep = (tEnd - tStart) / (_numCmpPoints);

    while(numWrites<_numCmpPoints)
    {
        if (curLine[0] < curCmpPoint)
        {
            curLine += sizeLine;
            continue;
        }
        for (size_t j = offset; j < range; ++j)
            res[in[j].finalPos].push_back( interpolateAtT(curCmpPoint, curLine[-sizeLine], curLine[0], curLine[in[j].pos - sizeLine], curLine[in[j].pos]));
        ++numWrites;
        curCmpPoint += cmpStep;
    }

    Mat_VarFree(m);
}

matvar_t * MatFileCMP::getMatVar(const std::string  & file, const std::string  & varName) const
{
    mat_t * f = NULL;
    matvar_t * m = NULL;
    f = Mat_Open(file.c_str(), MAT_ACC_RDONLY);
    if (f == NULL)
        throw std::runtime_error("File " + file + " does not exist.");

    m = Mat_VarRead(f,varName.c_str());

    if (m == NULL)
        throw std::runtime_error("Variable " + varName + " doesn't exist in " + file + ".");
    Mat_Close(f);

    return m;
}

