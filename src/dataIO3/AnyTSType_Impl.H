#pragma once

#include "AnyTSType.H"
#include "macros.H"

#include "Literal.H"
#include "Reference.H"
#include "AttributeList.H"
#include "Union.H"
#include "Array.H"

#include <string>
#include <utility>

// DISPATCH_CSP_METHOD_IMPL(AnyTSTypeSP, toString)
// DISPATCH_CSP_METHOD_IMPL(AnyTSTypeSP, genType)

// std::string AnyTSTypeSP::toString() const {
//     return std::visit(overloads{
//        [&](const auto& node) {
//            return node->toString(/* std::forward<Args>(args)... */);
//        }
//         });
// }

#ifndef MANUAL_IMPLEMENTATION

// template <class... Args>
// auto AnyTSTypeSP::genType(Args&&... args) const {
//     return std::visit(
//         overloads
//         {
//         [&](const auto& node) {
//        return node->genType(std::forward<Args>(args)...);
//    } }, *this
//     );
// }

DISPATCH_CSP_METHOD_IMPL(AnyTSTypeSP, toString)
DISPATCH_CSP_METHOD_IMPL(AnyTSTypeSP, genType)

#endif