#include "message.h"
#include "json/json.h"
#include <sstream>
#include <string.h>

DecisionMessage::DecisionMessage()
{

}

DecisionMessage::DecisionMessage(string ac, string dec)
{
    setDecision(ac, dec);
}

DecisionMessage::DecisionMessage(string ac, const char* dec)
{
    setDecision(ac, dec);
}

void DecisionMessage::setDecision(string ac, const char* dec)
{
    this->dec = dec;
    this->account = ac;
}

void DecisionMessage::setDecision(string ac, string dec)
{
    this->dec = dec;
    this->account = ac;
}

void DecisionMessage::toJson()
{
    dec_json["account"] = Json::Value(account );
    dec_json["dec"]     = Json::Value(dec     );

}

void DecisionMessage::fromJson()
{
    dec             = dec_json["dec"].asString();
    account         = dec_json["account"].asString();
}

void DecisionMessage::encodeMessage()
{
    toJson();

    std::ostringstream stream;

    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // No identation for message encoding
    std::string document = Json::writeString(wbuilder, dec_json) + "\n\n\0\0";
    message_c = document;
    message = message_c.c_str();
}

void DecisionMessage::decodeMessage()
{
    istringstream stream(message);
    stream >> dec_json;
    fromJson();
}

Message::Message()
{
}

Message::Message(const char *buf)
{
    fromBuffer(buf);
}

void Message::fromBuffer(const char *buf)
{
    message = buf;
}

void Message::encodeMessage()
{
    toJson();
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // No identation for message encoding
    message_c = Json::writeString(wbuilder, root) + "\n\n\0\0";
    message = message_c.c_str();
    //printf("Msg : %s\n", message);
}

void Message::decodeMessage()
{
    istringstream stream(message);
    stream >> root;
    //cout << root << endl;
    fromJson();
}

void Message::toJson()
{
    root["type"]        = Json::Value(type);
    root["content"]     = Json::Value(content);
}

void Message::fromJson()
{
    type            = root["type"].asInt();
    content         = root["content"].asString();
}