#include"Encoder.h"

Encoder::Encoder(int n, int k, int dataLength, int randomSeed)
{
	this->datalength = dataLength;
	this->n = n;
	this->k = k;
	this->codeBook = CodeBook(randomSeed, n, k).generateCodeBook();

}
Encoder::~Encoder() {};
char* Encoder::encode(char **block_buffer, int groupId, int blockId)
{
	char *buffer = (char *)malloc(sizeof(char)*(this->datalength + 2));//file buffer
	memset(buffer, '\0', this->datalength + 2);
	for (int j = 0; j < this->k; j++)
	{
		if (this->codeBook[blockId][j] == 1) {

			for (int index = 0; index < this->datalength; index++)
			{
				buffer[index + 2] ^= block_buffer[j][index];
			}
		}
	}
	buffer[0] = groupId - 128;//组号
	buffer[1] = blockId;//块内序号，用于查找codeBook
	return buffer;

}
std::vector<std::vector<char>> Encoder::encodeAll(const std::vector<std::vector<char>>&block_buffer, int groupId) {
	std::vector<std::vector<char>> code(64);
	for (int i = 0; i < 64; i++)
	{

		code[i].resize(this->datalength+2);

	}
	for (int n = 0; n < 64; n++)
	{
		for (int j = 0; j < this->k; j++)
		{
			if (this->codeBook[n][j] == 1) {

				for (int index = 0; index < this->datalength; index++)
				{
					code[n][index + 2] ^= block_buffer[j][index];
				}
			}
		}
		code[n][0] = groupId - 128;//组号
		code[n][1] = n;//块内序号，用于查找codeBook
	}
	return code;

}
