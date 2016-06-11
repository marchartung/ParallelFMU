
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

.PHONY: all build eclipse doc testFmus

all: testFmus build


testFmus:
	cd test/data && make build && make clean

build: testFmus
	mkdir -p build_tmp
	cd ./build_tmp && CC=$(CC) CXX=$(CXX) cmake -DBUILD_DOCUMENTATION=true -DFMILIB_HOME=$(FMILIB_HOME) -DRAPIDXML_ROOT=$(RAPIDXML_ROOT) -DCMAKE_INSTALL_PREFIX:PATH="../build" -DCMAKE_BUILD_TYPE=Debug ../
	make -C build_tmp VERBOSE=1
	make -C build_tmp install

eclipse: testFmus
	mkdir -p ../parallel_fmu_eclipse
	cd ../parallel_fmu_eclipse && CC=$(CC) CXX=$(CXX) cmake -DFMILIB_HOME=$(FMILIB_HOME) -DRAPIDXML_ROOT=$(RAPIDXML_ROOT) -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_CXX_COMPILER_ARG1=-std=c++11 -DCMAKE_BUILD_TYPE=Debug $(ROOT_DIR) -DCMAKE_INSTALL_PREFIX:PATH="$(ROOT_DIR)/build"

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
