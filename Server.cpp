//
// server.cpp
//
#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include"Decoder.h"
#include<vector>
#include<map>
#include<utility>
#include<ctime>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

#define BUF_MAXLEN 1030
#define MAX_NAME 1024
#define BLOCK_SIZE 64


DWORD WINAPI ClientThread(LPVOID lpParameter)
{
	//服务端UDP套接字
	SOCKET ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in remoteAddr;
	remoteAddr = *((sockaddr_in *)lpParameter);

	int nAddrLen = sizeof(remoteAddr);
	//服务端端口绑定
	u_short newport = 8888;
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(newport);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	//尝试绑定新的端口
	while (bind(ClientSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		newport++;
		serAddr.sin_port = htons(newport);
	}

	char  sendData[10] = "HELLO!";
	sendto(ClientSocket, sendData, strlen(sendData), 0, (sockaddr *)&remoteAddr, nAddrLen);//发送hello

	char choose[100];
	memset(choose, 0, sizeof(choose));
	recvfrom(ClientSocket, choose, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen);//接收选择

	//初始化decoders
	std::map<int, Decoder> decodersMap;
	int ret;
	if (!strncmp(choose, "1#", 2))
	{
		while (true)
		{
			//memset(sendData,0x00,sizeof(sendData));
			char message[BUF_MAXLEN];
			memset(message, 0x00, sizeof(message));
			ret = recvfrom(ClientSocket, message, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen);//接收数据
			cout << "-----------------------------RECEIVING...-----------------------------" << endl;
			cout << "Receive Messages:" << message << endl;
			sendto(ClientSocket, message, strlen(message), 0, (sockaddr *)&remoteAddr, nAddrLen);//回显数据
			//表明传输时间对象
			SYSTEMTIME st;
			GetLocalTime(&st);
			cout << "Linking Time:" << st.wYear << "/" << st.wMonth << "/" << st.wDay << "  " << st.wHour << ":" << st.wMinute << endl;
			cout << "Client " << inet_ntoa(remoteAddr.sin_addr) << ":" << ntohs(remoteAddr.sin_port) << " Connection！" << endl;
			cout << "----------------------------------------------------------------------" << endl;
			cout << endl;
		}
	}
	else if (!strncmp(choose, "2#", 2))
	{
		while (true)
		{
			char recvData[BUF_MAXLEN];
			char file_name[MAX_NAME];
			//接收文件名
			ret = recvfrom(ClientSocket, file_name, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen);
			file_name[ret] = '\0';
			cout << "-----------------------------RECEIVING...-----------------------------" << endl;
			//打开文件
			FILE * fp;
			if (!(fp = fopen(file_name, "wb")))
			{
				cout << "File " << file_name << " can't open" << endl;
				return -1;
			}
			memset(recvData, 0, BUF_MAXLEN);
			int file_len = 0;//接收文件
			int file_pointer = 0;//文件指针
			map<int, char **> file_buffer;//接收缓存
			bool write_flag = false;
			int counts = 0;
			int groupId;
			int blockId;
			map<int, Decoder >::iterator iter;
			Decoder *decoder;
			clock_t startTime, endTime;
			startTime = clock();
			while (!write_flag && (file_len = recvfrom(ClientSocket, recvData, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen)))
			{
				//cout << "file_len = "<< file_len << endl;
				if (file_len == INVALID_SOCKET || file_len == 0)
				{
					cout << "Receive Error" << endl;
					return -1;
				}
				groupId = (int)recvData[0] + 128;
				blockId = (int)recvData[1];
				counts++;
				//cout << counts << ": groupId:" << groupId << " BlockId" << blockId << endl;
				iter = decodersMap.find(groupId);
				if (iter == decodersMap.end())
				{
					decodersMap.insert(pair<int, Decoder>(groupId, Decoder()));
					decoder = &decodersMap.find(groupId)->second;
				}
				else {
					decoder = &iter->second;
				}
				decoder->addBuffer(recvData);
				/*
				//写入文件缓存
				if (decoder.addBuffer(recvData)) {
					cout << counts << endl;
					file_buffer.insert(pair<int, char **>(groupId, decoder.decode()));
					//查询指针
					map<int, char ** >::iterator iter_file;
					iter_file = file_buffer.find(file_pointer);
					if (iter_file != file_buffer.end())//文件缓存中存在文件块
					{
						char ** write_data = iter_file->second;
						for (int i = 0; i < 64; i++)
						{
							int data_length = write_data[i][0];
							ret = fwrite(write_data, sizeof(char), data_length, fp);
							if (ret < 1028)
							{
								write_flag = true;
								break;
							}
							if (ret < file_len)
							{
								cout << file_name << " Failed" << endl;
								break;
							}
						}
						file_pointer++;
				}
			}*/
			//fflush(fp);//磁盘刷新
			}
			iter = decodersMap.begin();
			while (iter != decodersMap.end()) {
				Decoder decoder = iter->second;
				char ** write_data = decoder.decode();
				for (int i = 0; i < BLOCK_SIZE; i++)
				{
					int data_length;
					memcpy(&data_length, write_data[i], sizeof(int));
					//cout << "Data Length:" << data_length << endl;
					if (data_length <= 0)
						break;
					ret = fwrite(write_data[i] + 4, sizeof(char), data_length, fp);
				}
				iter++;
			}
			endTime = clock();
			//cout << counts << ": groupId:" << groupId << " BlockId" << blockId << endl;
			cout << "Transfer time: " << endTime - startTime << "ms" << endl;
			cout << "Receive " << file_name << " Successful!" << endl;
			//表明传输时间，对象
			SYSTEMTIME st;
			GetLocalTime(&st);
			cout << "Linking Time:" << st.wYear << "/" << st.wMonth << "/" << st.wDay << "  " << st.wHour << ":" << st.wMinute << endl;
			cout << "Client " << inet_ntoa(remoteAddr.sin_addr) << ":" << ntohs(remoteAddr.sin_port) << " Connection！" << endl;
			cout << "----------------------------------------------------------------------" << endl;
			cout << endl;
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ServerSocket, ClientSocket;

	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		cout << "Init Windows Socket Failed:" << GetLastError() << endl;
		closesocket(ClientSocket);
		return -1;
	}

	ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "Create Socket Failed:" << GetLastError() << endl;
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(ServerSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		cout << "bind error !" << endl;
		return 0;
	}
	else
	{
		cout << "Server is started!" << endl;
	}

	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);

	while (true)
	{
		char choose[100];
		memset(choose, 0, sizeof(choose));
		recvfrom(ServerSocket, choose, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen);//接收选择
		if (!strncmp(choose, "1#", 2))
		{
			cout << "Message" << endl;
			char recvData[BUF_MAXLEN];
			memset(recvData, 0x00, sizeof(recvData));
			int ret = recvfrom(ServerSocket, recvData, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen);//接收hello
			if (!ret)
			{
				cout << "Receive Failed:" << GetLastError() << endl;
			}
			cout << "Receive the Connectiong:" << inet_ntoa(remoteAddr.sin_addr) << ":" << ntohs(remoteAddr.sin_port) << endl;
			cout << "--------------------------------" << recvData << "--------------------------------" << endl;
			cout << "Client OK!" << endl;

			HANDLE hThread = CreateThread(NULL, 0, ClientThread, &remoteAddr, 0, NULL);
			CloseHandle(hThread);
		}
		else if (!strncmp(choose, "2#", 2))
		{
			cout << "File" << endl;
			char recvData[BUF_MAXLEN];
			memset(recvData, 0x00, sizeof(recvData));
			int ret = recvfrom(ServerSocket, recvData, BUF_MAXLEN, 0, (sockaddr *)&remoteAddr, &nAddrLen);//接收hello
			if (!ret)
			{
				cout << "Receive Failed:" << GetLastError() << endl;
			}
			cout << "Receive the Connectiong:" << inet_ntoa(remoteAddr.sin_addr) << ":" << ntohs(remoteAddr.sin_port) << endl;
			cout << "--------------------------------" << recvData << "--------------------------------" << endl;
			cout << "Client OK!" << endl;

			HANDLE hThread = CreateThread(NULL, 0, ClientThread, &remoteAddr, 0, NULL);
			CloseHandle(hThread);
		}
	}

	WSACleanup();
	return 0;
}
