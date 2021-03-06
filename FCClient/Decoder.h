#pragma once
#include<vector>
class Decoder
{
public:
	Decoder(int randomSeed=23, int n = 128, int k = 64, int blockLength = 1028);
	~Decoder();
	char ** decode();
	void addBuffer(char* blockData);
	int nonZero(std::vector<int> & data);


private:
	std::vector<std::vector<int>> codeBook;
	std::vector<std::vector<char>> bufferData;
	int GaussMatrix[64][64] = { 0 };//��˹��Ԫ����
	int n;
	int k;
	int blockLength;
	bool success;

};

