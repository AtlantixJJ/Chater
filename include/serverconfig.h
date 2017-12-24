#ifndef SERVERCONFIG_H_
#define SERVERCONFIG_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include "json/json.h"
using namespace std;

class ServerConfig
{
public:
    ServerConfig();
    ServerConfig(const ServerConfig &ref);
    const ServerConfig& operator = (const ServerConfig &ref) ;
    ServerConfig(string fname);
    ~ServerConfig();

    string toString() const;

    void setFileName(string fname);
    void setValue(string key, Json::Value &val) {root[key] = val;}
    bool writeToFile();

    const char* getServerIP() const {return server_ip.c_str();}
    short getServerPort() const {return server_port;}
    int getMaxThread() const {return max_thr;}
private:
    void setFieldValueFromJson();
    void saveFieldValueToJson();
    void fromObj(const ServerConfig &ref);

    string temp;

    string server_ip;
    short server_port;
    int max_thr;

private:
    bool has_file;
    string file_name;
    Json::Value root;
};

#endif