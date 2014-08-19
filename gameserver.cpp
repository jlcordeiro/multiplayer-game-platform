#include "config.h"
#include "entity.h"
#include "tcpsocket.h"
#include <string>

static void handleJoin(Entity& sender, const std::string& buffer)
{
    string err;
    auto json = Json::parse(buffer, err);
    if (protocol::Join::validate_reply(buffer)) {
        string user_name = json[protocol::Join::tag]["user"].string_value();
        auto comm = shared_ptr<Communication>(new NoCommunication());
        auto user = shared_ptr<Entity>(new Entity(comm, 1));
        user->setName(user_name);
        sender.addRelation(user);
    }
}

static void handleQuit(Entity& sender, const std::string& buffer)
{
    string err;
    auto json = Json::parse(buffer, err);
    if (protocol::Quit::validate_reply(buffer)) {
        string user_name = json[protocol::Quit::tag]["user"].string_value();
        auto user = sender.getRelationByName(user_name);
        sender.removeRelation(user);
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        return 1;
    }

    TCPClient socket("localhost", atoi(Config::ROOM_PORT), false);
    auto comm = shared_ptr<Communication>(new TCPCommunication(socket));
    auto g = shared_ptr<Entity>(new Entity(comm, 10));

    auto print_buffer = [] (Entity& sender, const std::string& buffer)
                        { cout << "[" << sender.getName() << "] >> " << buffer << endl; };

    g->acceptMessageVisitor(print_buffer);
    g->acceptMessageVisitor(handleJoin);
    g->acceptMessageVisitor(handleQuit);

    g->setName(string(argv[1]));
    g->setMaxRelations(4);
    g->setVariable("var1", string(argv[2]));

    g->dispatch();
    return 0;
}
