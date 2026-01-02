#pragma once

#include <string>
#include <cstdint>
#include "datastream.h"
#include <thread>
#include <atomic>

/**
 * @brief Manage MAVLink connections with a flight controller.
 */

class ConnectionHandler
{
private:
    int socket_fd;                 // Socket file descriptor
    std::string ip_address;        // Flight controller IP address
    int port;                      // Flight controller port
    char buffer[1024];             // Buffer to store messages (1kB)
    DataStream *datastream;        // DataStream to manage messages
    std::thread reading_thread;    // Reads data from the socket
    std::atomic<bool> should_stop; // Flag for signaling the thread to stop

public:
    /**
     * @brief default constructor
     */
    ConnectionHandler();

    /**
     * @brief Constructor que arranca con IP y puerto
     */
    ConnectionHandler(const std::string &ip_address, int port, DataStream *datastream);

    /**
     * Destructor
     */
    ~ConnectionHandler();

    /**
     * Returns whether there is an active connection
     */
    bool isConnected();

    /**
     * Setter for ip_address
     */
    void setIpAddress(const std::string &ip_address);

    /**
     * Setter for port
     */
    void setPort(int port);

    /**
     * Open connection.
     * Returns true if success, false otherwise.
     */
    bool connectSocket(const std::string &ip_address, const int port);

    /**
     * Close connection.
     * Returns true if sucess, false otherwise.
     */
    bool disconnectSocket();

    /**
     * Read data from socket and push to DataStream.
     * Returns number of bytes read, or -1 on error, 0 on disconnect.
     */
    int readData();

    void readingLoop(); // Loop ran by thread
    void startReadingThread();
    void stopReadingThread();
};