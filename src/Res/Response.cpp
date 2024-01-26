/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paescano <paescano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 17:21:30 by paescano          #+#    #+#             */
/*   Updated: 2024/01/26 14:52:48 by paescano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Server.hpp"

Response::Response()
{
	this->from = "";
	this->code = NONE;
	this->to = "";
	this->command = "";
	this->trailer = "";
	this->sentfd = 1;
}

Response::~Response(){}

Response Response::createMessage()
{
	Response response;
	response.type = MESSAGE;
	return response;
}

Response Response::createReply(const Code &code)
{
	Response response;
	//response.from = from;
	response.code = code;
	response.type = REPLY;
	return response;
}

Response& Response::From(const Client &client) { 
	this->from = client.nick ;//+ "!" + client.user + "@" + client.host;
	return *this;
 }

 Response& Response::From(const Server & server) { 
	this->from = server.serverName + "." + server.serverHost ;
	return *this;
 }

Response& Response::To(const Client &client) {
	this->sentfd = client.fd;
	this->to = client.nick;
	return *this;
}

Response &	Response::Command( const std::string & command ){
	this->command = command;
	return *this;
}

Response& Response::Trailer(const std::string &trailer){
	this->trailer = trailer;
	return *this;
}

std::string Response::generateMessage(){
	std::stringstream ss;

	if (!this->from.empty())
		ss << ":" << this->from << " ";
	if (!this->command.empty())
		ss << this->command << " ";
	if (!this->trailer.empty())
		ss << ":" << this->trailer;
	ss << "\r\n";
	return ss.str();
}

std::string Response::generateReply(){
	std::stringstream ss;

	ss << ":" << this->from << " ";
	ss << std::setw(3) << std::setfill('0') << this->code << " ";
	ss << this->to << " ";
	if (!this->command.empty())
		ss << this->command << " ";
	if (!this->trailer.empty())
		ss << ":" << this->trailer;
	ss << "\r\n";
	return ss.str();
}

void Response::Send(){
	std::string message;
	switch(this->type){
		case MESSAGE:
			message = generateMessage();
			break;
		case REPLY:
			message = generateReply();
			break;
	}
	send(this->sentfd, message.c_str(), message.length(), 0);
	if (DEBUG)
		std::cout << "From response: " << message << std::endl;
}
