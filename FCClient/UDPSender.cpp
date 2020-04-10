#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <iostream>
#include"Encoder.h"
#include"Serializer.h"
#include<ctime>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define BUF_MAXLEN 1024
#define BUFFER_SIZE 1024
#define RAWDATA_SIZE 1028
#define BLOCK_SIZE 64

int main(int argc, char* argv[])
{
	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		cout << "Init Windows Socket Failed::" << GetLastError() << endl;
		return 0;
	}
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int len = sizeof(sin);
	char choose[10];
	cout << "Do you want to    1. Transfer message  OR  2. Send the file (choose 1# OR 2#)" << endl;
	cin >> choose;
	getchar();

	char sendData[BUF_MAXLEN];
	char recvData[BUF_MAXLEN];
	if (!strncmp(choose, "1#", 2))
	{
		sendto(sclient, choose, strlen(choose), 0, (sockaddr *)&sin, len);
		char  sendData_[10] = "HELLO!";
		sendto(sclient, sendData_, strlen(sendData_), 0, (sockaddr *)&sin, len);//发送hello
		cout << "Client OK!" << endl;

		memset(recvData, 0x00, sizeof(recvData));
		int ret = recvfrom(sclient, recvData, BUF_MAXLEN, 0, (sockaddr *)&sin, &len);//接收hello
		cout << "--------------------------------" << recvData << "--------------------------------" << endl;
		sendto(sclient, choose, strlen(choose), 0, (sockaddr *)&sin, len);//再次传输选择，使线程选择模式

		while (true)
		{
			char info[100];

			cout << "Input '#!#' to quit OR Press any key to continue. " << endl;
			cin >> info;//选择
			getchar();//吃掉回车
			if (!strncmp(info, "#!#", 3))
			{
				break;
			}

			cout << "------------------------------SENDING...------------------------------" << endl;
			cout << "Send Message:";
			memset(sendData, 0x00, sizeof(sendData));
			cin >> sendData;
			getchar();
			sendto(sclient, sendData, strlen(sendData), 0, (sockaddr *)&sin, len);//发送数据
			cout << "Client OK!" << endl;
			SYSTEMTIME st;
			GetLocalTime(&st);
			cout << "Linking Time:" << st.wYear << "/" << st.wMonth << "/" << st.wDay << "  " << st.wHour << ":" << st.wMinute << endl;
			cout << "Server:" << inet_ntoa(sin.sin_addr) << ":" << ntohs(sin.sin_port) << " Connection!" << endl;

			memset(recvData, 0x00, sizeof(recvData));
			ret = recvfrom(sclient, recvData, BUF_MAXLEN, 0, (sockaddr *)&sin, &len);//回显数据
			if (ret > 0)
			{
				cout << "Receive From Server: " << recvData << endl;
				cout << "----------------------------------------------------------------------" << endl;
				cout << endl;
				memset(recvData, 0x00, sizeof(recvData));
			}
		}
	}
	else if (!strncmp(choose, "2#", 2))
	{
		sendto(sclient, choose, strlen(choose), 0, (sockaddr *)&sin, len);
		char  sendData_[10] = "HELLO!";
		sendto(sclient, sendData_, strlen(sendData_), 0, (sockaddr *)&sin, len);//发送hello
		cout << "Client OK!" << endl;

		memset(recvData, 0x00, sizeof(recvData));
		int ret = recvfrom(sclient, recvData, BUF_MAXLEN, 0, (sockaddr *)&sin, &len);//接收hello
		cout << "--------------------------------" << recvData << "--------------------------------" << endl;
		sendto(sclient, choose, strlen(choose), 0, (sockaddr *)&sin, len);//再次传输选择，使线程选择模式
		while (true)
		{
			char info[100];
			cout << "Input '#!#' to quit OR Press any key to continue. " << endl;
			cin >> info;
			getchar();
			if (!strncmp(info, "#!#", 3))
			{
				break;
			}
			FILE *fp;
			char file_name[MAX_PATH];

			cout << "------------------------------SENDING...------------------------------" << endl;
			cout << "Please input the filename:" << endl;
			cin >> file_name;
			//传送文件名
			sendto(sclient, file_name, strlen(file_name), 0, (sockaddr *)&sin, len);
			if (!(fp = fopen(file_name, "rb")))
			{
				cout << "File " << file_name << " can't open" << endl;
				continue;
			}
			//
			char ** rawdata = (char **)malloc(sizeof(char *)*BLOCK_SIZE);
			Encoder encoder = Encoder();
			Serializer serializer = Serializer(BUFFER_SIZE);
			char *buffer = (char *)malloc(sizeof(char)*(BUFFER_SIZE));//file buffer
			int times = 1;
			int groupID = 0;
			//memset(sendData, 0x00, sizeof(sendData));
			//传送文件
			int length;
			clock_t startTime, endTime;
			startTime = clock();
			while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
			{
				rawdata[times - 1] = serializer.addHeader(length, buffer);
				if (times == BLOCK_SIZE)
				{
					for (int blockId = 0; blockId < 100; blockId++)
					{
						//发送数据
						ret = sendto(sclient, encoder.encode(rawdata, groupID, blockId), BUFFER_SIZE + 6, 0, (sockaddr *)&sin, len);
						//ret = sendto(sclient, sendData, length, 0, (sockaddr *)&sin, len);
						if (ret == SOCKET_ERROR || ret == 0)
						{
							cout << "wrong" << endl;
							return -1;
						}
					}
					//Sleep(30);
					times = 0;
					groupID++;
				}
				times++;
			}
			if (times < BLOCK_SIZE)
			{
				for (int i = times; i < BLOCK_SIZE; i++)
				{
					rawdata[i - 1] = serializer.addHeader(length, buffer);
				}
				for (int blockId = 0; blockId < 100; blockId++)
				{
					ret = sendto(sclient, encoder.encode(rawdata, groupID, blockId), BUFFER_SIZE + 6, 0, (sockaddr *)&sin, len);
				}
			}
			Sleep(10);
			ret = sendto(sclient, buffer, 0, 0, (sockaddr *)&sin, len);
			endTime = clock();
			cout << "Run Time:" << endTime - startTime << "ms" << endl;
			if (ret == SOCKET_ERROR)
			{
				cout << "wrong" << endl;
				return -1;
			}
			else
			{
				cout << "send successful!" << endl;
			}
			fclose(fp);
			//表明传输时间，对象
			SYSTEMTIME st;
			GetLocalTime(&st);
			cout << "Linking Time:" << st.wYear << "/" << st.wMonth << "/" << st.wDay << "  " << st.wHour << ":" << st.wMinute << endl;
			cout << "Server:" << inet_ntoa(sin.sin_addr) << ":" << ntohs(sin.sin_port) << " Connection!" << endl;
			cout << "----------------------------------------------------------------------" << endl;
		}
	}

	closesocket(sclient);
	WSACleanup();
	return 0;
}
