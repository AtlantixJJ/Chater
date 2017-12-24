PROJECT(chater)
cmake_minimum_required(VERSION 3.0)
INCLUDE_DIRECTORIES(../include)   
AUX_SOURCE_DIRECTORY(../ DIR_SRCS)

ADD_EXECUTABLE(../bin/serverv1 ref/v1/server.cpp)
ADD_EXECUTABLE(../bin/clientv1 ref/v1/client.cpp)