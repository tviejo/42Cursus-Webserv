#pragma once

#include "webserv.hpp"

class OutgoingData
{
private:	
	std::istream	*_header;
	std::istream	*_body;
	char			_buffer[IO_BUFFER_SIZE];
	char			*_bufptr;
	unsigned int	_buflen;

public:
	OutgoingData(const std::string &header, const std::string &body);
	OutgoingData(const std::string &header, const std::string &fileName, bool fromFileYes);
	~OutgoingData();
	void	loadBuffer();
	void	bufferForward(unsigned int bytes);
	char	*getbufptr();
	int		getbuflen();
	bool	hasRemainingData();
};
