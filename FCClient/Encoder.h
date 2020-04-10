#pragma once
#include"CodeBook.h"
#include<vector>
class Encoder
{
public:
	Encoder(int n = 128, int k = 64, int dataLength = 1028, int randomSeed = 23);
	~Encoder();
	std::vector<std::vector<char>> encode();
	char* encode(char **block_buffer, int groupId, int blockId);
	std::vector<std::vector<char>> encodeAll(const std::vector<std::vector<char>>&block_buffer, int groupId);

private:
	unsigned int n;//Âë·ûºÅ
	unsigned int k;//Ô´·ûºÅ
	unsigned int datalength;
	std::vector<std::vector<int>>codeBook;
	std::vector<char> rawData;
	std::vector<std::vector<char>> encodedCode;
};

