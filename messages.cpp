#include "messages.h"

namespace protocol
{

using namespace std;
using namespace json11;

const string Name::tag = "name";
const string Join::tag = "join";
const string Quit::tag = "quit";
const string MaxUsers::tag = "maxUsers";
const string RVar::tag = "rvar";
const string UVar::tag = "uvar";

const Json::shape Name::shape = {make_pair(Name::tag, Json::STRING)};
const Json::shape Join::request_shape = {make_pair(Join::tag, Json::STRING)};
const Json::shape Join::reply_shape = {make_pair(Join::tag, Json::OBJECT)};
const Json::shape Quit::request_shape = {make_pair(Quit::tag, Json::STRING)};
const Json::shape Quit::reply_shape = {make_pair(Quit::tag, Json::OBJECT)};
const Json::shape MaxUsers::shape = {make_pair(MaxUsers::tag, Json::NUMBER)};
const Json::shape RVar::shape = {make_pair(RVar::tag, Json::OBJECT)};
const Json::shape UVar::shape = {make_pair(UVar::tag, Json::OBJECT)};

}
