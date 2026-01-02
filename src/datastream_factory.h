#pragma once
#include "datastream.h"
#include <map>
#include <memory>
#include <string>

class DataStreamFactory
{
private:
  std::map<std::string, std::unique_ptr<DataStream>> datastreams;

public:
  DataStreamFactory() = default;
  ~DataStreamFactory() = default;

  /**
   * Create a new DataStream with the same ID as the Connection.
   */
  DataStream *create_datastream(const std::string name);
  /**
   * Searches for a DataStream in the map and returns it.
   */
  DataStream *get_datastream(const std::string conn_id);
};
