#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include <json11/json11.hpp>
#include "tcpsocket.h"
#include "messages.h"
using namespace std;

enum EntityType { NONE, ROOM, USER };

class Entity
{
    static unsigned long _id_current;

public:
    static EntityType type;

    Entity()
        : _id(_id_current++),
        _name("")
    {
    }

    int getFd() const
    {
        return _fd;
    }

    void setFd(int value)
    {
        _fd = value;
    }

    long int getId() const
    {
        return _id;
    }

    string getName() const
    {
        return _name;
    }

    virtual void setName(string value)
    {
        _name = value;
    }

    string getVariable(string name) const
    {
        auto it = _variables.find(name);

        if (it == _variables.end()) {
            return "";
        }

        return it->second;
    }

    virtual void setVariable(const string& name, const string& value)
    {
        if (getVariable(name) != value) {
            _variables[name] = value;
        }
    }

    bool containsVariable(string name) const
    {
        return (_variables.find(name) != _variables.end());
    }

    void removeVariable(string name)
    {
        _variables.erase(name);
    }

    const map<string, string>& getVariables() const
    {
        return _variables;
    }

protected:
    int _fd;
    unsigned long _id;
    string _name;
    map<string,string> _variables;
};

template<class T>
shared_ptr<T> findByName(const map<int,shared_ptr<T>>& group, string name)
{
    for (auto entity : group) {
        if (entity.second->getName() == name) {
            return entity.second;
        }
    }

    return nullptr;
}

template<class T>
void handleVariable(const map<int,shared_ptr<T>>& group, json11::Json object)
{
    string user_name;
    
    if (T::type == USER) {
        user_name = object["user"].string_value();
    } else {
        user_name = object["room"].string_value();
    }

    string name = object["name"].string_value();
    string value = object["value"].string_value();

    auto destiny = findByName<T>(group, user_name);
    destiny->setVariable(name, value);
}

#endif
