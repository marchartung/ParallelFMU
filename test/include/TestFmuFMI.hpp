/*
 * TestFmuFMI.hpp
 *
 *  Created on: 09.11.2015
 *      Author: hartung
 */

#ifndef SOURCE_DIRECTORY__TEST_INCLUDE_TESTFMUFMI_HPP_
#define SOURCE_DIRECTORY__TEST_INCLUDE_TESTFMUFMI_HPP_

#include "TestCommon.hpp"

class FmuFMIFixture : public TestCommon
{
 public:

    FmuFMIFixture()
            : TestCommon("./test/data/TestConfig_FmiLib.xml")
    {
    }

    void TearDown() override
    {
        TestCommon::TearDown();
    }

    ~FmuFMIFixture()
    {
    }
};

TEST_F (FmuFMIFixture, TestInitialization)
{

    ASSERT_GT(_simulation->getSolver().size(),0);
    FMI::AbstractFmu* fmu = _simulation->getSolver().back()->getFmu();
    ASSERT_EQ(_simulation->getSimulationType(), "serial");
    ASSERT_TRUE(fmu);
    fmu->load();
    ASSERT_EQ(0.0, fmu->getTime());
    ASSERT_EQ(2, fmu->getNumStates());
    vector<double> stateValues = vector<double>(fmu->getNumStates(), 0.0);
    fmu->getStates(stateValues.data());
    ASSERT_DOUBLE_EQ(1.0, stateValues[0]);
}

TEST_F (FmuFMIFixture, TestFMIEventHandling)
{
    //first event of should occur approximately at t=0.45
    ASSERT_GT(_simulation->getSolver().size(),0);
    ASSERT_EQ(_simulation->getSimulationType(), "serial");
    FMI::AbstractFmu* fmu = _simulation->getSolver().back()->getFmu();
    ASSERT_STREQ("BouncingBall", fmu->getFmuName().c_str());  //check if we are really using the bouncing ball model
    vector<double> stateValues = vector<double>(fmu->getNumStates(), 0.0);
    fmu->getStates(stateValues.data());
    ASSERT_DOUBLE_EQ(1.0, stateValues[0]);  //check if ball start with height of 1.0
    _simulation->setSimulationEndTime(0.8);
    _simulation->simulate();
    ASSERT_EQ(3, _simulation->getSolver().back()->getEventCounter());
}

#endif /* SOURCE_DIRECTORY__TEST_INCLUDE_TESTFMUFMI_HPP_ */
