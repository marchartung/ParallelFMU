
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

.PHONY: all build eclipse doc parallelFmu xeonPhi

all: parallelFmu build

parallelFmu:
	cd test/data && make build && make clean
build: parallelFmu
	mkdir -p build_tmp
	cd ./build_tmp && CC=$(CC) CXX=$(CXX) cmake -DBUILD_DOCUMENTATION=true -DFMILIB_HOME=$(FMILIB_HOME) -DMATIO_HOME=$(MATIO_HOME) -DRAPIDXML_ROOT=$(RAPIDXML_ROOT) -DCMAKE_INSTALL_PREFIX:PATH="../build" -DCMAKE_BUILD_TYPE=Debug ../
	make -C build_tmp VERBOSE=1
	make -C build_tmp install

xeonPhi:
	mkdir -p xeonPhi_build
	cd ./xeonPhi_build && CC=icc CXX=icpc FC=ifort CFLAGS="-mmic" CXXFLAGS=$CFLAGS FFLAGS=$CFLAGS MPI_C=mpiicc MPI_CXX=mpiicpc cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/toolchain_xeon_phi.cmake" -DFMILIB_HOME=$(FMILIB_HOME) -DMATIO_HOME=$(MATIO_HOME) -DRAPIDXML_ROOT=$(RAPIDXML_ROOT) -DCMAKE_INSTALL_PREFIX:PATH="../xeonPhi_build" -DCMAKE_BUILD_TYPE=RELEASE ../
	make -C xeonPhi_build VERBOSE=1	
	
eclipse-omp: parallelFmu
	mkdir -p ../parallel_fmu_eclipse-omp
	cd ../parallel_fmu_eclipse-omp && CC=$(CC) CXX=$(CXX) CFLAGS="-g"  cmake -DFMILIB_HOME=$(FMILIB_HOME) -DMATIO_HOME=$(MATIO_HOME) -DUSE_OPENMP=true -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug $(ROOT_DIR)

eclipse: parallelFmu
	mkdir -p ../parallel_fmu_eclipse
	cd ../parallel_fmu_eclipse && CC=$(CC) CXX=$(CXX) cmake -DFMILIB_HOME=$(FMILIB_HOME) -DMATIO_HOME=$(MATIO_HOME) -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug $(ROOT_DIR) -DCMAKE_INSTALL_PREFIX:PATH="$(ROOT_DIR)/build"

doc: build
	make -C build_tmp doc
	cp -r build_tmp/html build/doc

clean:
	rm -rf build
	rm -rf build_tmp
	rm -rf fmuTmp*

clean-eclipse:
	rm -rf ../parallel_fmu_eclipse

clean-eclipse-tmp:
	rm -rf ../parallel_fmu_eclipse/fmuTmp*

test: build
	cd build 
