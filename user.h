#ifndef __USER_H__
#define __USER_H__

#include <string>
#include <map>
using namespace std;

class User
{
    public:
        User(long int fd);
        long int getFd() const;
        string getName() const;
        void setName(string value);

        string getVariable(string name) const;
        void setVariable(string name, string value);
        bool containsVariable(string name) const;
        void removeVariable(string name);

    protected:

        long int _fd;
        string _name;
        map<string,string> _variables;
};

#endif
