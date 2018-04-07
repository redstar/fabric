//===-- json.h - JSON serialization -------*- C++ -*-===//
//
//                     Hyperledger Fabric
//
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
/// This file contains the interface of the JSON serializer.
///
//===----------------------------------------------------------------------===//

// License: Apache-2.0

#if !defined(FABRIC_UTILS_JSON_H)
#define FABRIC_UTILS_JSON_H

#include <string>

namespace fabric::utils {

// See https://stackoverflow.com/questions/17549906/c-json-serialization#answer-34165367

template<typename Class, typename T>
struct PropertyImpl {
    constexpr PropertyImpl(T Class::*aMember, const char* aName) : member{aMember}, name{aName} {}

    using Type = T;

    T Class::*member;
    const char* name;
};

template<typename Class, typename T>
constexpr auto property(T Class::*member, const char* name) {
    return PropertyImpl<Class, T>{member, name};
}

// unserialize function
template<typename T>
T fromJson(const std::string& data) {
    return T();
}

template<typename T>
std::string toJson(const T& object) {
    return "";
}

}

#endif // !defined(FABRIC_UTILS_JSON_H)

