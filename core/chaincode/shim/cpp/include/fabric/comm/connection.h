// License: Apache-2.0

#if !defined(FABRIC_COMM_CONNECTION_H)
#define FABRIC_COMM_CONNECTION_H

#include <string>
#include "grpc/grpc.h"
#include "grpcpp/channel.h"
#include "grpcpp/client_context.h"
#include "grpcpp/create_channel.h"
#include "grpcpp/security/credentials.h"
//#include "grpcpp/security/server_credentials.h"


namespace fabric::comm {

bool isTLSEnabled();

std::shared_ptr<grpc::ChannelCredentials> initTLSForShim(std::string key, std::string certStr);

std::shared_ptr<grpc::Channel> newClientConnectionWithAddress(std::string peerAddress, bool block, bool tlsEnabled,
                                                              const std::shared_ptr<grpc::ChannelCredentials>& creds,
                                                              const grpc::ChannelArguments& args);

}

#endif // !defined(FABRIC_COMM_CONNECTION_H)

