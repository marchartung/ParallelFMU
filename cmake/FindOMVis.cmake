
SET(OMVis_FOUND FALSE)

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/OMVis/include")
	SET(OMVis_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/OMVis")
	SET(OMVis_FOUND TRUE)
ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/OMVis/include")