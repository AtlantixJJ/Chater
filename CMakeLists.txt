PROJECT(chater)
cmake_minimum_required(VERSION 3.0)
INCLUDE_DIRECTORIES(../include)   
AUX_SOURCE_DIRECTORY(../ DIR_SRCS)

SET(EXECUTABLE_OUTPUT_PATH bin/)

ADD_EXECUTABLE(serverv1 ref/v1/server.c)
ADD_EXECUTABLE(clientv1 ref/v1/client.c)
ADD_EXECUTABLE(serverv2 ref/v2/server.c)
ADD_EXECUTABLE(clientv2 ref/v2/client.c)