#ifndef __VERBOSE_OBJECT_H__
#define __VERBOSE_OBJECT_H__

#include <string>
#include <iostream>

class VerboseObject
{
    public:
    VerboseObject(std::string&& name) : name(std::move(name))
    {
        std::cout << "Constructing " << name << "\n";
    }
    virtual ~VerboseObject()
    {
        std::cout << "Destructing " << name << "\n";
    }

    private:
    std::string name;
};

#endif