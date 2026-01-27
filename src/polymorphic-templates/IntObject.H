#pragma once

#include "polymorphic-templates.H"
#include "macros.H"

CDECL(IntObject);

class IntObject : public ObjectBase<IntObject> {
    int val;

public:
    IntObject(Secret, int val) : val(val) {}

    void incrementImpl();
    void dumpImpl() const;

};
