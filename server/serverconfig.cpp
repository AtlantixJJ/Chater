#include "json/json.h"
#include "serverconfig.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

void ServerConfig::setFieldValueFromJson()
{
    server_ip       = root["server_ip"      ].asString();
    server_port     = root["server_port"    ].asInt();
    max_thr         = root["max_thr"        ].asInt();
    download_path   = root["download_path"  ].asString();
}

void ServerConfig::saveFieldValueToJson()
{
    root["server_ip"      ] = Json::Value(server_ip       );
    root["server_port"    ] = Json::Value(server_port     );
    root["max_thr"        ] = Json::Value(max_thr         );
    root["download_path"  ] = Json::Value(download_path   );
}

ServerConfig::ServerConfig(const ServerConfig &ref)
{
    fromObj(ref);
}

ServerConfig::ServerConfig(string fname)
{
    has_file = true;
    file_name = fname;
    ifstream fin(fname);
    fin >> root;
    setFieldValueFromJson();
}

ServerConfig::ServerConfig()
{
    server_ip       = "127.0.0.1";
    download_path   = "./downloads/";
    server_port     = 10222;
    max_thr         = 128;
    
    has_file = false;
}

ServerConfig::~ServerConfig()
{
    
}

void ServerConfig::fromObj(const ServerConfig &ref)
{
    server_ip       = ref.server_ip       ;
    server_port     = ref.server_port     ;
    max_thr         = ref.max_thr         ;
    download_path   = ref.download_path   ;
}

const ServerConfig& ServerConfig::operator=(const ServerConfig &ref)
{
    fromObj(ref);
    return *this;
}

string ServerConfig::toString() const
{
    ostringstream stream;
    stream << root;
    string tmp = "ServerConfig:\n" + stream.str();
    return tmp;
}

void ServerConfig::setFileName(string fname)
{
    file_name = fname;
    has_file = true;
}

bool ServerConfig::writeToFile()
{
    if (has_file == false)
    {
        return false;
    }
    saveFieldValueToJson();
    ofstream fout(file_name);
    fout << root;
    return true;
}