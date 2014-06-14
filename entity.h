#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include "tcpsocket.h"
using namespace std;

class Entity
{
    static unsigned long _id_current;

public:
    Entity(long int fd)
        : _id(_id_current++),
        _fd(fd),
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

    int send(const string& msg) const
    {
        return ::send(_fd, msg);
    }

    void setName(string value)
    {
        _name = value;
    }

    string getVariable(string name) const
    {
        std::map<string,string>::const_iterator it = _variables.find(name);

        if (it == _variables.end()) {
            return NULL;
        }

        return it->second;
    }

    void setVariable(string name, string value)
    {
        _variables[name] = value;
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
    unsigned long _id;
    long _fd;
    string _name;
    map<string,string> _variables;
};

#endif
