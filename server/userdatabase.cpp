#include "userdatabase.h"
#include <fstream>

using namespace std;

UserDataBase::UserDataBase()
{
}

UserDataBase::UserDataBase(std::string fname)
{
    file_name = fname;
    fromJson();
}

void UserDataBase::fromJson()
{
    user_list.clear();
    for (auto item : root)
        user_list.push_back(item);
}

void UserDataBase::toJson()
{
    for (int i = 0; i < user_list.size(); i++)
    {
        string s;
        char temp[32];
        sprintf(temp, "%d", i);
        root[s=temp] = user_list[i];
    }
}

void UserDataBase::writeToFile()
{
    toJson();
    std::ofstream fout(file_name);
    fout << root;
}
