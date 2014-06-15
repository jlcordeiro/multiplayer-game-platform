#include "messages.h"

static 
static const Json::shape set_name_shape = {std::make_pair("setName", json11::Json::STRING)};
static const Json::shape join_shape = {std::make_pair("join", json11::Json::STRING)};
static const Json::shape quit_shape = {std::make_pair("quit", json11::Json::STRING)};
static const Json::shape maxusers_shape = {std::make_pair("maxUsers", json11::Json::NUMBER)};

static bool has_shape(const std::string& msg, Json::shape shape, std::string& err)
{
    auto json = Json::parse(msg, err);
    return ((json != nullptr) && json.has_shape(shape, err));
}

bool isSetName(const std::string& msg, std::string& err)
{
    return has_shape(msg, set_name_shape, err);
}

bool isJoin(const std::string& msg, std::string& err)
{
    return has_shape(msg, join_shape, err);
}

bool isQuit(const std::string& msg, std::string& err)
{
    return has_shape(msg, quit_shape, err);
}

bool isMaxUsers(const std::string& msg, std::string& err)
{
    return has_shape(msg, maxusers_shape, err);
}

bool isVariable(const std::string& msg, std::string& err)
{
    return msg[0] == '#';
}

Json getUserJoin(const User& u)
{
    return Json(Json::object({{"UserJoin", u.getName()}}));
}

Json getUserQuit(const User& u)
{
    return Json(Json::object({{"UserQuit", u.getName()}}));
}

Json setMaxUsers(int limit)
{
    return Json(Json::object({{"maxUsers", (int)limit}}));
}
