

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/NetworkOffloader/README.md")
    SET(NetworkOffloader_FOUND TRUE)
    SET(NetworkOffloader_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/NetworkOffloader/include")
ELSE()
    SET(NetworkOffloader_FOUND FALSE)
ENDIF()
    
