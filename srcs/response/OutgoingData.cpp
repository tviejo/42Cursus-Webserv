#include "OutgoingData.hpp"

OutgoingData::OutgoingData(const std::string &header, const std::string &body)
{
	_bufptr = _buffer;
	_buflen = 0;
	_header = new std::istringstream(header); 
	_body = new std::istringstream(body);
	loadBuffer();
}

/* fromFileYes value has no importance (it semantically should be true)
*/
OutgoingData::OutgoingData(const std::string &header, const std::string &filename, bool fromFileYes)
{
	(void)fromFileYes;  // by design: fromFileYes arg exists only to differentiate this ctor from the other
	_bufptr = _buffer;
	_buflen = 0;
	_body = new std::ifstream(filename.c_str(), std::ios_base::binary | std::ios_base::in);
	_header = new std::istringstream(header); 
	loadBuffer();
}

OutgoingData::~OutgoingData()
{
	delete _header;
	delete _body;
}

bool	OutgoingData::hasRemainingData()
{
	return (*_header || *_body);
}

void	OutgoingData::loadBuffer()
{
	if (*_header) {
		_header->read(_buffer, sizeof(_buffer));
		_buflen = _header->gcount();
		_bufptr = _buffer;
	} else if (*_body) {
		_body->read(_buffer, sizeof(_buffer));
		_buflen = _body->gcount();
		_bufptr = _buffer;
	}
}

void	OutgoingData::bufferForward(unsigned int bytes)
{
	if (bytes > _buflen)
		throw std::runtime_error("OutgoingData: buffer access out of range !");
	_bufptr += bytes;
	_buflen -= bytes;
}

char *	OutgoingData::getbufptr()
{
	return _bufptr;
}

int		OutgoingData::getbuflen()
{
	return _buflen;
}
