
SET(GTEST_FOUND FALSE)
SET(GTEST_SOURCE_DIR "")
SET(GTEST_INCLUDE_DIRS "")
SET(GTEST_BOTH_LIBRARIES "")
SET(GTEST_LIBRARIES "")
SET(GTEST_MAIN_LIBRARIES "")

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/")
	SET(GTEST_FOUND TRUE)
	SET(GTEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/")
#ELSE(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/")
	#FIND_PACKAGE(GTest)
ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/")
