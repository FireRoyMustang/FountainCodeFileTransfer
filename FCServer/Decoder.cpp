#include"Decoder.h"
#include<iostream>
#include"CodeBook.h"
Decoder::Decoder(int randomSeed, int n, int k, int blockLength)
{

	this->blockLength = blockLength;
	this->n = n;
	this->k = k;
	this->randomSeed = randomSeed;
	this->success = false;
	/*for (int i = 0; i < 64; i++)
	{
		std::cout << GaussMatrix[i][5] << std::endl;
	}*/

}


Decoder::~Decoder()
{
}
char ** Decoder::decode()
{
	this->codeBook = CodeBook(this->randomSeed, this->n, this->k).generateCodeBook();
	char ** decodedCode = (char **)malloc(sizeof(char *)*this->k);
	for (int i = 0; i < this->k; i++)
	{
		decodedCode[i] = (char *)malloc(sizeof(char)*this->blockLength);
		memset(decodedCode[i], 0, this->blockLength);
	}

	for (int i = 0; i < this->bufferData.size(); i++)
	{
		int blockId = this->bufferData[i][1];
		std::vector<int> code = this->codeBook[blockId];
		char *codeword = this->bufferData[i].data() + 2;
		int nonZeroIndex = this->nonZero(code);
		while (nonZeroIndex < 64)
		{
			//	std::cout << nonZeroIndex << std::endl;
			if (this->GaussMatrix[nonZeroIndex][nonZeroIndex] == 0)
			{
				std::copy(code.begin(), code.end(), this->GaussMatrix[nonZeroIndex]);
				memcpy(decodedCode[nonZeroIndex], codeword, 1028);
				break;
			}
			else
			{
				for (int j = 0; j < this->k; j++)
				{
					code[j] = code[j] ^ this->GaussMatrix[nonZeroIndex][j];
				}
				for (int j = 0; j < this->blockLength; j++)
				{
					codeword[j] = codeword[j] ^ decodedCode[nonZeroIndex][j];
				}
				nonZeroIndex = nonZero(code);
			}

		}
	}
	for (int i = this->k - 1; i >= 0; i--)
	{
		char *temp = decodedCode[i];
		for (int j = 0; j < i; j++)
		{
			if (this->GaussMatrix[j][i] == 1)
			{
				for (int d = 0; d < this->blockLength; d++)
				{
					decodedCode[j][d] = decodedCode[j][d] ^ temp[d];
				}
			}
		}
	}
	this->success = true;
	//²âÊÔÓÃ
	/*std::cout << "Code Size" << ":" << bufferData.size() << std::endl;
	for (int i = 0; i < 64; i++)
	{
		//std::cout << (int)decodedCode[0][1026] << " " << (int)decodedCode[63][1026] << std::endl;
		std::cout << "decoded" << i << ":" << (int)decodedCode[0][i] << std::endl;
	}*/

	return decodedCode;
}
bool Decoder::addBuffer(char* blockData)
{
	if (this->success)
		return false;

	this->bufferData.push_back(std::vector<char>(blockData, blockData + this->blockLength + 2));
	if (bufferData.size() >= 68)
	{
		this->success = true;
		return true;
	}
}
int Decoder::nonZero(std::vector<int> & data)//·µ»Ø×î¸ß·ÇÁãÎ»
{
	for (int i = 0; i < data.size(); i++)
	{
		if (data[i] != 0)
		{
			return i;
		}
	}
	return data.size();
}
