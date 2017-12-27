PROJECT(chater)
cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(JSON_PATH ${PROJECT_SOURCE_DIR}/thirdparty/jsoncpp/)
SET(CRPTO_PATH ${PROJECT_SOURCE_DIR}/thirdparty/cryptopp565/)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CRPTO_PATH})

INCLUDE_DIRECTORIES(${JSON_PATH}/include)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/server SERVER_SRC)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/client CLIENT_SRC)
SET(EXECUTABLE_OUTPUT_PATH bin/)

MESSAGE(STATUS "Project in ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "Json in ${JSON_PATH}")

#ADD_EXECUTABLE(serverv1 ref/v1/server.c)
#ADD_EXECUTABLE(clientv1 ref/v1/client.c)
#ADD_EXECUTABLE(serverv2 ref/v2/server.c)
#ADD_EXECUTABLE(clientv2 ref/v2/client.c)

ADD_EXECUTABLE(servermain ${SERVER_SRC} ${CRPTO_PATH}/libcryptopp.a)
ADD_LIBRARY(server ${SERVER_SRC} ${CRPTO_PATH}/libcryptopp.a)
ADD_EXECUTABLE(clientmain ${CLIENT_SRC})
TARGET_LINK_LIBRARIES(clientmain ${JSON_PATH}/build/debug/src/lib_json/libjsoncpp.a server ${CRPTO_PATH}/libcryptopp.a)
TARGET_LINK_LIBRARIES(servermain ${JSON_PATH}/build/debug/src/lib_json/libjsoncpp.a ${CRPTO_PATH}/libcryptopp.a)

ADD_EXECUTABLE(test_crypto test.cpp)
TARGET_LINK_LIBRARIES(test_crypto ${CRPTO_PATH}/libcryptopp.a)