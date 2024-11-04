#include "IngoingData.hpp"

//IngoingData::IngoingData() {}
//IngoingData::~IngoingData() {}

void	IngoingData::processBuffer(int nbBytes)
{
	_data.write(_buffer, nbBytes);
}