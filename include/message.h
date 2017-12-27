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
protected:
    std::string message_c;
    void toJson();
    void fromJson();
};

class DecisionMessage : public Message
{
public:
    DecisionMessage();
    DecisionMessage(string ac, string dec);
    DecisionMessage(string ac, const char* dec);

    void encodeMessage();
    void decodeMessage();

    void setDecision(string ac, string dec);
    void setDecision(string ac, const char* dec);
    string account;
    string dec;

protected:
    void toJson();
    void fromJson();

protected:
    Json::Value dec_json;

};

#endif