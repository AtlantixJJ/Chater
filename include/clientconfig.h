#ifndef CLIENTCONFIG_H_
#define CLIENTCONFIG_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include "json/json.h"
using namespace std;

class ClientConfig
{
public:
    string name;
    string account;
    string passwd;

public:
    ClientConfig();
    ClientConfig(string fname);

    bool writeToFile();
    const char* getName()const{return name.c_str();}

private:
    void setFieldValueFromJson();
    void saveFieldValueToJson();

private:
    string file_name;

    Json::Value root;
};

#endif