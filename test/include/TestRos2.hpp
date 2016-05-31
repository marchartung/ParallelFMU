#ifndef INCLUDE_TEST_TESTROS2_HPP_
#define INCLUDE_TEST_TESTROS2_HPP_

#include "TestCommon.hpp"

class SolverRos2 : public TestCommon
{
 public:

    SolverRos2()
            : TestCommon("./test/data/TestConfig_Ros2.xml")
    {
    }

    void TearDown() override
    {
        TestCommon::TearDown();
    }


    ~SolverRos2()
    {
    }
};

TEST_F (SolverRos2, TestRos2EventHandling)
{
    //first event of should occur approximately at t=0.45
    ASSERT_GT(_simulation->getSolver().size(),0);
    FMI::AbstractFmu* fmu = _simulation->getSolver().back()->getFmu();
    ASSERT_STREQ("BouncingBall", fmu->getFmuName().c_str());  //check if we are really using the bouncing ball model
    vector<double> stateValues = vector<double>(fmu->getNumStates(), 0.0);
    fmu->getStates(stateValues.data());
    ASSERT_DOUBLE_EQ(1.0, stateValues[0]);  //check if ball start with height of 1.0
    _simulation->setSimulationEndTime(0.8);
    _simulation->initialize();
    _simulation->simulate();
    ASSERT_EQ(3, _simulation->getSolver().back()->getEventCounter());
}

#endif /* INCLUDE_TEST_TESTROS2_HPP_ */
