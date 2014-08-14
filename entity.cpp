#include "entity.h"

unsigned long Entity::_id_current = 0;

long int Entity::getRelationCount() const
{
    return _relation_count;
}

long int Entity::getMaxRelations() const
{
    return _max_relations;
}

void Entity::setMaxRelations(long int value)
{
    _max_relations = value;
}

void Entity::addRelation(shared_ptr<Entity> user)
{
    if (containsRelation(user)) {
        return;
    }

    _relations[user->getId()] = user;
    _relation_count++;

    auto str = protocol::Join::reply(user->getName(), getName());
    for (auto u : _relations) {
        send(u.first, str);
    }
}

bool Entity::containsRelation(shared_ptr<Entity> user) const
{
    return (_relations.find(user->getId()) != _relations.end());
}

void Entity::removeRelation(shared_ptr<Entity> user)
{
    if (!containsRelation(user)) {
        return;
    }

    auto str = protocol::Quit::str(user->getName());
    for (auto u : _relations) {
        send(u.first, str);
    }

    _relations.erase(user->getId());
    _relation_count--;
}

shared_ptr<Entity> Entity::getRelationById(long int id)
{
    auto it = _relations.find(id);

    if (it == _relations.end()) {
        return shared_ptr<Entity>(NULL);
    }

    return it->second;
}

shared_ptr<Entity> Entity::getRelationByName(const string& name)
{
    return findByName<Entity>(_relations, name);
}

const map<int, shared_ptr<Entity> >& Entity::getRelations() const
{
    return _relations;
}
