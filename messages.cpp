#include "messages.h"

namespace protocol
{

using namespace std;
using namespace json11;

const string Name::tag = "name";
const string Join::tag = "join";
const string Quit::tag = "quit";
const string MaxUsers::tag = "maxUsers";

const Json::shape Name::shape = {make_pair(Name::tag, Json::STRING)};
const Json::shape Join::shape = {make_pair(Join::tag, Json::STRING)};
const Json::shape Quit::shape = {make_pair(Quit::tag, Json::STRING)};
const Json::shape MaxUsers::shape = {make_pair(MaxUsers::tag, Json::NUMBER)};

}
