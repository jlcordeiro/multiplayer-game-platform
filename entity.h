#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <map>
#include <json11/json11.hpp>
#include "tcpsocket.h"
#include "messages.h"
#include "relatives.h"
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
    for (auto o : object.object_items()) {
        auto var_name = o.first;
        if (var_name != protocol::Var::to_tag &&
                var_name != protocol::Var::from_tag) {
            string value = object[o.first].string_value();
            destiny->setVariable(o.first, value);
        }
    }
}

template<class T>
void handleVariable(const map<int,shared_ptr<T>>& group, json11::Json object)
{
    auto to = findByName<T>(group, object[protocol::Var::to_tag].string_value());
    if (to) {
        handleVariable(to, object);
    }
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
          _relatives(max_relations)
    {
    }

    Entity(long unsigned int max_relations)
        : Entity(shared_ptr<Communication>(new NoCommunication()), max_relations)
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
            _communication->send(protocol::Var::str(getName(), getName(), name, value));
        }
    }

    void acceptMessageVisitor(Visitor visitor)
    {
        _message_visitors.push_back(visitor);
    }

    void print();

    void dispatch();

    void joinRoom(const string& name)
    {
        auto room = shared_ptr<Entity>(new Entity(1000));
        room->setName(name);

        _relatives.add(room);

        _communication->send(protocol::Join::str(name, getName()));
    }

    void setMaxRelatives(long int value)
    {
        _relatives.limit(value);
        _communication->send(protocol::MaxUsers::str(value));
    }

    Relatives<Entity>& relatives()
    {
        return _relatives;
    }

    void broadcast(const string& msg) const
    {
        send(_fd, msg);
        for (auto pair : _relatives.get()) {
            send(pair.second->getFd(), msg);
        }
    }

private:
    static unsigned long _id_current;

    unsigned long _id;
    int _fd;
    string _name;

    map<string,string> _variables;

    shared_ptr<Communication> _communication;

    vector<Visitor> _message_visitors;

    Relatives<Entity> _relatives;
};

void handleJoin(Entity& room, const std::string& buffer);
void handleQuit(Entity& room, const std::string& buffer);
shared_ptr<Entity> getRoom(Entity& e);

#endif
