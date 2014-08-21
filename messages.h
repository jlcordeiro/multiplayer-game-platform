#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>
#include <json11/json11.hpp>
using json11::Json;
using namespace std;

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
    static const Json::shape shape;

    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const string& room_name, const string& user_name)
    {
        return user_room_message(tag, room_name, user_name);
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

    static string str(const string& room_name, const string& user_name)
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

class Var
{
public:
    static const string tag;
    static const Json::shape shape;
    static const string to_tag;
    static const string from_tag;

    static bool validate(const string& msg)
    {
        return has_shape(msg, shape);
    }

    static string str(const string& from, const string& to, const string& name, const string& value)
    {
        return Json(Json::object({{tag, Json::object{{from_tag, from},
                                                     {to_tag, to},
                                                     {name, value}}
                                  }})).dump();
    }
};

}
#endif
