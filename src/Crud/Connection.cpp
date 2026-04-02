#include "Connection.h"

Connection::Connection() {
    this->database = Database::getInstance();
}

Connection::~Connection() {}

const Database* Connection::getConnection() {
    return this->database;
}