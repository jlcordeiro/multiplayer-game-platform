#include "messages.h"

static const Json::shape set_name_shape = {std::make_pair("setName", json11::Json::STRING)};
static const Json::shape join_shape = {std::make_pair("join", json11::Json::STRING)};

bool isSetName(const std::string& msg, std::string& err)
{
    auto json = Json::parse(msg, err);
    return ((json != nullptr) && json.has_shape(set_name_shape, err));
}

bool isJoin(const std::string& msg, std::string& err)
{
    auto json = Json::parse(msg, err);
    return ((json != nullptr) && json.has_shape(join_shape, err));
}
