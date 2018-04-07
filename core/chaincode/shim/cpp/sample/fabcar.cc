#include "fabric/shim/chaincode.h"
#include "fabric/utils/json.h"

using namespace fabric;

// Define the car structure, with 4 properties.
struct Car {
    std::string make;
    std::string model;
    std::string colour;
    std::string owner;

    constexpr static auto properties = std::make_tuple(
        utils::property(&Car::make, "make"),
        utils::property(&Car::model, "model"),
        utils::property(&Car::colour, "colour"),
        utils::property(&Car::owner, "owner")
    );
};

class Chaincode : public shim::Chaincode {
    virtual protos::Response init(shim::ChaincodeStub& stub) {
        return shim::success();
    }

    virtual protos::Response invoke(shim::ChaincodeStub& stub) {
        std::string function;
        std::vector<std::string> args;

	// Retrieve the requested Smart Contract function and arguments
        stub.getFunctionAndParameters(function, args);

	// Route to the appropriate handler function to interact with the ledger appropriately
        if (function == "queryCar")
            return queryCar(stub, args);
        else if (function == "initLedger")
            return initLedger(stub, args);
        else if (function == "createCar")
            return createCar(stub, args);
        else if (function == "queryAllCars")
            return queryAllCars(stub, args);
        else if (function == "changeCarOwner")
            return changeCarOwner(stub, args);

        return shim::error("Invalid Smart Contract function name.");
    }

    protos::Response queryCar(shim::ChaincodeStub& stub, std::vector<std::string> args) {
        if (args.size() != 1) {
            throw std::logic_error("Incorrect number of arguments. Expecting 1");
        }

        auto car = stub.getStateAsString(args[0]);
        return shim::success(car);
    }

    protos::Response initLedger(shim::ChaincodeStub& stub, std::vector<std::string> args) {
        Car cars[] = {
            {"Toyota", "Prius", "blue", "Tomoko"},
            {"Ford", "Mustang", "red", "Brad"},
            {"Hyundai", "Tucson", "green", "Jin Soo"},
            {"Volkswagen", "Passat", "yellow", "Max"},
            {"Tesla", "S", "black", "Adriana"},
            {"Peugeot", "205", "purple", "Michel"},
            {"Chery", "S22L", "white", "Aarav"},
            {"Fiat", "Punto", "violet", "Pari"},
            {"Tata", "Nano", "indigo", "Valeria"},
            {"Holden", "Barina", "brown", "Shotaro"}
        };
        int i = 0;
        for (auto car : cars) {
            stub.putState("CAR" + std::to_string(i), utils::toJson(car));
            ++i;
        }
        return shim::success();
    }

    protos::Response createCar(shim::ChaincodeStub& stub, std::vector<std::string> args) {
        if (args.size() != 5) {
            throw std::logic_error("Incorrect number of arguments. Expecting 5");
        }

        Car car = { args[1], args[2], args[3], args[4] };
        stub.putState(args[0], utils::toJson(car));
        return shim::success();
    }

    protos::Response queryAllCars(shim::ChaincodeStub& stub, std::vector<std::string> args) {
        std::string startKey = "CAR0";
        std::string endKey = "CAR999";

        // TODO
        /* auto iter = */ stub.getStateByRange(startKey, endKey);

        return shim::success();
    }

    protos::Response changeCarOwner(fabric::shim::ChaincodeStub& stub, std::vector<std::string> args) {
        if (args.size() != 2) {
            throw std::logic_error("Incorrect number of arguments. Expecting 2");
        }

        auto carAsString = stub.getStateAsString(args[0]);
        Car car = utils::fromJson<Car>(carAsString);
        car.owner = args[1];
        stub.putState(args[0], utils::toJson(car));
        return shim::success();
    }
};

int main(int argc, char* argv[]) {
    Chaincode chaincode;
    shim::start(chaincode);
    return 0;
}
