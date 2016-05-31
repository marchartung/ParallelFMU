#ifndef TEST_INCLUDE_TESTXMLREADER_HPP_
#define TEST_INCLUDE_TESTXMLREADER_HPP_

//#include <tuple>
//
//#include "BasicTypedefs.hpp"
//#include "TestCommon.hpp"
//
///**
// * This class provides fixtures to test the Initialization::XMLConfigurationReader class and its methods.
// */
//class XmlReaderFixture : public TestCommon
//{
// public:
//    XmlReaderFixture()
//            : TestCommon("./test/data/TestConfig_MPI.xml")
//    {
//
//    }
//
//    void SetUp()
//    {
//    }
//
//    void TearDown()
//    {
//    }
//
//    ~XmlReaderFixture()
//    {
//    }
//};
//
///**
// * Fixture to test the FmuLoaderConfiguration object during initialization phase.
// * We check for correctness of the ID, loaderType and workingDirectory.
// */
//TEST_F (XmlReaderFixture, TestFmuLoaderConfiguration)
//{
//    // MPI configuration has two loaders.
//    ASSERT_EQ(config->getFmuLoaderConfigurations().size(), 2);
//
//    // Now we check all loaders. This can be done using a for loop since the loader objects
//    // are stored in a vector in the configuration class:
//    //   - The working directory of all loaders is "./".
//    //   - The loader IDs are 1 and 2, respectively.
//    //   - The loaders are of type "fmuSdkFmuLoader" and "fmiLibraryFmuLoader".
//    vector<string_type> loaderType = { "fmuSdkFmuLoader", "fmiLibraryFmuLoader" };
//    string_type wDir = "./";
//    vector<size_type> id = {1, 2};
//    for (int_type i = 0; i != 2; ++i)
//    {
//        ASSERT_EQ(wDir, config->getFmuLoaderConfigurations()[i]->workingDirectory);
//        ASSERT_EQ(id[i], config->getFmuLoaderConfigurations()[i]->id);
//        ASSERT_EQ(loaderType[i], config->getFmuLoaderConfigurations()[i]->loaderTypeName);
//    }
//}
//
///**
// * More general tests on the ConfigurationGraph object, e.g., we check if the number of nodes
// * and edges is correct.
// */
//TEST_F (XmlReaderFixture, TestConfigurationGraph)
//{
//    Initialization::ConfigurationGraphSPtr configGraph = config->getConfigurationGraph();
//    vector<Initialization::ConfigurationGraphNodeSPtr> nodes = configGraph->getNodes();
//
//    // Check the number of nodes in the ConfigurationGraph, i.e., the number of FMUs.
//    ASSERT_EQ(nodes.size(), 4);
//
//    // Check the number of edges in the ConfigurationGraph, i.e., the number of connections between the FMUs.
//    // This quantity is equal to the number of non zero elements in the adjacency matrix.
//    ASSERT_EQ(configGraph->getNumOfEdges(), 4);
//
//}
//
///**
// * This fixture provides complex and detailed tests on the ConfigurationGraph::adjacencyMatrix object
// * which represents the edges in the configuration graph.
// * We check the dimension of the matrix as well as their values.
// */
//TEST_F (XmlReaderFixture, TestConfigurationGraphEdges)
//{
//    // Get pointer to ConfigurationGraph object.
//    Initialization::ConfigurationGraphSPtr configGraph = config->getConfigurationGraph();
//    vector<list<Initialization::ConfigurationGraphEdgeSPtr>> adjacencyMatrix = configGraph->getAdjacencyMatrix();
//
//    // Predefined values to check against.
//    vector<vector<string_type>> source = { { "BouncingBall1", "BouncingBall1" }, { "BouncingBall2" }, { "BouncingBall3" } };
//    vector<vector<string_type>> dest = { { "BouncingBall2", "BouncingBall3" }, { "BouncingBall4" }, { "BouncingBall4" } };
//
//    // Number of rows in adjacency matrix equals number of FMUs.
//    ASSERT_EQ(adjacencyMatrix.size(), 4);
//
//    // Check source and destination of edges.
//    // Iterate over the rows.
//    for (size_t i = 0; i != adjacencyMatrix.size(); ++i)
//    {
//        size_t jj = 0;
//        // Iterate over all elements in the current row.
//        for (auto j : adjacencyMatrix[i])
//        {
//            //cout << i << "," << jj << "\t" << j->dest << "\t" << endl;
//            //cout << i << "," << jj << "\t" << source[i][jj] << endl;
//            ASSERT_EQ(j->source, source[i][jj]);
//            ASSERT_EQ(j->dest, dest[i][jj]);
//            //cout << j->varMapping.size() << endl;
//            ++jj;
//        }
//    }
//
//    // Check variable mappings, i.e., mapping of input to output variables for connections between two FMUs.
//    vector<vector<vector<tuple<string, size_t, size_t>>> > varMapping =
//    {  // First dimension
//        {  // Second dimension
//            {   std::make_tuple("real", 1, 1),
//                std::make_tuple("real", 2, 2),
//                std::make_tuple("int", 3, 3),
//                std::make_tuple("bool_type", 4, 4),
//                std::make_tuple("string", 5, 5)
//            },
//            {   std::make_tuple("real", 6, 6),
//                std::make_tuple("real", 7, 7),
//                std::make_tuple("int", 8, 8),
//                std::make_tuple("bool_type", 9, 9),
//                std::make_tuple("string", 10, 10)
//            },
//        },
//        {
//            {   std::make_tuple("real", 11, 11),
//                std::make_tuple("real", 12, 12),
//                std::make_tuple("int", 13, 13),
//                std::make_tuple("bool_type", 14, 14),
//                std::make_tuple("string", 15, 15)
//            }
//        },
//        {
//            {   std::make_tuple("real", 16, 16),
//                std::make_tuple("real", 17, 17),
//                std::make_tuple("int", 18, 18),
//                std::make_tuple("bool_type", 19, 19),
//                std::make_tuple("string", 20, 20)
//            }
//        }
//    };
//
//    // Okay, we set up the vector of vector of something and called it varMapping. Now, it is time to use this beast
//    // for testing against data structure ConfigurationGraph::adjacencyMatrix.
//    size_t i = 0, j, k;
//    for (auto row : adjacencyMatrix)
//    {
//        j = 0;
//        for (auto col : row)
//        {
//            k = 0;
//            for (auto& edge : col->varMapping)
//            {
//                ASSERT_EQ(std::get<0>(edge), std::get<0>(varMapping[i][j][k]));
//                ASSERT_EQ(std::get<1>(edge), std::get<1>(varMapping[i][j][k]));
//                ASSERT_EQ(std::get<2>(edge), std::get<2>(varMapping[i][j][k]));
//                //cout << "blubs " << std::tuple_size<decltype(edge)>::value << endl;
//                //cout << i << " " << j << " " << k << "\t" << std::get<0>(edge) << ", " << std::get<1>(edge) << ", " << std::get<2>(edge) << endl;
//                ++k;
//            }
//            ++j;
//        }
//        ++i;
//    }
//}
//
///**
// * This fixture provides tests for the nodes of the ConfigurationGraph object. We check for
// * correctness of the FMU name and path, loader ID, solver ID and relative tolerance.
// */
//TEST_F (XmlReaderFixture, TestConfigurationGraphNodes)
//{
//    // Get pointer to ConfigurationGraph object.
//    Initialization::ConfigurationGraphSPtr configGraph = config->getConfigurationGraph();
//    vector<Initialization::ConfigurationGraphNodeSPtr> nodes = configGraph->getNodes();
//
//    // Check the names of all FMUs.
//    vector<string_type> fmuNamesIn = configGraph->getFmuNames();
//    vector<string_type> fmuNamesSet = { "BouncingBall1", "BouncingBall2", "BouncingBall3", "BouncingBall4" };
//    ASSERT_EQ(fmuNamesIn.size(), fmuNamesSet.size());
//    for (size_t i = 0; i != fmuNamesIn.size(); ++i)
//        ASSERT_EQ(fmuNamesSet[i], fmuNamesIn[i]);
//
//    // Check the loader and solver Ids, path to the FMUs and relative tolerance.
//    vector<size_t> loaders = { 1, 1, 1, 2 };
//    vector<size_t> solvers = { 1, 2, 3, 4 };
//    vector<string_type> path(4, "test/data/BouncingBall.fmu");
//    vector<double> relTol(4, 1.0e-5);
//
//    for (size_t i = 0; i != nodes.size(); ++i)
//    {
//        ASSERT_EQ(nodes[i]->loaderId, loaders[i]);
//        ASSERT_EQ(nodes[i]->solverId, solvers[i]);
//        ASSERT_EQ(nodes[i]->fmuPath, path[i]);
//        ASSERT_EQ(nodes[i]->relativeTolerance, relTol[i]);
//    }
//}
//
///**
// * This fixture provides test for the SolverConfiguration object. We check for correctness of
// * solverTypeName, ID and defaultStepSize.
// */
//TEST_F (XmlReaderFixture, TestSolverConfiguration)
//{
//    vector<Initialization::SolverConfigurationSPtr> solverConfigs = config->getSolverConfigurations();
//
//    // Check for correct number of solvers.
//    ASSERT_EQ(solverConfigs.size(), 4);
//
//    // Check for correctness of attributes.
//    vector<string_type> solverName = {"euler", "euler", "euler", "euler"};
//    vector<size_t> solverId = {1, 2, 3, 4};
//    vector<double> stepSize = {0.0001, 0.0001, 0.0001, 0.0001};
//    int_type i(0);
//    for (auto solver : solverConfigs)
//    {
//        ASSERT_EQ(solver->solverTypeName, solverName[i]);
//        ASSERT_EQ(solver->id, solverId[i]);
//        ASSERT_EQ(solver->defaultStepSize, stepSize[i]);
//        ++i;
//    }
//}
//
///**
// * This fixture provides test for the WriterConfiguration object. We check for correctness of
// * writrTypeName, ID, resultFile and numOutputSteps.
// */
//TEST_F (XmlReaderFixture, WriterConfiguration)
//{
//    Initialization::WriterConfigurationSPtr writerConf = config->getWriterConfiguration();
//    ASSERT_EQ(writerConf->writerTypeName, "csvFileWriter");
//    ASSERT_EQ(writerConf->id, 1);
//    ASSERT_EQ(writerConf->resultFile, "result.csv");
//}
//
///**
// * This fixture provides test for the SimulationConfiguration object. We check for correctness of
// * startTime and endTime.
// */
//TEST_F (XmlReaderFixture, TestSimulationConfiguration)
//{
//    Initialization::SimulationConfigurationSPtr simConf = config->getSimulationConfiguration();
//    ASSERT_EQ(simConf->startTime, 2.2);
//    ASSERT_EQ(simConf->endTime, 3.3);
//}

#endif /* TEST_INCLUDE_TESTXMLREADER_HPP_ */
