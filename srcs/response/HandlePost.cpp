#include "webserv.hpp"
#include "Response.hpp"

static std::string	extractFilename(const std::string& body, const std::string& filenameKey)
{
	size_t filenameStart = body.find(filenameKey);

	if (filenameStart == std::string::npos)
		return "";
	filenameStart += filenameKey.length();

	size_t	filenameEnd = body.find("\"", filenameStart);
	if (filenameEnd == std::string::npos)
		return "";
	return body.substr(filenameStart, filenameEnd - filenameStart);
}

static	std::string getDate(const std::string& body)
{
	size_t dateStart = body.find("Date: ");

	if (dateStart == std::string::npos)
		return ""; //"01.01.1990";
	dateStart += 6;
	size_t dateEnd = body.find("G", dateStart);
	dateEnd -= 2;
	if (dateEnd == std::string::npos)
		return ""; //"01.01.1990";
	return body.substr(dateStart, dateEnd - dateStart);
}

static std::string getUpldFilesDir(const t_server &server)
{
	//return "www/uploadedFiles/";
	return	server.routes.count("/upload") > 0 ?
			server.routes.at("/upload").upload : "";
}

static std::string sanitizeFileName(const std::string& fileName)
{
	std::string safe = fileName;
	std::string banned = "\\/:*?\"<>|";
	for (size_t i = 0; i < banned.size(); i++)
		std::replace(safe.begin(), safe.end(), banned[i], '_');
	return safe;
}

OutgoingData*	Response::handleTextPost(const HTTPRequest& req, const size_t maxBodySize)
{
	try
	{
		const std::string&	content = req.getBody();
		std::string			fileName = getDate(req.getBody())
									 + extractFilename(req.getBody(), "filename=")
									 + "_" + ltoa(get_time()) + ".txt";
		std::string			safeFileName = sanitizeFileName(fileName);
		const std::string	path = getUpldFilesDir(req.getServer()) + "/" + safeFileName;
		std::string			line;
		std::ofstream		outFile(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		if (!outFile)
			throw std::runtime_error("Could not open file: " + path);
		if (content.length() > maxBodySize)
				return makeResponse(413, "Payload Too Large", "text/plain",
						"File size exceeds maximum allowed");
		outFile.write(content.c_str(), content.length());
		outFile.close();
		if (!outFile)
			throw std::runtime_error("Could not wrote to file: " + safeFileName);
		return makeResponse(201, "OK", "text/plain", "Text file processed succesfully: " + path);
	}
	catch (std::exception& e)
	{
		return makeResponse(400, "Bad Request", "text/plain", std::string("Failed to process text: ") + e.what());
	}
}

struct formPart
{
	std::string	name;
	std::string	fileName;
	std::string	contentType;
	std::string	content;
};

std::vector<formPart>	parseMultipartForm(const std::string& body, const std::string& boundary)
{
	std::vector<formPart>	partsVec;
	size_t					pos = 0;
		
	size_t	firstBoundary = body.find(boundary);
	if (firstBoundary == std::string::npos)
		return partsVec;
	pos = firstBoundary;
	while (pos < body.length())
	{
		size_t	boundaryPos = body.find(boundary, pos);
		if (boundaryPos == std::string::npos)
			break;
		pos = boundaryPos + boundary.length();
		if (pos + 2 > body.length())
			break ;
		if (body[pos] == '-' && body[pos + 1] == '-')
			break ;
		pos += 2;

		formPart	part;
		bool	headersEnd = false;
		while (!headersEnd && pos < body.length())
		{
			size_t	lineEnd = body.find("\r\n", pos);
			if (lineEnd == std::string::npos)
				break;
			if (lineEnd == pos)
			{
				headersEnd = true;
				pos += 2;
				break;
			}
			std::string	header = body.substr(pos, lineEnd - pos);
			pos = lineEnd + 2;
			if (header.find("Content-Disposition: ") == 0)
			{
				size_t	namePos = header.find("name=\"");
				if (namePos != std::string::npos)
				{
					namePos += 6;
					size_t	nameEnd = header.find("\"", namePos);
					if (nameEnd != std::string::npos)
						part.name = header.substr(namePos, nameEnd - namePos);
				}
				size_t filenamePos = header.find("filename=\"");
				if (filenamePos != std::string::npos)
				{
					filenamePos += 10;
					size_t	filenameEnd = header.find("\"", filenamePos);
						part.fileName = header.substr(filenamePos, filenameEnd - filenamePos);
				}
			}
			else if (header.find("Content-Type: ") == 0)
				part.contentType = header.substr(14);
		}
		size_t	nextBoundary = body.find(boundary, pos);
		if (nextBoundary == std::string::npos)
			nextBoundary = body.length() - 1;
		size_t	contentLength = nextBoundary - pos - 2;
		part.content = body.substr(pos, contentLength);
		partsVec.push_back(part);
		pos = nextBoundary;
	}
	return partsVec;
}

OutgoingData*	Response::handleFileUpload(const HTTPRequest& req, const size_t maxBodySize)
{
	try 
	{
		std::map<std::string, std::string>::const_iterator headerIt = req.getHeaders().find("Content-Type");
		if (headerIt == req.getHeaders().end())
			return makeResponse(400, "Bad Request", "text/plain",
					   "No boundary found in multipart/form-data");

		size_t boundaryPos = headerIt->second.find("boundary=");
		if (boundaryPos == std::string::npos)
			return makeResponse(400, "Bad Request", "text/plain",
					   "No boundary found in multipart/form-data");

		std::string				boundary = "--" + headerIt->second.substr(boundaryPos + 9);	
		std::vector<formPart>	parts = parseMultipartForm(req.getBody(), boundary);

		std::string				uploadDir = getUpldFilesDir(req.getServer());
		stringvec				uploadedFiles;
		for (std::vector<formPart>::iterator it = parts.begin(); it != parts.end(); it++)
		{
			if (it->fileName.empty())
				continue;
			if (it->content.size() > maxBodySize)
				return makeResponse(413, "Payload Too Large", "text/plain",
						"File size exceeds maximum allowed");
			std::string safeFileName = sanitizeFileName(it->fileName);
			std::string fullPath = uploadDir + "/" + safeFileName;
			std::ofstream outFile(fullPath.c_str(), std::ios::binary | std::ios::trunc);
			if (!outFile)
				throw std::runtime_error("Failed to create outFile: " + fullPath);
			outFile.write(it->content.data(), it->content.size());
			outFile.close();
			if (!outFile)
				throw std::runtime_error("Failed to write to outFile: " + fullPath);
			uploadedFiles.push_back(safeFileName);
		}
		if (uploadedFiles.empty())
			return makeResponse(400, "Bad Request", "text/plain",
					   "No files were uploaded");
		std::ostringstream ossResponse;
		ossResponse << "Successfully uploaded " << uploadedFiles.size() << " file(s):\n";
		for (stringvec::const_iterator it = uploadedFiles.begin(); it != uploadedFiles.end(); it++)
			ossResponse << " - " << *it << "\n";
		//ossResponse << "\r\n";  // this solve the problem of missing last char for last file name
		return makeResponse(201, "Created", "text/plain", ossResponse.str());
	}
	catch (const std::exception& e)
	{
		return makeResponse(500, "Internal Server Error", "text/plain",
					  std::string("Failed to process upload: ") + e.what());
	}
}

std::string Response::urlDecode(const std::string& encoded)
{
	std::string	result;
	for (size_t i = 0; i < encoded.length(); i++)
	{
		if ((encoded[i] == '%') && (i + 2 < encoded.length()))
		{
			std::string hexByte = encoded.substr(i + 1, 2);
			char ch = static_cast<char>(strtol(hexByte.c_str(), NULL, 16));
			result += ch;
			i += 2;
		}
		else if (encoded[i] == '+')
		{
			result += ' ';
		}
		else
			result += encoded[i];
	}
	return result;
}

OutgoingData*	Response::handleUrlEncodedForm(const HTTPRequest& req, size_t maxBodySize)
{
	std::string							body = req.getBody();
	std::string							fileName = getDate(body) + extractFilename(req.getBody(), "filename=");
	std::map<std::string, std::string>	formData;
	size_t								start = 0;
	size_t								end = body.find('&');

	while (start < body.length())
	{
		std::string	pair = body.substr(start, end - start);
		size_t		separator = pair.find('=');
		if (separator != std::string::npos)
		{
			std::string key = urlDecode(pair.substr(0, separator));
			std::string value = urlDecode(pair.substr(separator + 1));

			formData[key] = value;
		}
		if (end == std::string::npos)
			break;
		start = end + 1;
		end = body.find('&', start);
	}
	std::stringstream	ss;
	ss << "Received form data:\n";
	std::map<std::string, std::string>::const_iterator it = formData.begin();
	while (it != formData.end())
	{
		ss << it->first << ": " << it->second << "\n";
		it++;
	}
	if (ss.str().length() > maxBodySize)
		return makeResponse(413, "Payload Too Large", "text/plain",
				"File size exceeds maximum allowed");
	std::string fullPath = getUpldFilesDir(req.getServer()) + "/" + sanitizeFileName(fileName) + "_" + ltoa(get_time());
	std::ofstream outFile(fullPath.c_str(), std::ios::binary);
	if (!outFile)
		throw std::runtime_error("Failed to create outFile: " + fullPath);
	std::string	content = ss.str();
	outFile.write(content.c_str(), content.size());
	outFile.close();
	if (!outFile)
		throw std::runtime_error("Failed to write to outFile: " + fullPath);
	return makeResponse(201, "OK", "text/plain", "Form created succesfully: " + fullPath /*+ "\r\n"*/);
}

OutgoingData*	Response::handleJsonPost(const HTTPRequest& req, const size_t maxBodySize)
{
	try
	{
		const std::string&	content = req.getBody();
		if (content.empty())
			return makeResponse(400, "Bad Request", "application/json", "{\"error\": \"Empty request body\"}");
		if (content.length() > maxBodySize)
			return makeResponse(413, "Payload Too Large", "application/json", "{\"error\": \"Request body too large\"}");
		
		std::string		uploadDir = getUpldFilesDir(req.getServer());
		std::string		fileName = getDate(req.getBody()) + extractFilename(req.getBody(), "filename=")
								 + "_" + ltoa(get_time()) + ".json";
		std::string		fullPath = uploadDir + "/" + sanitizeFileName(fileName);
		std::ofstream	outFile(fullPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		if (!outFile)
			throw std::runtime_error("Could not create output file");
		outFile.write(content.c_str(), content.length());
		outFile.close();

		return makeResponse(201, "Created", "application/json",
					 "{\"status\": \"success\", \"file\": \"" + fileName + "\"}"/*\r\n"*/);
	}
	catch (const std::exception& e)
	{
		return makeResponse(500, "Internal Server Error", "application/json", "{\"error\"}: \"" + std::string(e.what()) + "\"}");
	}
}
