//===-- response.h -  ----------*- C++ -*-===//
//
//                     Hyperledger Fabric
//
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief
/// This file contains the chaincode implementation.
///
//===----------------------------------------------------------------------===//

// License: Apache-2.0

#if !defined(FABRIC_SHIM_RESPONSE_H)
#define FABRIC_SHIM_RESPONSE_H

#include <string>
#include "common/common.pb.h"
#include "peer/proposal_response.pb.h"

namespace fabric::shim {

enum {
    /// OK constant - status code less than 400, endorser will endorse it.
    /// OK means init or invoke successfully.
    OK = 200,

    /// ERRORTHRESHOLD constant - status code greater than or equal to 400 will be considered an error and rejected by endorser.
    ERRORTHRESHOLD = 400,

    /// ERROR constant - default error value
    ERROR = 500
};

inline protos::Response success() {
    protos::Response response;
    response.set_status(OK);
    return response;
}

inline protos::Response success(std::string payload) {
    protos::Response response;
    response.set_status(OK);
    response.set_payload(payload);
    return response;
}

inline protos::Response error(std::string msg) {
    protos::Response response;
    response.set_status(ERROR);
    response.set_message(msg);
    return response;
}

}

#endif // !defined(FABRIC_SHIM_RESPONSE_H)
