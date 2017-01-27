# This script will setup tests for the parallel FMI suite
#
# Some of these options can be controlled by passing arguments to CMAKE
#     if the FMI library is installed to a custom location              -DFMILIB_HOME=<path>


cmake_minimum_required(VERSION 2.8)
project(ParallelFmuProject)

set(CMAKE_VERBOSE_MAKEFILE ON)
#OPTION(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message(STATUS "Using Cmake version ${CMAKE_VERSION}")


# ------------------------------
# Find all required dependencies
# ------------------------------

# Find Lapack
find_package(Lapack)
if(NOT(LAPACK_FOUND))
  message(FATAL_ERROR "Lapack not found but required")
else(NOT(LAPACK_FOUND))
  message(STATUS "Using Lapack (LIB:${LAPACK_LIBRARIES})")
endif(NOT(LAPACK_FOUND))

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options filesystem system)
if(NOT(Boost_FOUND))
  message(FATAL_ERROR "Boost not found but required")
else(NOT(Boost_FOUND))
  message(STATUS "Boost found")
endif(NOT(Boost_FOUND))

# Find matio library
find_package(Matio)
if(NOT(MATIO_FOUND))
  message(FATAL_ERROR "Matio not found but required")
else(NOT(MATIO_FOUND))
  message(STATUS "Matio found")
endif(NOT(MATIO_FOUND))

# Find FMU-SDK
file(GLOB_RECURSE FMUSDK_SRCS "thirdparty/fmusdk/src/*.c" "thirdparty/fmusdk/src/*.cpp")
set(FMUSDK_INCLUDE_DIR "thirdparty/fmusdk/include")

#Find MatFileCompare
file(GLOB_RECURSE MATCMP_SRCS "thirdparty/MatFileCompare/MatFileCMP.cpp")
set(MATCMP_INCLUDE_DIR "thirdparty/MatFileCompare")


# ------------------------------
# Find all optional dependencies
# ------------------------------

# Find MPI
find_package(MPI)
if(MPI_C_FOUND)
  message(STATUS "MPI found")
  set(INTERNAL_USE_MPI TRUE)
  add_definitions(-DUSE_MPI)
else(MPI_C_FOUND)
  message(STATUS "MPI not found")
  set(INTERNAL_USE_MPI FALSE)
  set(MPI_C_INCLUDE_PATH "")
  set(MPI_C_LIBRARIES "")
  set(MPI_C_LINK_FLAGS "")
endif(MPI_C_FOUND)

# Find OpenMP
find_package(OpenMP)
if(OPENMP_FOUND)
  message(STATUS "OpenMP found")
  set(INTERNAL_USE_OPENMP TRUE)
  add_definitions(-DUSE_OPENMP)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else(OPENMP_FOUND)
  message(STATUS "OpenMP not found")
  set(INTERNAL_USE_OPENMP FALSE)
endif(OPENMP_FOUND)

# Find FMI library
find_package(FMILib)
if(FMILIB_FOUND)
  message (STATUS "FMI library found")
  add_definitions(-DUSE_FMILIB)
else(FMILIB_FOUND)
  message (STATUS "FMI library not found")
  set(FMILIB_INCLUDE_DIR "")
  set(FMILIB_LIBRARIES "")
endif(FMILIB_FOUND)

# Find Doxygen
find_package(Doxygen)

# Looking for server functionality
set(BUILD_PARALLELFMU_SERVER TRUE)
find_package(NetworkOffloader)
if(NETWORK_OFFLOADER_FOUND)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/NetworkOffloader")
  add_definitions(-DUSE_NETWORK_OFFLOADER)
  message(STATUS "Building NetworkOffload from submodule")
  file(GLOB_RECURSE NETWORK_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/network/src/*.cpp")
  set(NETWORK_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/network/include")
else(NETWORK_OFFLOADER_FOUND)
  message(STATUS "Disable offloading. Module NetworkOffload is empty.")
  set(BUILD_PARALLELFMU_SERVER FALSE)
  set(NETWORK_SRCS "")
  set(NETWORK_INCLUDES "")
endif(NETWORK_OFFLOADER_FOUND)


# ------------------------------
# Find dependencies for tests
# ------------------------------

# Find GoogleTest
find_package(GTEST)
if(GTEST_FOUND)
  message(STATUS "Using GoogleTest from git submodule")
  set(BUILD_PARALLELFMU_TEST TRUE)
  add_subdirectory("${GTEST_SOURCE_DIR}")
else(GTEST_FOUND)
  message(STATUS "No GoogleTest found.")
  set(BUILD_PARALLELFMU_TEST FALSE)
endif(GTEST_FOUND)

# Find OMC
find_package(OMC)
if(OMC_FOUND)
  message(STATUS "omc found")
else(OMC_FOUND)
  message(WARNING "OMC not found. No tests will be installed")
  set(BUILD_PARALLELFMU_TEST FALSE)
endif(OMC_FOUND)

# Check C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  add_compile_options(-std=c++11 -Wall)
elseif(COMPILER_SUPPORTS_CXX0X)
  add_compile_options(-std=c++0x -Wall)
else(COMPILER_SUPPORTS_CXX11)
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif(COMPILER_SUPPORTS_CXX11)


# ------------------------------
# Headers and Sources
# ------------------------------

# Adding sources
file(GLOB_RECURSE SRCS "src/*/*.cpp")
file(GLOB_RECURSE HEADERS "include/*/*.hpp" "include/*/*.h")
add_definitions(-DUSE_LOGGER)

# Remove OpenMP-Files if OpenMP is not available
#set(INTERNAL_USE_OPENMP FALSE) #for debugging only
if(NOT(INTERNAL_USE_OPENMP))
  #message(STATUS "Pre: ${SRCS}")
  list(REMOVE_ITEM SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization/openmp/OpenMPConnection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization/openmp/OpenMPCounter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization/openmp/OpenMPDataHistory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/simulation/openmp/OpenMPSimulation.cpp"
  )
  list(REMOVE_ITEM HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/simulation/openmp/OpenMPSimulation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/OpenMPConnection.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/OpenMPCounter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/openmp/OpenMPDataHistory.hpp"
  )
endif(NOT(INTERNAL_USE_OPENMP))

# Remove MPI-Files if MPI is not available
#set(INTERNAL_USE_MPI FALSE) #for debugging only
if(NOT(INTERNAL_USE_MPI))
  message(STATUS "Pre: ${SRCS}")
  list(REMOVE_ITEM SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/synchronization/mpi/MPIConnection.cpp"
  )
  list(REMOVE_ITEM HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/synchronization/mpi/MPIConnection.hpp"
  )
endif(NOT(INTERNAL_USE_MPI))


# ---------------------------
# Build Documentation
# ---------------------------

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif(NOT DOXYGEN_FOUND)
  message(STATUS "Doxygen is used to build the documentation.")
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
  set(doxyfile_html_in ${CMAKE_CURRENT_SOURCE_DIR}/doc)

  set(DOC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/include")

  configure_file(${doxyfile_in} ${doxyfile} @ONLY IMMEDIATE)

  add_custom_target(pf_doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
endif(BUILD_DOCUMENTATION)


# ---------------------------
# Add custom target fmucleancmake which will remove fmuTmp* folders
# ---------------------------

add_custom_target(fmuclean
        COMMAND rm -rf fmuTmp*
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Removing fmuTmp folders."
        )

add_custom_target(fmucleancmake
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
add_dependencies(fmucleancmake fmuclean)


# ---------------------------
# Setup Build
# ---------------------------

# Handle this includes as system libraries. This suppresses compiler warnings.
include_directories(SYSTEM ${NETWORK_INCLUDES} ${NETWORK_OFFLOADER_INCLUDE_DIR} ${MPI_C_INCLUDE_PATH}
                           ${FMILIB_INCLUDE_DIR} ${FMUSDK_INCLUDE_DIR} ${MATIO_INCLUDE_DIR}
                           ${MATCMP_INCLUDE_DIR} ${GTEST_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIR})

include_directories(PRIVATE "include")

set(LINK_LIBRARIES ${MATIO_LIBRARIES} ${NETWORK_OFFLOADER_LIBRARY} ${FMILIB_LIBRARIES} ${LAPACK_LIBRARIES}
                   ${Boost_FILESYSTEM_LIBRARY} ${Boost_LIBRARIES} ${MPI_LIBRARIES} "dl" "expat")

add_executable(ParallelFmu ${SRCS} ${NETWORK_SRCS} ${FMUSDK_SRCS} "src/Main.cpp")
target_link_libraries(ParallelFmu ${LINK_LIBRARIES})
install(TARGETS ParallelFmu DESTINATION "bin")


# Adding testsuit if possible
if(BUILD_PARALLELFMU_TEST)
  #compile FMUs
  message(STATUS "Building FMUs for testing")
  if(NOT (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/test/data/BouncingBall.fmu") OR
                 "${CMAKE_CURRENT_SOURCE_DIR}/test/data/BouncingBall.mos" IS_NEWER_THAN
                 "${CMAKE_CURRENT_BINARY_DIR}/test/data/BouncingBall.fmu")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test_tmp")
    execute_process(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/BouncingBall.mos" WORKING_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    if(OMC_RESULT)
      message(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    else(OMC_RESULT)
      message(STATUS "Built BouncingBall.fmu")
    endif(OMC_RESULT)

    execute_process(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/SimpleView.mos" WORKING_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    if(OMC_RESULT)
      message(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    else(OMC_RESULT)
      message(STATUS "Built SimpleView.fmu")
    endif(OMC_RESULT)

    execute_process(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/Blower.mos" WORKING_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    if(OMC_RESULT)
      message(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    else(OMC_RESULT)
      message(STATUS "Built Blower.fmu")
    endif(OMC_RESULT)

    execute_process(COMMAND "${OMC_COMPILER}" "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/BouncingBall_gravity_input.mos" WORKING_DIRECTORY
                    "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data" RESULT_VARIABLE OMC_RESULT OUTPUT_VARIABLE OMC_ERROR)
    if(OMC_RESULT)
      message(FATAL_ERROR "Couldn't build test fmus: ${git_ver}")
    else(OMC_RESULT)
      message(STATUS "Built BouncingBall_gravity_input.fmu")
    endif(OMC_RESULT)

    file(GLOB_RECURSE XML_FMU_BINARIES "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/*.fmu"
         "${CMAKE_CURRENT_BINARY_DIR}/test_tmp/data/*.xml")
    file(COPY ${XML_FMU_BINARIES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test/data")
    file(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/test_tmp")
  endif()

  # Add executable and files
  include_directories("include" "test/include" ${NETWORK_INCLUDES} ${MPI_C_INCLUDE_PATH} ${MATIO_INCLUDE_DIR}
                      ${FMILIB_INCLUDE_DIR} ${FMUSDK_INCLUDE_DIR} ${NETWORK_OFFLOADER_INCLUDE_DIR}
                      ${MATCMP_INCLUDE_DIR} ${GTEST_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIR})
  add_executable(testParallelFmu ${SRCS} ${NETWORK_SRCS} ${FMUSDK_SRCS} ${MATCMP_SRCS} "test/Main.cpp")
  target_link_libraries(testParallelFmu ${LINK_LIBRARIES} "gtest")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test_tmp")
endif(BUILD_PARALLELFMU_TEST)


#INSTALL (DIRECTORY "data" DESTINATION ".")

# ---------------------------
# Build Precompiled Header
# ---------------------------

#INCLUDE("${CMAKE_CURRENT_LIST_DIR}/cmake/PrecompiledHeader.cmake")
#add_precompiled_header(test_fmu "include/Stdafx.hpp" SOURCE_CXX "src/Stdafx.cpp")
#add_precompiled_header(test_fmu_test "include/Stdafx.hpp" SOURCE_CXX "src/Stdafx.cpp")
