#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "datastream_service/datastream.grpc.pb.h"
#include "datastream_service/datastream.pb.h"
#include "datastream_factory.h"

#include <chrono>
#include <thread>
#include "heartbeat_data.h"
#include "msg_data.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

class DataStreamServiceImpl final : public DataStreamService::Service
{
private:
    DataStreamFactory &datastream_factory;

public:
    DataStreamServiceImpl(DataStreamFactory &datastream_factory) : datastream_factory(datastream_factory) {}
    Status receive(ServerContext *context, const DataStreamRequest *request,
                   ServerWriter<DataStreamResponse> *writer)
    {
        // server receives the command, starts checking if there are messages in the datafactory.
        // behavior: server receives the ID of the connection with which the DataStream is also identified.
        std::string conn_id = request->conn_id();
        DataStream *datastream = datastream_factory.get_datastream(conn_id);
        std::string outgoing_message; // TODO: delete me
        MsgData msgdata;
        HeartbeatPayload *hpayload;

        while (!(context->IsCancelled()))
        // This loop will only die when the client disconnects.
        {
            // is there a message?
            if (datastream->has_messages())
            {
                // outgoing_message = datastream->pull_msg(); // currently returns the string. TODO: this is no longer a string!!!
                msgdata = datastream->pull_msg();
                // has to return the sys source, comp source, heartbeat payload
                DataStreamResponse dsresponse = DataStreamResponse();
                hpayload = dsresponse.mutable_heartbeat();
                hpayload->set_custom_mode(msgdata.heartbeatdata.custom_mode); 
                hpayload->set_type(msgdata.heartbeatdata.type); 
                hpayload->set_autopilot(msgdata.heartbeatdata.autopilot); 
                hpayload->set_base_mode(msgdata.heartbeatdata.base_mode); 
                hpayload->set_system_status(msgdata.heartbeatdata.system_status); 
                hpayload->set_mavlink_version(msgdata.heartbeatdata.mavlink_version); 
                dsresponse.set_content(outgoing_message); // TODO: delete this
                dsresponse.set_src_system(msgdata.src_sys);
                dsresponse.set_src_component(msgdata.src_comp);
                std::cout << "[X] DataStreamService::sending out message: " << outgoing_message << std::endl;
                writer->Write(dsresponse);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // TODO improve this in the next version, and let datastream have a way to notify this thread that there is new data.
        }
        // When the connection ends
        return Status::OK;
    }
};