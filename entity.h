#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include <json11/json11.hpp>
#include "tcpsocket.h"
using namespace std;

typedef json11::Json Variable;

class Entity
{
    static unsigned long _id_current;

public:
    Entity()
        : _id(_id_current++),
        _name("")
    {
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

    Variable getVariable(string name) const
    {
        auto it = _variables.find(name);

        if (it == _variables.end()) {
            return nullptr;
        }

        return it->second;
    }

    void setVariable(string name, Variable value)
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

    const map<string, Variable>& getVariables() const
    {
        return _variables;
    }

protected:
    unsigned long _id;
    string _name;
    map<string,Variable> _variables;
};

#endif
