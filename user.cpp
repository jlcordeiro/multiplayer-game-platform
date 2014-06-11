#include "user.h"

User::User(long int id, string name)
    : _id(id), _name(name)
{
}

long int User::getId() const
{
    return _id;
}

string User::getName() const
{
    return _name;
}

bool User::containsVariable(string name) const
{
    return (_variables.find(name) != _variables.end());
}

string User::getVariable(string name) const
{
    std::map<string,string>::const_iterator it = _variables.find(name);

    if (it == _variables.end()) {
        return NULL;
    }

    return it->second;
}

void User::setVariable(string name, string value)
{
    _variables[name] = value;
}

void User::removeVariable(string name)
{
    _variables.erase(name);
}
