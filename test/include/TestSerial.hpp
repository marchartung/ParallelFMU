/*
 * TestSolver.hpp
 *
 *  Created on: 20.10.2015
 *      Author: marcus
 */

#ifndef TEST_INCLUDE_TESTSerial_HPP_
#define TEST_INCLUDE_TESTSerial_HPP_

#include "TestCommon.hpp"

class Serial : public TestCommon
{
 public:
    list<double> eventTimePoints;

    Serial()
            : TestCommon("./test/data/TestConfig_SimpleView_serial.xml")
    {
    }

    ~Serial()
    {
    }
};

TEST_F (Serial, TestDependenciesSerial)
{
    ASSERT_TRUE(_simulation->getSimulationType() == "serial");
    ASSERT_GT(_simulation->getSolver().size(),1);
    _simulation->setSimulationEndTime(0.5);
    _simulation->initialize();
    _simulation->simulate();
    for (const auto & solv : _simulation->getSolver())
        ASSERT_EQ(solv->getCurrentTime(), 0.5);
}

#endif /* TEST_INCLUDE_TESTSerial_HPP_ */
