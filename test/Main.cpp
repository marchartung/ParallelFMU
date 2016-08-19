#include <gtest/gtest.h>


//#include "TestSerial.hpp"
#ifdef USE_FMILIB
//#include "TestFmuFMI.hpp"
#endif
//#include "TestOpenMP.hpp"
//#include "TestMPI.hpp"
#include "TestFmuSdk.hpp"

/*#include "TestXmlReader.hpp"
#include "TestRos2.hpp"
*/
//#include "TestBouncing1000_openmp.hpp"
//#include "TestBouncing1000.hpp"

int_type main(int_type argc, char **argv)
{
    Util::LogSettings logSettings;
    logSettings.setAll(Util::LL_WARNING);
    Util::Logger::initialize(logSettings);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

