#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "json/json.h"
#include "common.h"
#include <sstream>

class Message
{
public:
    Message();
    Message(const char* buf);

    void fromBuffer(const char *buf);

    void encodeMessage();
    void decodeMessage();

public:
    int type;
    std::string content;

    /// TODO: should not be public
    const char* message;
    Json::Value root;
private:
    void toJson();
    void fromJson();
};

#endif