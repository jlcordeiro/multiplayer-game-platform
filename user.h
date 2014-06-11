#ifndef __USER_H__
#define __USER_H__

#include <string>
#include <map>
using namespace std;

class User
{
    public:
        User(long int id, string name);
        long int getId() const;
        string getName() const;

        string getVariable(string name) const;
        void setVariable(string name, string value);
        bool containsVariable(string name) const;
        void removeVariable(string name);

    protected:

        long int _id;
        string _name;
        map<string,string> _variables;
};

#endif
