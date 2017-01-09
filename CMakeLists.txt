# This script will setup tests for the parallel FMI suite
#
# Some of these options can be controlled by passing arguments to CMAKE
#     if the FMI library is installed to a custom location              -DFMILIB_HOME=<path>


CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (ParallelFmuProject)

SET(CMAKE_VERBOSE_MAKEFILE ON)
#OPTION(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
MESSAGE(STATUS "Using Cmake version ${CMAKE_VERSION}")

# ---------------------------
# Find all dependencies
# ---------------------------
# Find MPI
FIND_PACKAGE(MPI)
IF(MPI_C_FOUND)
  MESSAGE(STATUS "MPI enabled")
  ADD_DEFINITIONS(-DUSE_MPI)
ELSE(MPI_C_FOUND)
  MESSAGE(STATUS "MPI disabled")
  SET(MPI_C_INCLUDE_PATH "")
  SET(MPI_C_LIBRARIES "")
  SET(MPI_C_LINK_FLAGS "")
ENDIF(MPI_C_FOUND)

# Find OpenMP
IF(USE_OPENMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    MESSAGE(STATUS "OpenMP enabled")
    SET(INTERNAL_USE_OPENMP TRUE)
    ADD_DEFINITIONS(-DUSE_OPENMP)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ELSE(OPENMP_FOUND)
    SET(INTERNAL_USE_OPENMP FALSE)
    MESSAGE(STATUS "OpenMP not found")
  ENDIF(OPENMP_FOUND)
ELSE(USE_OPENMP)
  MESSAGE(STATUS "OpenMP disabled")
ENDIF(USE_OPENMP)

# Find Lapack
FIND_PACKAGE(Lapack)
IF(NOT(LAPACK_FOUND))
  MESSAGE(FATAL_ERROR "Lapack not found but required.")
ELSE(NOT(LAPACK_FOUND))
  MESSAGE(STATUS "Using Lapack (LIB:${LAPACK_LIBRARIES})")
ENDIF(NOT(LAPACK_FOUND))

# Find Boost
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options filesystem system)
IF(Boost_FOUND)
  MESSAGE(STATUS "Boost enabled")
ELSE(Boost_FOUND)
  MESSAGE(FATAL_ERROR "Boost required")
ENDIF(Boost_FOUND)

# Find Doxygen
FIND_PACKAGE(Doxygen)

# Check C++11
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX11)
  ADD_COMPILE_OPTIONS(-std=c++11 -Wall)
ELSEIF(COMPILER_SUPPORTS_CXX0X)
  ADD_COMPILE_OPTIONS(-std=c++0x -Wall)
ELSE()
  MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF(COMPILER_SUPPORTS_CXX11)

# Find FMI library
FIND_PACKAGE(FMILib)
IF(FMILIB_FOUND)
  MESSAGE (STATUS "Using FMI library")
  ADD_DEFINITIONS(-DUSE_FMILIB)
ELSE(FMILIB_FOUND)
  SET(FMILIB_INCLUDE_DIR "")
  SET(FMILIB_LIBRARIES "")
ENDIF(FMILIB_FOUND)

# Find matio library
FIND_PACKAGE(Matio)
IF(MATIO_FOUND)
  MESSAGE(STATUS "Using matio")
ELSE(MATIO_FOUND)
  MESSAGE(FATAL_ERROR "Couldn't find matio")
ENDIF(MATIO_FOUND)

# Find FMU-SDK
FILE(GLOB_RECURSE FMUSDK_SRCS "thirdparty/fmusdk/src/*.c" "thirdparty/fmusdk/src/*.cpp")
SET(FMUSDK_INCLUDE_DIR "thirdparty/fmusdk/include")

#Find MatFileCompare
FILE(GLOB_RECURSE MATCMP_SRCS "thirdparty/MatFileCompare/MatFileCMP.cpp")
SET(MATCMP_INCLUDE_DIR "thirdparty/MatFileCompare")

# Adding sources
FILE(GLOB_RECURSE SRCS "src/*/*.cpp")
FILE(GLOB_RECURSE HEADERS "include/*/*.hpp" "include/*/*.h")
ADD_DEFINITIONS(-DUSE_LOGGER)


# Remove OpenMP-Files if no OpenMP is available
#SET(INTERNAL_USE_OPENMP FALSE) #for debugging only
IF(NOT(INTERNAL_USE_OPENMP))
#   MESSAGE(STATUS "Pre: ${SRCS}")
  LIST(REMOVE_ITEM SRCS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization//openmp/OpenMPConnection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization//openmp/OpenMPCounter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization//openmp/OpenMPDataHistory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/simulation//openmp/OpenMPSimulation.cpp"
  )
  LIST(REMOVE_ITEM HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/simulation/openmp/OpenMPSimulation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/OpenMPConnection.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/OpenMPCounter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/OpenMPDataHistory.hpp"
  )
# src/simulation/openmp/OpenMPSimulation.cpp" ) 
# "${CMAKE_CURRENT_SOURCE_DIR}/include/simulation/openmp/OpenMPSimulation.hpp" # 
# "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization/openmp/*" # 
# "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/")
#   MESSAGE(STATUS "Post: ${SRCS}")
ENDIF(NOT(INTERNAL_USE_OPENMP))

# Looking for visualization
#FIND_PACKAGE(OMVis)
#IF(OMVis_FOUND)
#	MESSAGE(STATUS "Building OMVis from submodule")
#	SET(OMVIS_FROM_SUBMODULE TRUE)
#ELSE(OMVis_FOUND)
#	MESSAGE(STATUS "Disable OMVis")
#	SET(OMVIS_FROM_SUBMODULE FALSE)
#ENDIF(OMVis_FOUND)



#looking for test dependencies
SET(BUILD_PARALLELFMU_TEST TRUE)
# 1. Find GTest
#IF(NOT OMVIS_FROM_SUBMODULE)
#	FIND_PACKAGE(GTEST)
#	IF(GTEST_FOUND)
#		MESSAGE(STATUS "Using GoogleTest from git submodule")
#		ADD_SUBDIRECTORY("${GTEST_SOURCE_DIR}")
#	ELSE(GTEST_FOUND)
#		MESSAGE(STATUS "No GoogleTest found.")
#		SET(BUILD_PARALLELFMU_TEST FALSE)
#	ENDIF(GTEST_FOUND)
#ELSE(NOT OMVIS_FROM_SUBMODULE)
#	MESSAGE(STATUS "Using GTest from OMVis")
#	SET(GTEST_INCLUDE_DIR "${OMVis_SOURCE_DIR}/thirdparty/gtest-1.7.0/include")
#	ADD_SUBDIRECTORY(${OMVis_SOURCE_DIR})
#ENDIF(NOT OMVIS_FROM_SUBMODULE)
FIND_PACKAGE(GTEST)
IF(GTEST_FOUND)
  MESSAGE(STATUS "Using GoogleTest from git submodule")
  ADD_SUBDIRECTORY("${GTEST_SOURCE_DIR}")
ELSE(GTEST_FOUND)
  MESSAGE(STATUS "No GoogleTest found.")
  SET(BUILD_PARALLELFMU_TEST FALSE)
ENDIF(GTEST_FOUND)



# 2. Find OMC
FIND_PACKAGE(OMC)
IF(OMC_FOUND)
  MESSAGE(STATUS "Set up tests with omc when possible")
ELSE(OMC_FOUND)
  MESSAGE(WARNING "Couldn't find omc. No tests will be installed")
  SET(BUILD_PARALLELFMU_TEST FALSE)
ENDIF(OMC_FOUND)


# Looking for server functionality
SET(BUILD_PARALLELFMU_SERVER TRUE)
FIND_PACKAGE(NetworkOffloader)
IF(NETWORK_OFFLOADER_FOUND)
  ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/NetworkOffloader")
  ADD_DEFINITIONS(-DUSE_NETWORK_OFFLOADER)
  MESSAGE(STATUS "Building NetworkOffload from submodule")
  FILE(GLOB_RECURSE NETWORK_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/network/src/*.cpp")
  SET(NETWORK_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/network/include")
ELSE(NETWORK_OFFLOADER_FOUND)
  MESSAGE(STATUS "Disable offloading. Module NetworkOffload is empty.")
  SET(BUILD_PARALLELFMU_SERVER FALSE)
  SET(NETWORK_SRCS "")
  SET(NETWORK_INCLUDES "")
ENDIF(NETWORK_OFFLOADER_FOUND)

# ---------------------------
# Build Documentation
# ---------------------------

IF(BUILD_DOCUMENTATION)
  IF(NOT DOXYGEN_FOUND)
    MESSAGE(FATAL_ERROR "Doxygen is needed to build the documentation.")
  ENDIF(NOT DOXYGEN_FOUND)
  MESSAGE(STATUS "Doxygen is used to build the documentation;-)")
  SET(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
  SET(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
  SET(doxyfile_html_in ${CMAKE_CURRENT_SOURCE_DIR}/doc)

  SET(DOC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/include")

  CONFIGURE_FILE(${doxyfile_in} ${doxyfile} @ONLY IMMEDIATE)

  ADD_CUSTOM_TARGET(pf_doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
ENDIF(BUILD_DOCUMENTATION)


# ---------------------------
# Add custom target fmucleancmake which will remove fmuTmp* folders
# ---------------------------

ADD_CUSTOM_TARGET(fmuclean
        COMMAND rm -rf fmuTmp*
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Removing fmuTmp folders."
        )

ADD_CUSTOM_TARGET(fmucleancmake
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
ADD_DEPENDENCIES(fmucleancmake fmuclean)



# ---------------------------
# Setup Build
# ---------------------------

# Handle this includes as system libraries. This suppresses compiler warnings.
INCLUDE_DIRECTORIES(SYSTEM ${NETWORK_INCLUDES} ${MPI_C_INCLUDE_PATH} ${MATIO_INCLUDE_DIR} ${FMILIB_INCLUDE_DIR} 
                           ${FMUSDK_INCLUDE_DIR} ${MATIO_INCLUDE_DIR} ${NETWORK_OFFLOADER_INCLUDE_DIR} 
                           ${MATCMP_INCLUDE_DIR} ${GTEST_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIR})

INCLUDE_DIRECTORIES(PRIVATE "include")

SET(LINK_LIBRARIES ${MATIO_LIBRARIES} ${NETWORK_OFFLOADER_LIBRARY} ${FMILIB_LIBRARIES} ${LAPACK_LIBRARIES} 
                   ${Boost_FILESYSTEM_LIBRARY} ${Boost_LIBRARIES} ${MPI_LIBRARIES} "dl" "expat")

ADD_EXECUTABLE(ParallelFmu ${SRCS} ${NETWORK_SRCS} ${FMUSDK_SRCS} "src/Main.cpp")
TARGET_LINK_LIBRARIES (ParallelFmu ${LINK_LIBRARIES})
INSTALL (TARGETS ParallelFmu DESTINATION "bin")



# adding testsuit if possible
IF(BUILD_PARALLELFMU_TEST)
  #compile FMUs
  MESSAGE(STATUS "Building FMUs for testing")
  IF(NOT (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/test/data/BouncingBall.fmu") OR "${CMAKE_CURRENT_SOURCE_DIR}/test/data/BouncingBall.mos" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/test/data/BouncingBall.fmu")
    FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test_tmp")
    EXECUTE_PROCESS(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/BouncingBall.mos" WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    IF(OMC_RESULT)
      MESSAGE(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    ELSE(OMC_RESULT)
      MESSAGE(STATUS "Built BouncingBall.fmu")
    ENDIF(OMC_RESULT)

    EXECUTE_PROCESS(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/SimpleView.mos" WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    IF(OMC_RESULT)
      MESSAGE(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    ELSE(OMC_RESULT)
      MESSAGE(STATUS "Built SimpleView.fmu")
    ENDIF(OMC_RESULT)

    EXECUTE_PROCESS(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/Blower.mos" WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    IF(OMC_RESULT)
      MESSAGE(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    ELSE(OMC_RESULT)
      MESSAGE(STATUS "Built Blower.fmu")
    ENDIF(OMC_RESULT)

    EXECUTE_PROCESS(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/BouncingBall_gravity_input.mos" WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    IF(OMC_RESULT)
      MESSAGE(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    ELSE(OMC_RESULT)
      MESSAGE(STATUS "Built BouncingBall_gravity_input.fmu")
    ENDIF(OMC_RESULT)

    FILE(GLOB_RECURSE XML_FMU_BINARIES "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/*.fmu" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/*.xml")
    FILE(COPY ${XML_FMU_BINARIES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test/data")
    FILE(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/test_tmp")
  ENDIF()

  # add executable and files
  INCLUDE_DIRECTORIES("include" ${NETWORK_INCLUDES} "test/include" ${MPI_C_INCLUDE_PATH} ${MATIO_INCLUDE_DIR} 
                                ${FMILIB_INCLUDE_DIR} ${FMUSDK_INCLUDE_DIR} ${MATIO_INCLUDE_DIR} 
                                ${NETWORK_OFFLOADER_INCLUDE_DIR} ${MATCMP_INCLUDE_DIR} ${GTEST_INCLUDE_DIR} 
                                ${Boost_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIR}})
  ADD_EXECUTABLE(testParallelFmu ${SRCS} ${NETWORK_SRCS} ${FMUSDK_SRCS} ${MATCMP_SRCS} "test/Main.cpp")
  TARGET_LINK_LIBRARIES (testParallelFmu ${LINK_LIBRARIES} "gtest" )
  FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test_tmp")
ENDIF(BUILD_PARALLELFMU_TEST)

IF(BUILD_PARALLELFMU_SERVER)

ENDIF(BUILD_PARALLELFMU_SERVER)

#INSTALL (DIRECTORY "data" DESTINATION ".")


# ---------------------------
# Build Precompiled Header
# ---------------------------

#INCLUDE("${CMAKE_CURRENT_LIST_DIR}/cmake/PrecompiledHeader.cmake")
#add_precompiled_header(test_fmu "include/Stdafx.hpp" SOURCE_CXX "src/Stdafx.cpp")
#add_precompiled_header(test_fmu_test "include/Stdafx.hpp" SOURCE_CXX "src/Stdafx.cpp")
