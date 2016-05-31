#ifndef INCLUDE_TEST_TESTFMUSDK_H_
#define INCLUDE_TEST_TESTFMUSDK_H_

#include "TestCommon.hpp"

class FmuSdkFixture : public TestCommon
{
 public:

    FmuSdkFixture()
            : TestCommon("./test/data/TestConfig_SdkFmu.xml")
    {
    }

    void TearDown() override
    {
        TestCommon::TearDown();
    }

    ~FmuSdkFixture()
    {
    }
};

TEST_F (FmuSdkFixture, TestInitialization)
{
    ASSERT_GT(_simulation->getSolver().size(),0);
    FMI::AbstractFmu* fmu = _simulation->getSolver().front()->getFmu();
    ASSERT_TRUE(fmu);
    fmu->load();
    ASSERT_EQ(0.0, fmu->getTime());
    ASSERT_EQ(2, fmu->getNumStates());
    vector<double> stateValues = vector<double>(fmu->getNumStates(), 0.0);
    fmu->getStates(stateValues.data());
    ASSERT_DOUBLE_EQ(1.0, stateValues[0]);
}

TEST_F (FmuSdkFixture, TestFmuSdkEventHandling)
{
    //first event of should occur approximately at t=0.45
    ASSERT_GT(_simulation->getSolver().size(),0);
    ASSERT_EQ(_simulation->getSimulationType(), "serial");
    _simulation->setSimulationEndTime(0.8);
    _simulation->initialize();
    FMI::AbstractFmu* fmu = _simulation->getSolver().back()->getFmu();
    ASSERT_STREQ("BouncingBall", fmu->getFmuName().c_str());  //check if we are really using the bouncing ball model
    vector<double> stateValues = vector<double>(fmu->getNumStates(), 0.0);
    fmu->getStates(stateValues.data());
    ASSERT_DOUBLE_EQ(1.0, stateValues[0]);  //check if ball start with height of 1.0
    _simulation->simulate();
    ASSERT_EQ(3, _simulation->getSolver().back()->getEventCounter());
}

#endif /* INCLUDE_TEST_TESTFMUSDK_H_ */
