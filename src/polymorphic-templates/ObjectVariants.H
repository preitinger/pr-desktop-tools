#pragma once

#include "FloatObject.H"
#include "IntObject.H"

#include <variant>

using ObjectVariantsSP = std::variant<
    FloatObjectSP
    , IntObjectSP
>;
