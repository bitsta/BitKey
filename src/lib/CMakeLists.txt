FIND_PACKAGE(JSONCPP REQUIRED)
FIND_PACKAGE(JSONRPCCPP REQUIRED)
FIND_PACKAGE(CURL REQUIRED)

# Find header and source files
FILE(GLOB librootcoin_header ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB librootcoin_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Set target libraries
ADD_LIBRARY(librootcoin SHARED ${librootcoin_source})
ADD_LIBRARY(librootcoin_static STATIC ${librootcoin_source})

# Set output name of static library
SET_TARGET_PROPERTIES(librootcoin_static PROPERTIES OUTPUT_NAME librootcoin)

# Link the library with json-rpc-cpp libs
TARGET_LINK_LIBRARIES(librootcoin
                        ${CURL_LIBRARY}
                        ${JSONCPP_LIBRARY}
                        jsonrpccpp-common
                        jsonrpccpp-client)

TARGET_LINK_LIBRARIES(librootcoin_static
                        ${CURL_LIBRARY}
                        ${JSONCPP_LIBRARY}
                        jsonrpccpp-common
                        jsonrpccpp-client)

# Set version settings
SET(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
SET_TARGET_PROPERTIES(librootcoin librootcoin_static PROPERTIES
    VERSION "${VERSION_STRING}"
    SOVERSION "${SO_VERSION}")

# Install to appropriate destinations
INSTALL(FILES ${librootcoin_header} DESTINATION include/librootcoin)

INSTALL(TARGETS librootcoin librootcoin_static
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
