// Written in the D programming language.
module fabcar;

import fabric.shim.chaincode;
import fabric.shim.response;

class Chaincode : DefaultChaincode
{
    mixin InvokeHelper!Chaincode;

    @SmartContract
    Response queryCar(ChaincodeStub stub, string arg)
    {
        auto carAsBytes = stub.getState(arg);
        return success(carAsBytes);
    }
}

