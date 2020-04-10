#pragma once
#include<vector>
//对数据进行补零和添加首部操作
class Serializer
{
public:
	Serializer(int blockLength=1024);
	~Serializer();
	char * addHeader(unsigned int dataLength, char *rawdata);
	//不足1024补齐为1024 dataLength为有效字节长度

private:
	int blockLength;
	

};

