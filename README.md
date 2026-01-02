# mavlink-driver
This component is the C++ driver that connects to the MAVLink flight controller and sends and receives MAVLink messages via a TCP link.

Messages are received as a stream in the driver, which verifies them and discards the not valid ones. The valid messages are then transformed into useful information, by extracting their key data, such as `system_id`, `component_id`, `system_status`, `autopilot`, etc. and sent via gRPC to the backend server. The driver works as the low-level layer that works directly with messages in their bytes form and such.

The `protobuf` specification for the communication between the driver and the backend is found here. This is the shared code that both components must use when talking to each other.

## Building the project

There are two ways to build and deploy the project. One is to follow all the steps and locally deploy, the second is to use the Docker image included and launch it from the main repository (with all the submodules). Refer to [github.com/miguelgargs/ground-station](https://github.com/miguelgargs/ground-station) or keep reading for finding out how to deploy it locally.

Install the required dependencies:
```bash
sudo apt install protobuf-compiler
sudo apt install -y libgrpc++-dev protobuf-compiler-grpc
```

Now build the project using the `build.sh` script (this uses `ninja`):
```bash
chmod +x ./build.sh
./build.sh
```

Not needed, but documented here, generating the .proto files for `gRPC` and `protobuf`.
```bash
protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) src/proto/connection.proto
```

## Running the project
```bash
./build/mavlink-driver
```

This deploys the driver and makes it accessible via:
- IP: `localhost`
- PORT: `50051`
