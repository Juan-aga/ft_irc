#include "Server.hpp"

Server::Server(std::string password, int port) : password(password), port(port) {}

Server::~Server() {}

int const			&Server::getPort() const { return (this->port); }

std::string const	&Server::getPassword() const { return (this->password); }