#ifndef USERDATABASE_H_
#define USERDATABASE_H_

#include "json/json.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <string.h>
using namespace std;


class UserDataBase
{
public:
    UserDataBase();
    UserDataBase(std::string fname);

    void writeToFile();

private:
    void fromJson();
    void toJson();

private:
    std::vector<Json::Value> user_list;

    std::string file_name;

    Json::Value root;

};

#endif