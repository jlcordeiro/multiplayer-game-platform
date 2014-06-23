#ifndef MESSAGES_H
#define MESSAGES_H

#include "entity.h"
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

static string user_room_message(const string& tag,
                                const string& room_name,
                                const string& user_name)
{
    return Json(Json::object({{tag, Json::object{{"user", user_name},
                                                 {"room", room_name}}
                              }})).dump();
}

class Join
{
public:
    static const string tag;
    static const Json::shape request_shape;
    static const Json::shape reply_shape;

    static bool validate_request(const string& msg)
    {
        return has_shape(msg, request_shape);
    }

    static bool validate_reply(const string& msg)
    {
        return has_shape(msg, reply_shape);
    }

    static string request(const string& room_name)
    {
        return Json(Json::object({{tag, room_name}})).dump();
    }

    static string reply(const string& room_name, const string& user_name)
    {
        return user_room_message(tag, room_name, user_name);
    }

};

class Quit
{
public:
    static const string tag;
    static const Json::shape request_shape;
    static const Json::shape reply_shape;
    
    static bool validate_request(const string& msg)
    {
        return has_shape(msg, request_shape);
    }

    static bool validate_reply(const string& msg)
    {
        return has_shape(msg, reply_shape);
    }

    static string str(const Entity& u)
    {
        return Json(Json::object({{tag, u.getName()}})).dump();
    }

    static string reply(const string& room_name, const string& user_name)
    {
        return user_room_message(tag, room_name, user_name);
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

class RVar
{
public:
    static const string tag;
    static const Json::shape shape;

    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const string& room_name, const string& name, const string& value)
    {
        return Json(Json::object({{tag, Json::object{{"room", room_name},
                                                     {"name", name},
                                                     {"value", value}}
                                  }})).dump();
    }
};

class UVar
{
public:
    static const string tag;
    static const Json::shape shape;

    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const string& room_name, const string& name, const string& value)
    {
        return Json(Json::object({{tag, Json::object{{"user", room_name},
                                                     {"name", name},
                                                     {"value", value}}
                                  }})).dump();
    }
};

}
#endif
