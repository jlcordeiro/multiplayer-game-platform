#ifndef MESSAGES_H
#define MESSAGES_H

#include "user.h"
#include <string>
#include <json11/json11.hpp>
using json11::Json;

bool isVariable(const std::string& msg, std::string& err);

namespace protocol
{

static inline bool has_shape(const string& msg, Json::shape shape)
{
    string err;
    auto json = Json::parse(msg, err);
    return ((json != nullptr) && json.has_shape(shape, err));
}

class Name
{
public:
    static const string tag;
    static const Json::shape shape;

    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const string& s)
    {
        return Json(Json::object({{tag, s}})).dump();
    }

};

class Join
{
public:
    static const string tag;
    static const Json::shape shape;

    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const User& u)
    {
        return Json(Json::object({{tag, u.getName()}})).dump();
    }
};

class Quit
{
public:
    static const string tag;
    static const Json::shape shape;
    
    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const User& u)
    {
        return Json(Json::object({{tag, u.getName()}})).dump();
    }
};

class MaxUsers
{
public:
    static const string tag;
    static const Json::shape shape;
    
    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(int limit)
    {
        return Json(Json::object({{tag, limit}})).dump();
    }
};

class Variable
{
public:
    static bool validate(const string& msg)
    {
        return msg[0] == '#';
    }
};

}
#endif
