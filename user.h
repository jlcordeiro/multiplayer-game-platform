#ifndef __USER_H__
#define __USER_H__

#include "entity.h"

class User : public Entity
{
public:
    User(long int fd)
        : Entity(fd)
    {
    }
};

#endif
