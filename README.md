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

    2. FMU SDK (BSD)
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

\subsubsection canDependencies Additional Sofware Dependencies
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


\subsubsection usage Usage
To execute ParallelFMU:
\code
~> ParallelFMU -c /path/to/Simulation_config.xml
\endcode

Simulation_config.xml determines which FMUs should be simulated and how they are connected. Example:
\code
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
	<fmus>
		<fmu name="BouncingBall" path="test/data/BouncingBall.fmu" loader="fmuSdk" solver="euler" relativeTolerance="1.0e-5" />
		<fmu name="SimpleView" path="test/data/SimpleView.fmu" loader="fmuSdk" solver="euler" relativeTolerance="1.0e-5" defaultStepSize="0.1"/>
	</fmus>
	<connections>
		<connection source="BouncingBall" dest="SimpleView">
			<real out="0" in="0" />
		</connection>
	</connections>
	<writer>
		<csvFileWriter id="0" resultFile="result_simpleview.csv" numOutputSteps="100" />
	</writer>
	<scheduling>
		<nodes numNodes="1" numCoresPerNode="1" numFmusPerCore="2"/>
	</scheduling>
	<simulation startTime="0.0" endTime="5.0" globalTolerance="1.0e-5" globalMaxError="1.0e-6" globalDefaultStepSize="1.0e-3" globalEventInterval="2.0e-5"/>
</configuration>
\endcode
   * in fmus:
	* it's necessary to define at least one fmu-tag
	* the "name" attribute can be defined by the user and it should be unique in the simulation
	* "path" is the absolute or relative path to the FMU file
	* "solver" attribute defines which solver should be used for solving the fmu (in the moment only euler is available, and onle FMI1.0 me is supported.)
   * in connections
	* "connection" defines one output - input link between tow fmus.
	* for every variable connected the hast to be a variable tag
	* variable tags are "real", "bool", "int" (strings currently not supported)
	* the attributes "out" and "in" of a variable tag defines which variable reference of the source fmu is connected to which variable reference of the target fmu
  * in writer

  * in simulation 

*/
