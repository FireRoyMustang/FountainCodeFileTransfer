#include "CodeBook.h"
#include<time.h>
#include<iostream>
CodeBook::CodeBook(int randomSeed, int n, int k)
{
	srand(randomSeed);
	this->n = n;
	this->k = k;
}
CodeBook::CodeBook(int n, int k)
{
	srand(time(NULL));
	this->n = n;
	this->k = k;
}

CodeBook::~CodeBook()
{
}
std::vector<std::vector<int>> CodeBook::generateCodeBook()
{
	std::vector < std::vector < int> > codeBook;
	for (int row = 0; row < this->n; row++)
	{
		std::vector<int>temp;
		for (int col = 0; col < this->k; col++)
		{
			int a = rand() % 2 < 1;
			temp.push_back(a);
		}
		codeBook.push_back(temp);
	}
	return codeBook;
}
