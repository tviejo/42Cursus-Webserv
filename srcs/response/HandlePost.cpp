#include "webserv.hpp"
#include "Response.hpp"

//static std::string	extractFilename(const std::string& body, const std::string& filenameKey)
//{
//	size_t filenameStart = body.find(filenameKey);
//
//	if (filenameStart == std::string::npos)
//		throw std::runtime_error("No filename for uploaded file");
//	filenameStart += filenameKey.length();
//
//	size_t	filenameEnd = body.find("\"", filenameStart);
//	if (filenameEnd == std::string::npos)
//		throw std::runtime_error("No filename for uploaded file");
//
//	return body.substr(filenameStart, filenameEnd - filenameStart);
//}

static	std::string getDate(const std::string& body)
{
	size_t dateStart = body.find("Date: ");

	if (dateStart == std::string::npos)
		return "01.01.1990";
	dateStart += 6;
	size_t dateEnd = body.find("G", dateStart);
	dateEnd -= 2;
	if (dateEnd == std::string::npos)
		return "01.01.1990";
	return body.substr(dateStart, dateEnd - dateStart);
}

std::string sanitizeFileName(const std::string& fileName)
{
	std::string safe = fileName;
	std::string banned = "\\/:*?\"<>|";
	for (size_t i = 0; i < banned.size(); i++)
		std::replace(safe.begin(), safe.end(), banned[i], '_');
	return safe;
}

OutgoingData*	Response::handleTextPost(const HTTPRequest& req, const t_route& route)
{
	try
	{
		const std::string&	content = req.getBody();
		std::string			fileName = getDate(req.getBody()) + ".txt";
		std::string			safeFileName = sanitizeFileName(fileName);
		const std::string	path = "www/upload/" + fileName;
		std::string			line;
		//set max length to protect against ddos
		//check if directory exist and handle it if it does not
		//check if file already exists and handle that case
		//sanitize filename
		std::ofstream		outFile(path.c_str(), std::ios::out | std::ios::binary);
		if (!outFile)
			throw std::runtime_error("Could not open file: " + path);
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
		
	while (true)
	{
		size_t	boundaryPos = body.find(boundary, pos);
		if (boundaryPos == std::string::npos)
			break;
		pos = boundaryPos + boundary.length() + 2;
		if (body.substr(boundaryPos + boundary.length(), 4) == "--\r\n")
			break ;

		formPart	part;
		while (pos < body.length())
		{
			size_t	lineEnd = body.find("\r\n", pos);
			if (lineEnd == pos)
			{
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
		size_t	contentEnd = body.find("\r\n" + boundary, pos);
		if (contentEnd == std::string::npos)
			break;
		part.content = body.substr(pos, contentEnd - pos);
		pos = contentEnd + 2;
		partsVec.push_back(part);
	}
	return partsVec;
}



OutgoingData*	Response::handleFileUpload(const HTTPRequest& req, const t_route& route)
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

		//create upload directory here if needed
		std::string					uploadDir = "www/upload";
		std::vector<std::string>	uploadedFiles;

		for (std::vector<formPart>::iterator it = parts.begin(); it != parts.end(); it++)
		{
			if (it->fileName.empty())
				continue;
			if (it->content.size() > 10 * 1024 * 1024) // 10 MB
				return makeResponse(413, "Payload Too Large", "text/plain",
						"File size exceeds maximum allowed");
			std::string safeFileName = sanitizeFileName(it->fileName);
			std::string fullPath = uploadDir + "/" + safeFileName;
			
			//handle duplicate  file names here with access()

			std::ofstream outFile(fullPath.c_str(), std::ios::binary);
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
	
		std::string response = "Successfully uploaded " + std::to_string(uploadedFiles.size()) + " file(s):\n";
		for (std::vector<std::string>::const_iterator it = uploadedFiles.begin(); it != uploadedFiles.end(); it++)
			response += "- " + *it + "\n";
		return makeResponse(201, "Created", "text/plain", response);
	}
	catch (std::exception& e)
	{
		return makeResponse(500, "Internal Server Error", "text/plain",
					  std::string("Failed to process upload: ") + e.what());
	}
}
 
OutgoingData*	Response::handleFormSubmission(const HTTPRequest& req, const t_route& route)
{
}

OutgoingData*	Response::handleJsonPost(const HTTPRequest& req, const t_route& route)
{
}
