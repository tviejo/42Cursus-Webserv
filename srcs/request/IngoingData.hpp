#pragma once

#include "webserv.hpp"

class IngoingData
{
private:	
	HTTPRequest		*_request;
	std::ostream	*_content;
	char			_buffer[IO_BUFFER_SIZE];
	char			*_bufptr;
	unsigned int	_buflen;

public:
	IngoingData(const std::string &header, const std::string &body);
	IngoingData(const std::string &header, const std::string &fileName, bool fromFileYes);
	~IngoingData();
	void	loadBuffer();
	void	bufferForward(unsigned int bytes);
	char	*getbufptr();
	int		getbuflen();
	bool	hasRemainingData();
};
