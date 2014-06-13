#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>
#include <json11/json11.hpp>
using json11::Json;

bool isSetName(const std::string& msg, std::string& err);
bool isJoin(const std::string& msg, std::string& err);

// class UserJoin {
// private:
//     int _id;
//     string _name;
// 
// public:
//     UserJoin(User u) : _id(u.getFd()), _name(u.getName()) {}
// 
//     Json to_json() const { return Json::object ({{"UserJoin", Json::object({{"id", _id}, {"name", _name}})}}); }
// };

#endif
