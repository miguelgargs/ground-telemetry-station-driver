#include "datastream_factory.h"
#include <iostream>
#include <sstream>

DataStream *DataStreamFactory::create_datastream(const std::string name)
{
    auto datastream = std::make_unique<DataStream>(); // unique pointer
    DataStream *ptr = datastream.get();               // get the pointer
    datastreams[name] = std::move(datastream);        // add the pointer to the list
    std::cout << "Datastream created: " << name << std::endl;
    return ptr;
}

DataStream *DataStreamFactory::get_datastream(const std::string conn_id)
{
    auto it = datastreams.find(conn_id);

    if (it == datastreams.end())
    {
        std::cerr << "DataStream not found: " << conn_id << std::endl;
        return nullptr;
    }
    std::cout << "Datastream found: " << conn_id << std::endl;
    return it->second.get();
}
