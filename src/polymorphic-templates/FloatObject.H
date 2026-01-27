#pragma once

#include "polymorphic-templates.H"
#include "macros.H"

CDECL(FloatObject);

class FloatObject : public ObjectBase<FloatObject> {
    float val;

public:
    FloatObject(Secret, float val) : val(val) {}


    void incrementImpl();
    void dumpImpl() const;
};
