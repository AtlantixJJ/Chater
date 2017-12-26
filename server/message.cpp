#include "message.h"

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
    ostringstream stream;
    stream << root;
    message = stream.str().c_str();
}

void Message::decodeMessage()
{
    istringstream stream(message);
    stream >> root;
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