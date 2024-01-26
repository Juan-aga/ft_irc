/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paescano <paescano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 17:21:30 by paescano          #+#    #+#             */
/*   Updated: 2024/01/26 13:02:27 by paescano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response()
{
	this->from = server.serverName + "." + server.serverHost;
	this->code = NONE;
	this->to = "";
	this->content = "";
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
	response.code = code;
	response.type = REPLY;
	return response;
}

Response& Response::from(const Client &client) { 
	this->from = client.nick + "!" + client.user + "@" + client.host;
	return *this;
 }

Response& Response::to(const Client &client) {
	this->fd = client.fd;
	this->to = client.nick;
	return *this;
}

Response& Response::content(const string &content){
	this->content = content;
	return *this;
}

string Response::generateMessage(){
	std::stringstream stream;

	if (!this->from.empty())
		stream << ":" << this->from << " ";
	stream << this->content << "\r\n";
	return stream.str();
}

string Response::generateReply(){
	std::stringstream stream;

	stream << ":" << this->from << " ";
	stream << std::setw(3) << std::setfill('0') << this->code << " ";
	stream << this->to;
	if (!this->content.empty())
		stream << " :" << this->content;
	stream << "\r\n";
	return stream.str();
}

void Response::send(){
	std::stringstream stream;
	string message;
	switch(this->type){
		case MESSAGE:
			message = generateMessage();
			break;
		case REPLY:
			message = generateReply();
			break;
	}
	::send(this->sentfd, message.c_str(), message.length(), 0);
}
