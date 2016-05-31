/*
 * TestSolver.hpp
 *
 *  Created on: 20.10.2015
 *      Author: marcus
 */

#ifndef TEST_INCLUDE_TESTSOLVERFMI_HPP_
#define TEST_INCLUDE_TESTSOLVERFMI_HPP_

#include "TestCommon.hpp"

class SolverFixture : public TestCommon
{
 public:
    SolverFixture() : TestCommon("./test/data/TestConfig_FmiLib.xml")
    {
    }

    ~SolverFixture()
    {
    }
};


#endif /* TEST_INCLUDE_TESTSOLVERFMI_HPP_ */
