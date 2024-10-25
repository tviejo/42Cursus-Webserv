#include "webserv.hpp"
#include "Response.hpp"

static std::string	extractFilename(const std::string& body, const std::string& filenameKey)
{
	size_t filenameStart = body.find(filenameKey);

	if (filenameStart == std::string::npos)
		throw std::runtime_error("No filename for uploaded file");
	filenameStart += filenameKey.length();

	size_t	filenameEnd = body.find("\"", filenameStart);
	if (filenameEnd == std::string::npos)
		throw std::runtime_error("No filename for uploaded file");

	return body.substr(filenameStart, filenameEnd - filenameStart);
}

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

OutgoingData*	Response::handleTextPost(const HTTPRequest& req, const t_route& route)
{
	try
	{
		const std::string&	content = req.getBody();
		std::string			fileName = getDate(req.getBody()) + ".txt";
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
			throw std::runtime_error("Could not wrote to file: " + fileName);
		return makeResponse(201, "OK", "text/plain", "Text file processed succesfully: " + path);
	}
	catch (std::exception& e)
	{
		return makeResponse(400, "Bad Request", "text/plain", std::string("Failed to process text: ") + e.what());
	}
}

OutgoingData*	Response::handleFileUpload(const HTTPRequest& req, const t_route& route)
{
}

OutgoingData*	Response::handleFormSubmission(const HTTPRequest& req, const t_route& route)
{
}

OutgoingData*	Response::handleJsonPost(const HTTPRequest& req, const t_route& route)
{
}
