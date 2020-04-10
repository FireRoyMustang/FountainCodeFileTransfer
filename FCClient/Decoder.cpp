#include"Decoder.h"
#include"CodeBook.h"
#include<iostream>
Decoder::Decoder(int randomSeed, int n, int k, int blockLength)
{

	this->blockLength = blockLength;
	this->n = n;
	this->k = k;
	this->codeBook = CodeBook(randomSeed, n, k).generateCodeBook();
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

	char ** decodedCode = (char **)malloc(sizeof(char *)*this->k);
	for (int i = 0; i < this->k; i++)
	{
		decodedCode[i] = (char *)malloc(sizeof(char)*this->blockLength);
		//memset(decodedCode[i], '\0', this->blockLength);
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
				memcpy(decodedCode[nonZeroIndex], codeword, this->blockLength);
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
	//测试用
	for (int i = 0; i < 100; i++)
	{
		//std::cout << (int)decodedCode[0][1026] << " " << (int)decodedCode[63][1026] << std::endl;
		std::cout << "decoded" << i << ":" << (int)decodedCode[25][i] << std::endl;
	}
	
	return decodedCode;
	/*std::vector<std::vector<char>> decodedCode(64);
	for (int i = 0; i < 64; i++)
	{
		decodedCode[i].resize(this->blockLength);
	}
	for (int i = 0; i < this->bufferData.size(); i++)
	{
		int blockId = this->bufferData[i][1];
		std::vector<int> code = this->codeBook[blockId];
		char *codeword = this->bufferData[i].data() + 2;
		int nonZeroIndex = this->nonZero(code);
		while (nonZeroIndex < 64)
		{
			std::cout << nonZeroIndex << std::endl;
			if (this->GaussMatrix[nonZeroIndex][nonZeroIndex] == 0)
			{
				std::copy(code.begin(), code.end(), this->GaussMatrix[nonZeroIndex]);
				decodedCode[nonZeroIndex] = std::vector<char>(codeword, codeword + this->blockLength);
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
		std::vector<char> temp = decodedCode[i];
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
	for (int i = 900; i < 1028; i++)
	{
		//std::cout << (int)decodedCode[0][1026] << " " << (int)decodedCode[63][1026] << std::endl;
		std::cout << "decoded" << i << ":" << (int)decodedCode[27][i] << std::endl;
	}
	return decodedCode;*/
	return NULL;
}
void Decoder::addBuffer(char* blockData)
{
	if (this->success)
		return;

	this->bufferData.push_back(std::vector<char>(blockData, blockData + this->blockLength + 2));
	if (bufferData.size() >= 70)
	{
		this->decode();
	}
}
int Decoder::nonZero(std::vector<int> & data)//返回最高非零位
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