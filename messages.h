#ifndef MESSAGES_H
#define MESSAGES_H

#include "user.h"
#include <string>
#include <json11/json11.hpp>
using json11::Json;

bool isSetName(const std::string& msg, std::string& err);
bool isJoin(const std::string& msg, std::string& err);
bool isQuit(const std::string& msg, std::string& err);
bool isMaxUsers(const std::string& msg, std::string& err);

Json getUserJoin(const User& u);
Json getUserQuit(const User& u);

#endif
