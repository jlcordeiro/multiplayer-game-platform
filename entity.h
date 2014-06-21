#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include <json11/json11.hpp>
#include "tcpsocket.h"
using namespace std;

class Entity
{
    static unsigned long _id_current;

public:
    Entity()
        : _id(_id_current++),
        _name("")
    {
    }

    int getFd() const {
        return _fd;
    }

    void setFd(int value) {
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

#endif
