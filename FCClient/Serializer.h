#pragma once
#include<vector>
//�����ݽ��в��������ײ�����
class Serializer
{
public:
	Serializer(int blockLength=1024);
	~Serializer();
	char * addHeader(unsigned int dataLength, char *rawdata);
	//����1024����Ϊ1024 dataLengthΪ��Ч�ֽڳ���

private:
	int blockLength;
	

};

