//===-- chaincode.cc - Chaincode implementation -----------------*- C++ -*-===//
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


#include "fabric/shim/chaincode.h"
#include "fabric/comm/connection.h"
#include "peer/chaincode_shim.grpc.pb.h"
#include "handler.h"
#include "utils.h"

namespace fabric::shim {

namespace {

std::string key;
std::string cert;

std::string peerAddress;

std::string getPeerAddress() {
    if (peerAddress != "")
        return peerAddress;

    peerAddress = fabric::utils::getenv("CORE_CHAINCODE_PEER_ADDRESS");
    if (peerAddress == "") {
        throw std::logic_error("peer.address not configured, can't connect to peer");
    }

    return peerAddress;
}

void setupChaincodeLogging() {
    // TODO Initialize logging
    // Variables:
    //    CORE_CHAINCODE_LOGGING_FORMAT
    //    CORE_CHAINCODE_LOGGING_LEVEL
    //    CORE_CHAINCODE_LOGGING_SHIM
    //    CORE_CHAINCODE_BUILDLEVEL
}

std::shared_ptr<grpc::Channel> newPeerClientConnection() {
    auto peerAddress = getPeerAddress();
    // set the keepalive options to match static settings for chaincode server
    grpc::ChannelArguments args;
    args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 60*1000);
    args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 20*1000);
    if (fabric::comm::isTLSEnabled()) {
        return fabric::comm::newClientConnectionWithAddress(peerAddress, true, false, fabric::comm::initTLSForShim(key, cert), args);
    }
    return fabric::comm::newClientConnectionWithAddress(peerAddress, true, false, grpc::InsecureChannelCredentials(), args);
}


std::unique_ptr<PeerChaincodeStream> userChaincodeStreamGetter(const std::string name) {
/*
        clientConn, err := newPeerClientConnection()
        if err != nil {
                err = errors.Wrap(err, "error trying to connect to local peer")
                chaincodeLogger.Errorf("%+v", err)
                return nil, err
        }

        chaincodeLogger.Debugf("os.Args returns: %s", os.Args)

        chaincodeSupportClient := pb.NewChaincodeSupportClient(clientConn)

        stream, err := chaincodeSupportClient.Register(context.Background())
        if err != nil {
                return nil, errors.WithMessage(err, fmt.Sprintf("error chatting with leader at address=%s", getPeerAddress()))
        }

        return stream, nil
*/
    grpc::ClientContext context;

    // Establish connection with validating peer
    std::shared_ptr<grpc::Channel> clientConn = newPeerClientConnection();
    auto chaincodeSupport = protos::ChaincodeSupport::NewStub(clientConn);

    // Establish stream with validating peer
    return chaincodeSupport->Register(&context);
}


void chatWithPeer(std::string chaincodename, std::unique_ptr<PeerChaincodeStream>& stream, Chaincode& cc) {
    Handler* handler = Handler::newChaincodeHandler();
    // raii(stream->CloseSend());
    protos::ChaincodeID chaincodeID;
    chaincodeID.set_name(chaincodename);
    std::string payload;
    chaincodeID.SerializeToString(&payload);

    protos::ChaincodeMessage chaincodeMsg;
    chaincodeMsg.set_type(::protos::ChaincodeMessage::REGISTER);
    chaincodeMsg.set_payload(payload);
    handler->serialSend(chaincodeMsg);

/*

*/
}

}

void start(Chaincode& cc)
{
    // If start() is called, we assume this is a standalone chaincode and set
    // up formatted logging.
    setupChaincodeLogging();
    auto chaincodename = fabric::utils::getenv("CORE_CHAINCODE_ID_NAME");
    if (chaincodename == "") {
        throw std::logic_error("error chaincode id not provided");
    }

/*
        err := factory.InitFactories(factory.GetDefaultOpts())
        if err != nil {
                return errors.WithMessage(err, "internal error, BCCSP could not be initialized with default options")
        }
*/

    // TODO Implement mock getter
/*
        //mock stream not set up ... get real stream
        if streamGetter == nil {
                streamGetter = userChaincodeStreamGetter
        }

        stream, err := streamGetter(chaincodename)
        if err != nil {
                return err
        }
*/
    std::unique_ptr<PeerChaincodeStream> stream = userChaincodeStreamGetter(chaincodename);

    chatWithPeer(chaincodename, stream, cc);
}

}
