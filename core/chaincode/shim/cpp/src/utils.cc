//===-- utils.cc - Utility implementation -----------------*- C++ -*-===//
//
//                     Hyperledger Fabric
//
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains utility functions.
///
//===----------------------------------------------------------------------===//


#include <cstdlib>
#include "utils.h"

namespace fabric::utils {

std::string getenv(std::string key) {
    char* value = std::getenv(key.c_str());
    return value == nullptr ? std::string() : std::string(value);
}

}
