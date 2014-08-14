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
    Entity(EntityType type, long unsigned int max_relations)
        : _id(_id_current++),
          _name(""),
          _type(type),
          _max_relations(max_relations),
          _relation_count(0)
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

    void _setVariable(const string& name, const string& value)
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

    long int getRelationCount() const;
    long int getMaxRelations() const;
    virtual void setMaxRelations(long int value);

    void addRelation(shared_ptr<Entity> e);
    bool containsRelation(shared_ptr<Entity> e) const;
    void removeRelation(shared_ptr<Entity> e);
    shared_ptr<Entity> getRelationById(long int id);
    shared_ptr<Entity> getRelationByName(const string& name);
    const map<int, shared_ptr<Entity> >& getRelations() const;


protected:
    int _fd;
    unsigned long _id;
    string _name;
    map<string,string> _variables;

    EntityType _type;

    map<int, shared_ptr<Entity> > _relations;
    long unsigned int _max_relations;
    long unsigned int _relation_count;
};

template<class T>
shared_ptr<T> findByName(const map<int,shared_ptr<T>>& group, string name)
{
    for (auto entity_pair : group) {
        auto entity = entity_pair.second;
        if (entity && entity->getName() == name) {
            return entity;
        }
    }

    return nullptr;
}

template<class T>
void handleVariable(shared_ptr<T> destiny, json11::Json object)
{
    string name = object["name"].string_value();
    string value = object["value"].string_value();
    destiny->_setVariable(name, value);
}

template<class T>
void handleVariable(const map<int,shared_ptr<T>>& group, json11::Json object)
{
//     string user_name;
//     
//     if (T::type == USER) {
//         user_name = object["user"].string_value();
//     } else {
//         user_name = object["room"].string_value();
//     }
// 
//     handleVariable(findByName<T>(group, user_name), object);
}

#endif
