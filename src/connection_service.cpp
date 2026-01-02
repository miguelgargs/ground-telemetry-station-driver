#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "connection.grpc.pb.h"
#include "connection.pb.h"
#include "connection_factory.h"
#include "datastream_factory.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

// Implementar el servicio ConnectionService
class ConnectionServiceImpl final : public ConnectionService::Service
{
private:
  ConnectionFactory &factory;
  DataStreamFactory &datastream_factory;

public:
  ConnectionServiceImpl(ConnectionFactory &factory, DataStreamFactory &datastream_factory) : factory(factory), datastream_factory(datastream_factory) {}
  // the following function has override bc 'connect' is a keyword
  Status connect(ServerContext *context, const ConnectionRequest *request,
                 ConnectionResponse *reply) override
  {
    std::cout << "Connect request received:" << std::endl;
    std::cout << "|-- Address: " << request->address() << std::endl;
    std::cout << "|-- Port: " << request->port() << std::endl;

    auto connection_id = ("conn_" + request->address() + "_" +
                             std::to_string(request->port()));

    auto new_datastream = datastream_factory.create_datastream(connection_id);

    auto connection_created =
        factory.createConnection(request->address(), request->port(), new_datastream);
    reply->set_connection_id(connection_id);
    reply->set_success(true);
    // reply->set_error_message("");
    Status operation_result = Status::OK;
    if (connection_created.length() < 1)
    {

      operation_result =
          Status(grpc::StatusCode::INVALID_ARGUMENT,
                 "The connection could not be open for unknown reasons.");
    }
    return operation_result;
  }

  Status disconnect(ServerContext *context, const DisconnectRequest *request,
                    DisconnectResponse *reply) override
  {
    std::cout << "Disconnect request received:" << std::endl;
    std::cout << "  Connection ID: " << request->connection_id() << std::endl;

    auto existing_connection = factory.getConnection(request->connection_id());
    bool connection_closed = false;
    if (existing_connection)
    {
      connection_closed = factory.closeConnection(request->connection_id());
    }

    Status operation_result = Status::OK;
    if (!connection_closed)
    {
      operation_result = Status(grpc::StatusCode::INVALID_ARGUMENT,
             "The connection could not be open for unknown reasons.");
    }

    reply->set_success(true);

    return operation_result;
  }
};
