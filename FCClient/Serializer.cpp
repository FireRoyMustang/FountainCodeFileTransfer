#include "Serializer.h"
#include<iostream>
Serializer::Serializer(int blockLength)
{
	this->blockLength = blockLength;
}

Serializer::~Serializer()
{
}
char * Serializer::addHeader(unsigned int dataLength, char *rawdata)
{
	char *paddingData = (char *)malloc(sizeof(char)*(this->blockLength + 4));//file buffer
	memset(paddingData, '\0', this->blockLength + 4);
	int data_length;
	memcpy(paddingData, &dataLength, sizeof(int));
	memcpy(paddingData + 4, rawdata, dataLength);
	//int l;
	//memcpy(&l, paddingData, sizeof(int));

	//std::cout << "Data Length:" << l << std::endl;
	return paddingData;
}