#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <map>
#include <json11/json11.hpp>
#include "tcpsocket.h"
#include "messages.h"
using namespace std;

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
    destiny->setVariable(name, value);
}

template<class T>
void handleVariable(const map<int,shared_ptr<T>>& group, json11::Json object)
{
    auto from = findByName<T>(group, object["from"].string_value());
    handleVariable(from, object);
}

class NoCommunication
{
public:
    int send(const string& msg) {return 0;}
    int recv(vector<string>& recv_messages) {return 0;}
};

class EntityBase
{
public:
    typedef function<void(EntityBase& sender, const std::string& buffer)> Visitor;

    EntityBase() : _id(_id_current++), _name("") {};

    long int getId() const
    {
        return _id;
    }

    string getName() const
    {
        return _name;
    }

    int getFd() const
    {
        return _fd;
    }

    void setFd(int value)
    {
        _fd = value;
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

    void acceptMessageVisitor(Visitor visitor)
    {
        _message_visitors.push_back(visitor);
    }

    template <class Communication> friend class Entity;

private:
    static unsigned long _id_current;
    unsigned long _id;
    int _fd;
    string _name;
    map<string,string> _variables;

    vector<Visitor> _message_visitors;
};

template <class T> struct MessageVisitorInterface;

template <class Communication>
class Entity : public EntityBase, public enable_shared_from_this<Entity<Communication>>
{
private:
    shared_ptr<Communication> _communication;

    map<int, shared_ptr<Entity> > _relations;
    long unsigned int _max_relations;
    long unsigned int _relation_count;

public:
    Entity(shared_ptr<Communication> c, long unsigned int max_relations)
        : _communication(c),
          _max_relations(max_relations),
          _relation_count(0)
    {
    }

    void setName(string value)
    {
        _name = value;
        _communication->send(protocol::Name::str(value));
    }

    void setVariable(const string& name, const string& value)
    {
        auto it = _variables.find(name);
        if (it == _variables.end() || it->second != value) {
            _variables[name] = value;
            _communication->send(protocol::Var::str(getName(), name, value));
        }
    }

    long int getRelationCount() const
    {
        return _relation_count;
    }

    long int getMaxRelations() const
    {
        return _max_relations;
    }

    void setMaxRelations(long int value)
    {
        _max_relations = value;
        _communication->send(protocol::MaxUsers::str(value));
    }

    void addRelation(shared_ptr<Entity> e)
    {
        if (!containsRelation(e)) {
            _relations[e->getId()] = e;
            _relation_count++;

            auto str = protocol::Join::reply(e->getName(), getName());
            for (auto u : _relations) {
                send(u.first, str);
            }
        }
    }

    bool containsRelation(shared_ptr<Entity> e) const
    {
        return (_relations.find(e->getId()) != _relations.end());
    }

    void removeRelation(shared_ptr<Entity> e)
    {
        if (containsRelation(e)) {
            auto str = protocol::Quit::str(e->getName());
            for (auto u : _relations) {
                send(u.first, str);
            }

            _relations.erase(e->getId());
            _relation_count--;
        }
    }

    shared_ptr<Entity> getRelationByName(const string& name)
    {
        return findByName<Entity>(_relations, name);
    }

    const map<int, shared_ptr<Entity> >& getRelations() const
    {
        return _relations;
    }

    void print()
    {
        cout << " ------ " << endl;
        cout << "[" << _fd << "/" << getName() << "]" << endl;
        for (auto varp : getVariables()) {
            cout << "~ " << varp.first << " => " << varp.second << endl;
        }

        for (auto u : _relations) {
            cout << "<<< [R] " << u.second->getName() << endl;
            for (auto varp : u.second->getVariables()) {
                cout << "      - " << varp.first << " => " << varp.second << endl;
            }
        }
    }

    void dispatch() {
        while (1) {
            vector<string> new_messages;
            int res = _communication->recv(new_messages);
            if (res < 0) {
                return;
            }

            for (auto visitor : _message_visitors) {
                for (auto buffer : new_messages) {
                    visitor(*(this), buffer);
                }
            }

            print();
            sleep(1);
        }
    }

    void joinRoom(const string& name)
    {
        _communication->send(protocol::Join::request(name));

//         auto room = shared_ptr<NEntity>(new NEntity(ROOM, 1000));
//         room->setName(name);
// 
//         addRelation(room);
//         room->addRelation(this->shared_from_this());
    }
};

typedef Entity<TCPClient> SEntity;
typedef Entity<NoCommunication> NEntity;

void printMessage(EntityBase& sender, const std::string& buffer);
void handleJoin(EntityBase& sender, const std::string& buffer);

#endif
