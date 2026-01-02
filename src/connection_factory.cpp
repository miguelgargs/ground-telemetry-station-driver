#include "connection_factory.h"
#include <iostream>
#include <sstream>

std::string ConnectionFactory::createConnection(const std::string &ip, int port, DataStream *datastream) {
  /**
   * Crea una nueva conexión MAVLink y la almacena en el mapa.
   *
   * @param ip Dirección IP del flight controller
   * @param port Puerto del flight controller
   * @return ID único de la conexión (ej: "conn_127.0.0.1_5760")
   *
   * La conexión se abre inmediatamente y se mantiene en memoria
   * hasta que se llame a closeConnection().
   */

  // Generar ID único para la conexión
  std::ostringstream oss;
  oss << "conn_" << ip << "_" << port;
  std::string conn_id = oss.str();

  // Crear nueva instancia de ConnectionHandler
  auto handler = std::make_unique<ConnectionHandler>(ip, port, datastream);

  // Intentar conectar
  if (!handler->connectSocket(ip, port)) {
    std::cerr << "Failed to create connection: " << conn_id << std::endl;
    return "";  // Retornar string vacío si falla
  }

  // Almacenar en el mapa
  connections[conn_id] = std::move(handler);

  std::cout << "Connection created: " << conn_id << std::endl;
  return conn_id;
}

bool ConnectionFactory::closeConnection(const std::string &conn_id) {
  /**
   * Cierra y elimina una conexión MAVLink del mapa.
   *
   * @param conn_id ID de la conexión a cerrar
   * @return true si se cerró exitosamente, false si no existe
   *
   * Al eliminar del mapa, el destructor de ConnectionHandler
   * se ejecuta automáticamente y cierra el socket.
   */

  auto handler = getConnection(conn_id);
  if (handler == nullptr) {
    return false;
  }

  // Desconectar antes de eliminar
  handler->disconnectSocket();

  // Eliminar del mapa (destructor se ejecuta automáticamente)
  connections.erase(conn_id);

  std::cout << "Connection closed: " << conn_id << std::endl;
  return true;
}

ConnectionHandler* ConnectionFactory::getConnection(const std::string &conn_id) {
  /**
   * Obtiene un puntero a una conexión existente.
   *
   * @param conn_id ID de la conexión
   * @return Puntero a ConnectionHandler si existe, nullptr si no
   *
   * El puntero es válido mientras la conexión exista en el factory.
   * No se debe guardar el puntero para uso posterior, ya que puede
   * ser invalidado si la conexión se cierra.
   */

  auto it = connections.find(conn_id);

  if (it == connections.end()) {
    std::cerr << "Connection not found: " << conn_id << std::endl;
    return nullptr;
  }

  return it->second.get();
}
