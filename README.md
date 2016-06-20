/**\mainpage ParallelFMU - A scalable Framework for computing multiple FMUs in parallel.

\tableofcontents

\section what_sec What is ParallelFMU?
ParallelFMU is a framework for ...

ParallelFMU is developed by Marc Hartung and Martin Flehmig at TU Dresden. If you have questions 
concerning ParallelFMU or trouble using it, feel free to contact us via 
marc.hartung[at]tu-dresden.de.

ParallelFMU is open-source and released under GNU GENERAL PUBLIC LICENSE (v3.0). See the LICENSE 
file which is comes with ParallelFMU or visit http://www.gnu.org/licenses/gpl.html for further 
information.


\section install_sec Installation

\subsection mustDependencies Software Dependencies
ParallelFMU, like most other software projects, builds up on a variety of software packages. In 
the following we give a list of all necessary dependencies and the version number, which is 
required.

    0. CMake (>= 3.0.0)
        - https://cmake.org/
        - Linux: via Package Manager or build from source
        - Windows: Install binary

    1. C++ Compiler
        - Supporting C++ standard 2011 (-std=c++11)

    2. FMU SDK (?)
        - https://www.qtronic.de/de/fmusdk.html
        - Is "shipped" as thirdparty
        - Linux: 
        - Windows: Download from website (s.a.)

    3. matio Library (BSD)
        - https://sourceforge.net/projects/matio/
        - Linux: via Package Manager (libmatio-dev) or build from source
        - Windows: ?

    4. Boost C++ Libraries (Boost Software License)
        - http://www.boost.org/
        - Subset of Boost Libraries: program_options, filesystem, system
        - Linux: via Package Manager
            --> libboost-filesystem-dev
            --> libboost-program-options-dev
            --> libboost-system-dev
            --> libboost-dev
        - Windows: build from source

     5. Lapack Implementation

\subsubsection canDependencies Additional Sofwarte Dependencies
In order to get the full support for parallel computation of ParallelFMU, there are additional 
software dependencies.

    6. MPI implementation

    7. OpenMP
      - C++ compiler supporting OpenMP (,e.g., via -fopenmp or -openmp)
      - OpenMP runtime (,e.g., GNU libgomp or Intel OpenMP runtime)

    8. OpenModelica Compiler (OSMC-PL, GPL v3.0)
      - https://github.com/OpenModelica/OMCompiler
      - The OMC is used to build the test FMUs. Of course, this can be also done with an other Modelica Compiler.

    9. FMI Library (>= 2.0) (BSD)
        - Replaced by FMU SDK and therefore not supported anymore
        - http://www.jmodelica.org/FMILibrary
        - Linux: Download sources, unzip, read FMILIB_Readme.txt, configure, make, install
        - Windows: download binaries
        - Its important to remember where FMIlibary is installed to. The installation path has to 
          be provided to the CMake file of OMVis.


\subsection configure Configure and Build using CMakefile
\code
~> mkdir build
~> cd build
~> cmake -DFMILIB_HOME=/PATH/TO/FMILIB2/ -DMATIO_HOME=/PATH/TO/MATIO
~> make 
\endcode

\subsubsection environment Environment Settings / Configure Options
It might be neccessary to provide one or all of the following environment settings to the CMake 
makefile of OMVis:

As usual, the C/C++ compiler and the compiler flags can be controlled via the environment variables 
CC=<cc>, CXX=<cxx>, CFLAGS=<> and CXXFLAGS=<cxxflags>.

More to come.

FMILIB_HOME=/PATH/TO/FMILIB make
MATIO_HOME=/PATH/TO/MATIO

\subsubsection configure Configure and Build using Makefile
ParallelFmu is shipped with a Makefile. We need to emphasise, that the Makefile might be not fully 
compliant with the CMake build process. Nevertheless, it might be useful and provides the following
targets:

  * testFmus: Builds the test FMUs
  * build: Builds ParallelFMU in the subdirectory ./build
  * eclipse: Builds ParallelFMU as Eclipse project in the directory ../parallel_fmu_eclipse. This is very useful for developers.
  * doc: Builds documentation.
  * clean-eclipse: Removes Eclipse project folder.

*/