#pragma once

#include "connection_handler.h"
#include "datastream.h"
#include <map>
#include <memory>
#include <string>

class ConnectionFactory {
    private:
        std::map<std::string, std::unique_ptr<ConnectionHandler>> connections;

    public:
        ConnectionFactory() = default; // ?
        ~ConnectionFactory() = default; // ?

        std::string createConnection(const std::string &ip, int port, DataStream *datastream);
        bool closeConnection(const std::string &conn_id);
        ConnectionHandler* getConnection(const std::string &conn_id);
};