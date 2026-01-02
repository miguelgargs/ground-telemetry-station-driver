#pragma clang diagnostic ignored "-Wunused-parameter"
#include "connection_factory.h"
#include "connection_service.cpp"
#include "datastream_factory.h"
#include "datastream_service.cpp"
#include <csignal>
#include <thread>

void runServer()
{
  /**
  Runs the server loop.
  */
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
  DataStreamFactory datastream_factory;
  ConnectionFactory factory;
  std::string server_address("0.0.0.0:50051");
  ConnectionServiceImpl service(factory, datastream_factory);
  DataStreamServiceImpl dataservice(datastream_factory);

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  builder.RegisterService(&dataservice);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // thread waits for CTRL-C and kills server if received
  std::thread([&sigset, &server]()
              {
    int sig;
    sigwait(&sigset, &sig);  // Blocks until CTRL+C
      gpr_timespec deadline = gpr_time_add(
      gpr_now(GPR_CLOCK_REALTIME),          // current time
      gpr_time_from_seconds(3, GPR_TIMESPAN)); // +3 seconds
    std::cout << "\nCTRL+C received, shutting down..." << std::endl;
    server->Shutdown(deadline);
    std::cout << "\nServer Shutdown has been called." << std::endl; })
      .detach();

  server->Wait();
}

int main(int argc, char *argv[])
{
  runServer();
  return 0;
}
