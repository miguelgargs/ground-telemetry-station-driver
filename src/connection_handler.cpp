/**
 * @file connectionhandler.cpp
 * @brief ConnectionHandler implementation for managing MAVLink connections.
 *
 * Esta clase encapsula toda la lógica necesaria para establecer y mantener una conexión
 * TCP/UDP con un flight controller a través del protocolo MAVLink. Proporciona métodos
 * para conectar, desconectar, recibir y procesar mensajes MAVLink, así como validar
 * checksums y extraer información de los paquetes recibidos.
 *
 * @author miguelgargs
 * @version 1.0
 * @date 2025-11-23
 *
 * @see connectionhandler.h for the class definition
 */

#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include "connection_handler.h"

ConnectionHandler::ConnectionHandler() : socket_fd(-1), ip_address(""), port(0) {}

ConnectionHandler::ConnectionHandler(const std::string &ip_address, const int port, DataStream *datastream) : socket_fd(-1), ip_address(ip_address), port(port), datastream(datastream) {}

ConnectionHandler::~ConnectionHandler()
{
  disconnectSocket();
}

bool ConnectionHandler::isConnected()
{
  return this->socket_fd > 0 ? true : false;
}

void ConnectionHandler::setIpAddress(const std::string &ip_address)
{
  this->ip_address = ip_address;
}

void ConnectionHandler::setPort(const int port)
{
  this->port = port;
}

bool ConnectionHandler::connectSocket(const std::string &ip_address, const int port)
{
  std::cout << "connectSocket parameters:" << ip_address << "===" << port << std::endl;
  this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->socket_fd < 0)
  {
    std::cerr << "Error trying to open socket" << std::endl;
    return false;
  }
  std::cout << "Sucessfully opened socket" << std::endl;

  // Set up hints for getaddrinfo
  struct addrinfo hints, *res;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP

  // Convert port to string
  std::string port_str = std::to_string(port);

  // Resolve hostname/IP using getaddrinfo
  int status = getaddrinfo(ip_address.c_str(), port_str.c_str(), &hints, &res);
  if (status != 0)
  {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    close(this->socket_fd);
    this->socket_fd = -1;
    return false;
  }

  // Try to connect using the resolved address
  if (connect(this->socket_fd, res->ai_addr, res->ai_addrlen) < 0)
  {
    std::cerr << "Error connecting to socket. Check if there is a MAVLink device on the other side - " << ip_address << ":" << port
              << " - " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
    freeaddrinfo(res);
    close(this->socket_fd);
    this->socket_fd = -1;
    return false;
  }

  // Clean up the addrinfo structure
  freeaddrinfo(res);

  std::cout << "Socket conectado a " << ip_address << ":" << port << std::endl;
  // Now: start thread!
  this->should_stop = false;
  this->startReadingThread();
  return true;
}

bool ConnectionHandler::disconnectSocket()
{
  if (this->isConnected())
  {
    this->should_stop = true;
    shutdown(this->socket_fd, SHUT_RDWR); // Life hack: if shutdown is called before killing the thread, it will finish faster, as no more data will arrive.
    // Signal the thread to stop
    this->stopReadingThread();
    close(this->socket_fd);
  }
  return true;
}

/**
 * Read data from socket and push to DataStream.
 * Uses blocking recv() - will wait until data arrives.
 * Returns number of bytes read, -1 on error, 0 on disconnect.
 */
int ConnectionHandler::readData()
{
  if (!this->isConnected())
  {
    std::cerr << "Error: Socket not connected" << std::endl;
    return -1;
  }

  // Read data from socket (blocking call)
  int bytes_received = recv(this->socket_fd, this->buffer, sizeof(this->buffer), 0);

  if (bytes_received < 0)
  {
    std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
    return -1;
  }
  else if (bytes_received == 0)
  {
    std::cout << "Connection closed by remote host" << std::endl;
    return 0;
  }

  // Push data to DataStream for processing
  if (this->datastream != nullptr)
  {
    this->datastream->push_raw_data((const uint8_t *)this->buffer, bytes_received);
  }
  else
  {
    std::cerr << "Warning: DataStream is null, data discarded" << std::endl;
  }

  return bytes_received;
}

/**
 * Runs in a loop and reads data
 * from the socket and sends it over to
 * the DataStream buffer.
 *
 * Due to the way in which this has been written,
 * it is using blocking recv, meaning that it won't
 * lock up and use 100% CPU while waiting for new data.
 */
void ConnectionHandler::readingLoop() {
  while (!this->should_stop) {
    int bytes = readData();
    if (bytes < 0) {
      // Error: exit
      break;
    } else if (bytes == 0) {
      // Disconnect: exit
      break;
    }
  }
}

/**
 * Starts the reading thread.
 */
void ConnectionHandler::startReadingThread() {
  this->reading_thread = std::thread(&ConnectionHandler::readingLoop, this);
}

/**
 * Join the threads and close business.
 */
void ConnectionHandler::stopReadingThread() {
  if (this->reading_thread.joinable()) {
    this->reading_thread.join();
  }
}