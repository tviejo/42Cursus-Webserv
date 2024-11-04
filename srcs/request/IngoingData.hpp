#pragma once

#include "webserv.hpp"

enum receivingState { rsHeader, rsBody };

class IngoingData
{
private:
	static const size_t	_maxHeaderSize = 8192;
	HTTPRequest			*_request;
	std::stringstream	_data;
	char				_buffer[IO_BUFFER_SIZE];
	//char				*_bufptr;
	//unsigned int		_buflen;
	size_t				_headerSize;
	size_t				_bodySize;
	size_t				_maxBodySize;
	receivingState		_state;

public:
	IngoingData() {}
	~IngoingData() {}
	char 	*getBuffer() { return _buffer; }
	void 	processBuffer(int nbBytes);
};
