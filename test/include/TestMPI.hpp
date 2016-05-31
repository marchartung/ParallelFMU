/*
 * TestSolver.hpp
 *
 *  Created on: 20.10.2015
 *      Author: marcus
 */

#ifndef TEST_INCLUDE_TESTMPITEST_HPP_
#define TEST_INCLUDE_TESTMPITEST_HPP_

#include "TestCommon.hpp"

class MPITest : public TestCommon
{
 public:
    list<double> eventTimePoints;

    MPITest()
            : TestCommon("./test/data/TestConfig_SimpleView_mpi.xml")
    {
    }

    ~MPITest()
    {
    }
};


TEST_F (MPITest, TestDependenciesMPI)
{
    ASSERT_EQ(_simulation->getSimulationType(),"mpi");
    _simulation->initialize();
    _simulation->simulate();
    for(const auto & solv : _simulation->getSolver())
    {
        ASSERT_EQ(solv->getCurrentTime(),0.5);
    }
}



#endif /* TEST_INCLUDE_TESTMPITEST_HPP_ */
