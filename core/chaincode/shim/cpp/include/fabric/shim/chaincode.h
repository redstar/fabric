//===-- chaincode.h - Chaincodeinterface ----------*- C++ -*-===//
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

#if !defined(FABRIC_SHIM_CHAINCODE_H)
#define FABRIC_SHIM_CHAINCODE_H

#include <string>
#include "common/common.pb.h"
#include "peer/proposal_response.pb.h"
#include "fabric/shim/chaincodestub.h"
#include "fabric/shim/response.h"

namespace fabric::shim {

/// @class Chaincode
/// Chaincode interface must be implemented by all chaincodes. The fabric runs
/// the transactions by calling these functions as specified.
class Chaincode {
public:
    virtual ~Chaincode() {}

    /// Init is called during Instantiate transaction after the chaincode container
    /// has been established for the first time, allowing the chaincode to
    /// initialize its internal data
    /// @param stub
    virtual protos::Response init(ChaincodeStub& stub) {
        return success();
    }

    /// Invoke is called to update or query the ledger in a proposal transaction.
    /// Updated state variables are not committed to the ledger until the
    /// transaction is committed.
    /// @param stub
    virtual protos::Response invoke(ChaincodeStub& stub) {
        return success();
    }
};


void start(Chaincode& cc);

}

#endif // !defined(FABRIC_SHIM_CHAINCODE_H)

