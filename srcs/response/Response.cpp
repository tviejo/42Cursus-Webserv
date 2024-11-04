/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:48:44 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 17:09:08 by ade-sarr         ###   ########.fr       */
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
						 size_t contentLength, int ID_cookie)
{
	std::ostringstream	response;

	response << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
	if (ID_cookie != -1)
		response << "Set-Cookie: ID=" << ID_cookie << "; Path=/ \r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << contentLength << "\r\n";
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
	if (!addHeader.empty())
		response << addHeader << "\r\n";
	response << "\r\n";
	return new OutgoingData(response.str(), content);
}

OutgoingData *Response::makeErrorResponse(uint32_t status, const std::string &statusMessage, const t_server &server, int clientId)
{
	std::string	filename;
	ssize_t		filesize;
	
	if (status == 404)  // 404 special error page (Amstrad CPC 464 style !)
	{
		filename = server.root + server.error;
		size_t lastSlashPos = filename.find_last_of('/');
		if (lastSlashPos != std::string::npos)
		{
			filename.erase(lastSlashPos);
			filename += "/404.html";
			if ((filesize = getFileSize(filename)) > 0)
			{
				//std::cout << "filename: " << filename << "   filesize: " << filesize << std::endl;
				std::string header = makeResponseHeader(status, statusMessage, "text/html", filesize, clientId);
				return new OutgoingData(header, filename, true);
			}
		}
	}
	filename = server.root + server.error;
	filesize = getFileSize(filename);
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
	std::string header = makeResponseHeader(status, statusMessage, getContentType("html"), htmlContent.size(), clientId);
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

OutgoingData * Response::handleGet(const HTTPRequest & req, int clientSocket, Server &serverObj)
{
	const t_server & server = req.getServer();
	std::string uri = req.getUriWithoutQString();
	// Process routes redirections in first place
	std::map<std::string,std::string>::const_iterator it = server.redirs.begin();
	for (; it != server.redirs.end(); it++)
		if (it->first == uri)
			return makeResponse(301, "Moved Permanently", "text/plain", "301 Moved Permanently",
				"location: " + it->second);
	
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
	if (!route.cgi.empty())
	{
		Cgi cgi(route.cgi, req, clientSocket);
		return cgi.handleCgi();
	}
	
	// Test request for maximum upload size
	if (route.path == "/upload" && !req.getQueryStrings("getMaxUploadSize").empty())
		return makeResponse(200, ltoa(route.max_body_size), "", "");
	
	std::string filename;
	if (uri == route.path && route.autoindex)
		filename = route.directory + "/" + route.index;
	else
		filename = route.directory + "/" + uri.substr(route.path.length());
	if (isDirectory(filename)) {
		if (!route.dir_listing)
			return makeErrorResponse(405, "Directory listing not allowed", server, clientSocket);
		std::cout << "     uri: " << uri << "\n";
		std::cout << "     directory: " << filename << "\n";
		if (uri[uri.length() - 1] != '/')  //if no final slash on directory: redirection 301 with uri + '/'
			return makeResponse(301, "Moved Permanently", "text/plain", "301 Moved Permanently",
				"location: " + uri + '/');
		std::ostringstream htmlContent;
		htmlContent << serverObj.getDisplayDirHtmlP1();
		readDirectory(filename, htmlContent, ",", "''");
		htmlContent << serverObj.getDisplayDirHtmlP2();
		return makeResponse(200, "OK", "text/html", htmlContent.str());
	}
	ssize_t filesize = getFileSize(filename);
	std::cout << "     filename: '" << filename << "'  filesize: " << filesize << "\n";
	if (filesize == -1)
		return makeErrorResponse(404, "Not Found", server, clientSocket);
	std::string header = makeResponseHeader(200, "OK", getContentType(uri), filesize, clientSocket);
	return new OutgoingData(header, filename, true);
}

OutgoingData * Response::handlePost(HTTPRequest & req, int clientSocket)
{
	const t_server & server = req.getServer();
	std::string			uri = req.getUriWithoutQString();
	const t_route		*routeptr = getRouteFromUri(server, uri);
	
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
	size_t	maxBodySize = route.max_body_size;
	map_str_cite it = req.getHeaders().find("Content-Type");
	std::string contentType = (it == req.getHeaders().end()) ? "" : it->second;
	
	if (!route.cgi.empty())
	{
		// POST form processing for CGI and store form fields to the request
		if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		{
			int status;
			OutgoingData *resp = handleUrlEncodedForm(req, maxBodySize, &req.getForm(), &status);
			if (status != 201)
				return resp;
			delete resp;  // POST form processing is OK ; now go with cgi
		}
		Cgi cgi(route.cgi, req, clientSocket);
		return cgi.handleCgi();
	}
	else if (contentType.empty())
		return makeErrorResponse(204, "No Content", server, clientSocket);
	else if (contentType.find("text/plain") != std::string::npos)
		return handleTextPost(req, maxBodySize);
	else if (contentType.find("multipart/form-data") != std::string::npos)
		return handleFileUpload(req, maxBodySize);
	else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		return handleUrlEncodedForm(req, maxBodySize);
	else if (contentType.find("application/json") != std::string::npos)
		return handleJsonPost(req, maxBodySize);
	else
		return makeResponse(415, "Unsupported Media Type", "text/plain", "415 Unsupported Media Type: " + contentType);
}

OutgoingData * Response::handleDelete(const HTTPRequest & req, int clientSocket)
{
	std::cerr << "\nDELETE REQUEST\n\n";
	const t_server & server = req.getServer();
	std::string uri = req.getUriWithoutQString();
	const t_route *routeptr = getRouteFromUri(server, uri);
	if (routeptr == NULL) {
		std::cerr << "     no route found from uri: " << uri << std::endl;
		return makeErrorResponse(404, "Not Found", server, clientSocket);
	}
	const t_route &route = *routeptr;
	std::cerr << "     route found: " << route.path << std::endl;
	if (route.methods.find(req.get_method()) == route.methods.end())
	{
		std::cout << "     Unauthorized method: " << req.get_method() << " for route: " << route.path << std::endl;
		return makeErrorResponse(405, "Method Not Allowed", server, clientSocket);
	}
	if (route.path == "/delete")
	{
		if (req.getFirstQueryString().find("..") != std::string::npos)
			return makeErrorResponse(403, "Forbidden", server, clientSocket);
 		/*std::string uploadDirectory = server.routes.count("/upload") > 0 ?
									  server.routes.at("/upload").upload : "";
		if (uploadDirectory.empty())
			return makeErrorResponse(404, "Not Found", server, clientSocket);*/
		std::string filePathToDelete = route.directory + '/' + req.getFirstQueryString(true);
		std::cout << "deleting : " << filePathToDelete << std::endl; 
		if (std::remove(filePathToDelete.c_str()) != 0)
			return makeErrorResponse(404, "Not Found", server, clientSocket);
		else
			return makeResponse(200, "OK", "text/plain", "200 OK");
	}
	else
		return makeErrorResponse(404, "Not Found", server, clientSocket);
}
