#include "AnyObject.H"
#include "FloatObject.H"
#include "IntObject.H"

#include <vector>
#include <algorithm>

int main() {
    std::shared_ptr<FloatObject> floatObject = FloatObject::createSP(1);
    FloatObject floatObjectOnStack = FloatObject::create(2);
    floatObject->increment();
    floatObject->dump();
    floatObjectOnStack.increment();
    floatObjectOnStack.dump();

    // AnyObject any = FloatObject::create(3);
    // any.increment();
    // any.dump();

    AnyObjectSP any = FloatObject::createSP(3);
    any.increment();
    any.dump();

    any = IntObject::createSP(4);
    any.increment();
    any.dump();

    std::vector<AnyObjectSP> manyObjects;
    manyObjects.push_back(FloatObject::createSP(2.3));
    manyObjects.push_back(IntObject::createSP(42));

    std::ranges::for_each(manyObjects,
        [&](auto& o)
        {
            o.increment();
            o.dump();
        }
    );

    return 0;
}