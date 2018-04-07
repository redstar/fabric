// License: Apache-2.0

#include "fabric/shim/chaincodestub.h"

namespace fabric::shim {

std::vector<std::string> ChaincodeStub::getStringArgs() {
    return std::vector<std::string>();
}

void ChaincodeStub::getFunctionAndParameters(std::string& func, std::vector<std::string>& params) {
    auto stringArgs = getStringArgs();
    // TODO
    func = stringArgs[1];
    params = stringArgs;
}

}
