/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jteissie <jteissie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/28 11:26:13 by jteissie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

std::map<std::string,std::string> Response::_contentTypeMap;

void Response::setupContentTypeMap()
{
	_contentTypeMap["html"] = "text/html";
	_contentTypeMap["css"] = "text/css";
	_contentTypeMap["js"] = "text/javascript";
	_contentTypeMap["txt"] = "text/plain";
	_contentTypeMap["jpg"] = "image/jpeg";
	_contentTypeMap["jpeg"] = "image/jpeg";
	_contentTypeMap["png"] = "image/png";
	_contentTypeMap["gif"] = "image/gif";
}

std::string	Response::makeResponseHeader(uint32_t status,
						 const std::string & statusMessage,
						 const std::string & contentType,
						 size_t contentLength, int clientSocket)
{
	std::ostringstream	response;

	response << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
	if (clientSocket != -1)
		response << "Set-Cookie: ID=" << clientSocket << "; Path=/ \r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << contentLength << "\r\n";
	//response << "Connection: Closed\r\n";
	response << "\r\n";
	return response.str();
}

OutgoingData *	Response::makeResponse(uint32_t status,
						 const std::string & statusMessage,
						 const std::string & contentType,
						 const std::string & content,
						 const std::string & addHeader)
{
	std::ostringstream	response;

	response << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << content.length() << "\r\n";
	response << addHeader << "\r\n";
	response << "\r\n";
	return new OutgoingData(response.str(), content);
}

OutgoingData *Response::makeErrorResponse(uint32_t status, const std::string &statusMessage, const t_server &server, int clientSocket)
{
	std::string filename = server.root + server.error;
	ssize_t filesize = getFileSize(filename);

	if (filesize == -1)
	{
		std::ostringstream content;
		content << status << " " << statusMessage;
		return makeResponse(status, statusMessage, "text/plain", content.str());
	}
	std::ifstream file(filename.c_str());
	std::string htmlContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::ostringstream statusCodeStr;
	statusCodeStr << status;
	size_t pos;
	pos = htmlContent.find("{{statusCode}}");
	if (pos != std::string::npos)
		htmlContent.replace(pos, 14, statusCodeStr.str());
	pos = htmlContent.find("{{errorMessage}}");
	if (pos != std::string::npos)
		htmlContent.replace(pos, 16, statusMessage);
	std::string header = makeResponseHeader(status, statusMessage, getContentType("html"), htmlContent.size(), clientSocket);
	return new OutgoingData(header, htmlContent);
}

OutgoingData *Response::makeErrorResponse(uint32_t status, const std::string &statusMessage, std::string root, std::string error, int clientSocket)
{
	std::string filename = root + error;
	ssize_t filesize = getFileSize(filename);

	if (filesize == -1)
	{
		std::ostringstream content;
		content << status << " " << statusMessage;
		return makeResponse(status, statusMessage, "text/plain", content.str());
	}
	std::ifstream file(filename.c_str());
	std::string htmlContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::ostringstream statusCodeStr;
	statusCodeStr << status;
	size_t pos;
	pos = htmlContent.find("{{statusCode}}");
	if (pos != std::string::npos)
		htmlContent.replace(pos, 14, statusCodeStr.str());
	pos = htmlContent.find("{{errorMessage}}");
	if (pos != std::string::npos)
		htmlContent.replace(pos, 16, statusMessage);
	std::string header = makeResponseHeader(status, statusMessage, getContentType("html"), htmlContent.size(), clientSocket);
	return new OutgoingData(header, htmlContent);
}

/* Return first route that match 'uri'
 * if not found with full uri retry in a loop with uri without the last (1,2,3,...) folder(s)
 * it will ultimately match with "/" if this route exits or return NULL
*/
const t_route * Response::getRouteFromUri(const t_server & server, std::string uri)
{
	while (true) {
		//std::cout << "[getRouteFromUri():find loop] uri: " << uri << "\n";
		std::map<std::string, t_route>::const_iterator it;
		it = server.routes.find(uri.size() == 0 ? "/" : uri);
		if (it != server.routes.end())
			return &it->second;
		size_t lastSlashPos = uri.find_last_of("/");
		if (lastSlashPos != std::string::npos)
			uri.erase(lastSlashPos);
		else
			return NULL;
	}
}

std::string Response::getContentType(const std::string & uri)
{
	std::string ext = uri.substr(uri.find_last_of(".") + 1);
	return _contentTypeMap[ext];
}

OutgoingData * Response::handleGet(const t_server & server, const HTTPRequest & req, int clientSocket, Server &serverObj)
{
	std::string uri = req.getUriWithoutQString();
	const t_route *routeptr = getRouteFromUri(server, uri);
	if (routeptr == NULL) {
		std::cout << "     no route found from uri: " << uri << std::endl;
		return makeErrorResponse(404, "Not Found", server, clientSocket);
	}
	const t_route &route = *routeptr;
	std::cout << "     route found: " << route.path << std::endl;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeErrorResponse(405, "Method Not Allowed", server, clientSocket);
	}
	if (route.cgi.empty() == false)
	{
		Cgi cgi(route.cgi, req.get_method(), req.getFirstQueryString());
		return cgi.handleCgi(server.root, server.error, clientSocket);
	}
	else {
		std::string filename;
		if (uri == route.path && route.autoindex)
			filename = route.directory + "/" + route.index;
		else
			filename = route.directory + "/" + uri.erase(0, route.path.length());
		if (isDirectory(filename)) {
			if (filename[filename.length() - 1] != '/')  //if no final slash on directory: redirection 301 with uri + '/'
				return makeErrorResponse(301, "Moved Permanently", server, clientSocket);
			std::ostringstream htmlContent;
			htmlContent << serverObj.getDisplayDirHtmlP1();
			readDirectory(filename, htmlContent, ",", "''");
			htmlContent << serverObj.getDisplayDirHtmlP2();
			return makeResponse(200, "OK", "text/html", htmlContent.str());
		}
		ssize_t filesize = getFileSize(filename);
		std::cerr << "     filename: '" << filename << "'  filesize: " << filesize << "\n";
		if (filesize == -1)
			return makeErrorResponse(404, "Not Found", server, clientSocket);
		std::string header = makeResponseHeader(200, "OK", getContentType(uri), filesize, clientSocket);
		return new OutgoingData(header, filename, true);
	}
}

OutgoingData * Response::handlePost(const t_server & server, const HTTPRequest & req, int clientSocket)
{
	(void)clientSocket;
	size_t	maxBodySize = server.max_body_size;
	std::string			uri = req.getUriWithoutQString();
	const t_route		*routeptr = getRouteFromUri(server, uri);
	std::map<std::string, std::string>::const_iterator contentType = req.getHeaders().find("Content-Type");

	if (routeptr == NULL) {
		std::cout << "     no route found from uri: " << uri << std::endl;
		return makeErrorResponse(404, "Not Found", server, clientSocket);
	}
	const t_route &route = *routeptr;
	std::cout << "     route found: " << route.path << std::endl;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeErrorResponse(405, "Method Not Allowed", server, clientSocket);
	}
//	req.printRequest();
	if (route.cgi.empty() == false)
	{
		Cgi cgi(route.cgi, req.get_method(), req.getFirstQueryString());
		return cgi.handleCgi(server.root, server.error, clientSocket);
	}
	else if (contentType == req.getHeaders().end())
		return makeErrorResponse(404, "Not Found", server, clientSocket);
	else if (contentType->second.find("text/plain") != std::string::npos)
		return handleTextPost(req, maxBodySize);
	else if (contentType->second.find("multipart/form-data") != std::string::npos)
		return handleFileUpload(req, maxBodySize);
	else if (contentType->second.find("application/x-www-form-urlencoded") != std::string::npos)
		return handleUrlEncodedForm(req, maxBodySize);
	else if (contentType->second.find("application/json") != std::string::npos)
		return handleJsonPost(req, maxBodySize);
	else
		return makeResponse(415, "Unsupported Media Type", "text/plain", "415 Unsupported Media Type: " + contentType->second);
}

OutgoingData * Response::handleDelete(const t_server & server, const HTTPRequest & req, int clientSocket)
{
	std::cerr << "\nDELETE REQUEST\n\n";
	std::string uri = req.getUriWithoutQString();
	const t_route *routeptr = getRouteFromUri(server, uri);
	if (routeptr == NULL) {
		std::cerr << "     no route found from uri: " << uri << std::endl;
		return makeErrorResponse(404, "Not Found", server, clientSocket);
	}
	std::cerr << uri << std::endl;
	const t_route &route = *routeptr;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeErrorResponse(405, "Method Not Allowed", server, clientSocket);
	}
	std::cerr << "     route found: " << route.path << std::endl;
	if (route.path == "/delete")
	{
		if (req.getFirstQueryString().find("..") != std::string::npos)
			return makeErrorResponse(403, "Forbidden", server, clientSocket);
 		if (std::remove(( "./www/html/uploadedFiles/" + req.getFirstQueryString()).c_str()) != 0)
			return makeErrorResponse(404, "Not Found", server, clientSocket);
		else
			return makeResponse(200, "OK", "text/plain", "200 OK");
	}
	else
		return makeErrorResponse(404, "Not Found", server, clientSocket);
}
