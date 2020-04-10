#pragma once
#include<vector>
class CodeBook
{
public:
	CodeBook(int randomseed,int n, int k);
	CodeBook(int n,int k);
	~CodeBook();
	std::vector<std::vector<int>> generateCodeBook();
private:
	unsigned int n;//Âë·ûºÅÊı
	unsigned int k;//Ô­·ûºÅÊı


};

