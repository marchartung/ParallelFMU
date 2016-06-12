

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/NetworkOffloader/README.md")
    SET(NetworkOffload_FOUND TRUE)
ELSE()
    SET(NetworkOffload_FOUND FALSE)
ENDIF()
    
