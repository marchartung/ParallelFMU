/*
 * MatFileCMP.hpp
 *
 *  Created on: 16.02.2016
 *      Author: hartung
 */

#ifndef SOURCE_DIRECTORY__THIRDPARTY_MATFILECOMPARE_MATFILECMP_HPP_
#define SOURCE_DIRECTORY__THIRDPARTY_MATFILECOMPARE_MATFILECMP_HPP_

/*
 * MatFileCMP.cpp
 *
 *  Created on: 15.02.2016
 *      Author: hartung
 */
#include <stdexcept>
#include <vector>
#include <utility>
#include <memory>
#include <matio.h>

class MatFileCMP
{
 public:

    MatFileCMP(const std::vector<std::string >& leftFiles, const std::vector<std::string >& rightFiles, size_t numCmpPoints = 100);

    MatFileCMP(const std::string  & leftFile, const std::string  & rightFile, size_t numCmpPoints = 100);

    ~MatFileCMP();

    double getMaxError() const;

    void setVerboseLevel(int lvl);

 private:
    typedef std::vector<std::vector<double>> vector2D;

    struct ValRef
    {
        std::string  name;
        std::string  file;
        size_t pos;
        size_t finalPos;

        bool operator<(const ValRef & in) const
        {
            return name < in.name;
        }

        bool operator==(const ValRef & in) const
        {
            return name == in.name;
        }

        bool operator!=(const ValRef & in) const
        {
            return !(in == *this);
        }
    };

   std::vector<std::string > _lFiles;
   std::vector<std::string > _rFiles;
    size_t _numCmpPoints;
    double _delta;
    int _verboseLevel;

    std::pair<std::vector<ValRef>, std::vector<ValRef> > getCompareableVars() const;

    vector2D readVals(const std::vector<ValRef> & in) const;

    double calcMaxRelDiff(const vector2D& l, const vector2D & r) const;

    double calcMaxRelDiff(double l, double r) const;

   std::vector<ValRef> getVarsFromFile(const std::string & file) const;

    double interpolateAtT(double t, double t1, double t2, double a1, double a2) const;

    void readVals(const std::vector<ValRef>& in, vector2D& res, size_t offset, size_t range) const;

    matvar_t * getMatVar(const std::string  & file, const std::string  & varName) const;
};

#endif /* SOURCE_DIRECTORY__THIRDPARTY_MATFILECOMPARE_MATFILECMP_HPP_ */
