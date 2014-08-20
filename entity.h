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

class Communication
{
public:
    virtual int send(const string& msg) = 0;
    virtual int recv(vector<string>& recv_messages) = 0;
};

class NoCommunication : public Communication
{
public:
    int send(const string& msg) override {return 0;}
    int recv(vector<string>& recv_messages) override {return 0;}
};

class TCPCommunication : public Communication
{
private:
    TCPClient& _socket;
public:
    TCPCommunication(TCPClient& s) : _socket(s) {}
    int send(const string& msg) override {return _socket.send(msg);}
    int recv(vector<string>& recv_messages) override {return _socket.recv(recv_messages);}
};


class Entity : public enable_shared_from_this<Entity>
{
public:
    typedef function<void(Entity& sender, const std::string& buffer)> Visitor;

    Entity(shared_ptr<Communication> c, long unsigned int max_relations)
        : _id(_id_current++),
          _fd(-1),
          _name(""),
          _communication(c),
          _max_relations(max_relations),
          _relation_count(0)
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
        }
    }

    bool containsRelation(shared_ptr<Entity> e) const
    {
        return (_relations.find(e->getId()) != _relations.end());
    }

    void removeRelation(shared_ptr<Entity> e)
    {
        if (containsRelation(e)) {
            _relations.erase(e->getId());
            _relation_count--;
        }
    }

    shared_ptr<Entity> getRelationByName(const string& name)
    {
        return findByName<Entity>(_relations, name);
    }

    map<int, shared_ptr<Entity>>& getRelations()
    {
        return _relations;
    }

    void acceptMessageVisitor(Visitor visitor)
    {
        _message_visitors.push_back(visitor);
    }

    void print();

    void dispatch();

    void joinRoom(const string& name)
    {
        auto comm = shared_ptr<Communication>(new NoCommunication());
        auto room = shared_ptr<Entity>(new Entity(comm, 1000));
        room->setName(name);

        addRelation(room);
        room->addRelation(this->shared_from_this());

        _communication->send(protocol::Join::str(name, getName()));
    }

private:
    static unsigned long _id_current;

    unsigned long _id;
    int _fd;
    string _name;

    map<string,string> _variables;

    shared_ptr<Communication> _communication;

    map<int, shared_ptr<Entity>> _relations;
    long unsigned int _max_relations;
    long unsigned int _relation_count;

    vector<Visitor> _message_visitors;
};

void handleJoin(Entity& room, const std::string& buffer);
void handleQuit(Entity& room, const std::string& buffer);
shared_ptr<Entity> getRoom(Entity& e);

#endif
