/*
 * TestSolver.hpp
 *
 *  Created on: 20.10.2015
 *      Author: marcus
 */

#ifndef TEST_INCLUDE_TESTOPENMPTEST_HPP_
#define TEST_INCLUDE_TESTOPENMPTEST_HPP_

#include "TestCommon.hpp"

class OpenMPTest : public TestCommon
{
 public:
    list<double> eventTimePoints;

    OpenMPTest()
            : TestCommon("./test/data/TestConfig_SimpleView_openmp.xml")
    {
    }

    ~OpenMPTest()
    {
    }
};


TEST_F (OpenMPTest, TestDependenciesOpenMP)
{
    ASSERT_EQ(_simulation->getSimulationType(),"serial");
    ASSERT_GT(_simulation->getSolver().size(),1);
    _simulation->initialize();
    _simulation->simulate();
    for(const auto& solv : _simulation->getSolver())
    {
        ASSERT_EQ(solv->getCurrentTime(),0.5);
    }
}



#endif /* TEST_INCLUDE_TESTOPENMPTEST_HPP_ */
