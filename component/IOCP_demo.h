#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <process.h>

#define READ 1
#define WRITE 2


/** 说明：主要完成端口API上传入的我们自定义的结构体对象的地址，在工作线程上，又给我们返回我们的对象地址，这一点非常重要 */

typedef struct
{
	SOCKET hClientSock;
	SOCKADDR_IN clentAddr;
} SOCK_DATA, *LPSOCK_DATA;

typedef struct
{
	OVERLAPPED overlapped;
	WSABUF buff;
	int rwMode;
	char buffer[64];
	
} IO_DATA, *LPIO_DATA;

unsigned WINAPI threadFunc(void *args)
{
	HANDLE hComPort = (HANDLE)args;
	DWORD recvSize = 0;
	LPSOCK_DATA pClientData;
	LPIO_DATA pIOData;
	while (1)
	{
		/** 等待获取完成端口的信息,注意这里所有的处理完成端口的线程都阻塞在这里，当事件OK后，完成端口会分发一个线程来处理 */
		//这里的数据pClientData、pIOData都是传出来的，不需要申请内存
		GetQueuedCompletionStatus(hComPort, &recvSize, (LPDWORD)&pClientData, (LPOVERLAPPED*)&pIOData,
			INFINITE);
		SOCKET sock = pClientData->hClientSock;
		if (pIOData->rwMode == READ)
		{
			std::cout << "get data...\n";
			if (recvSize == 0)
			{
				std::cout << "client has close\n";
				closesocket(sock);
				continue;
			}
			else
			{
				//数据已经被接收了，我们只需要打印出来即可！
				std::cout << pIOData->buff.buf << std::endl;
			}
		}

	}
}


int main()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		std::cerr << "startup network lib error!\n";
		exit(1);
	}
	
	HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // 创建完成端口
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	
	/** 获取系统支持的最大线程数并创建出处理IO的线程 */
	for (int idx = 0; idx < sysInfo.dwNumberOfProcessors; ++idx)
	{
		_beginthreadex(NULL, 0, threadFunc, hComPort, NULL, NULL);
	}

	SOCKET hServerSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (hServerSock == INVALID_SOCKET)
	{
		std::cerr << "WSASocket error!\n";
		exit(1);
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(10000);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(hServerSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		std::cerr << "bind error!\n";
		exit(1);
	}

	listen(hServerSock, 10);

	SOCK_DATA clientData;
	memset(&clientData, 0, sizeof(clientData));
	IO_DATA ioData;
	memset(&ioData, 0, sizeof(ioData));
	DWORD flags = 0;
	while (1)
	{
		SOCKET hClientSock;
		SOCKADDR_IN clientAddr;
		int addr_len = sizeof(clientAddr);
		memset(&clientAddr, 0, addr_len);
		hClientSock = accept(hServerSock, (SOCKADDR*)&clientAddr, &addr_len);
		clientData.hClientSock = hClientSock;
		memcpy(&clientData.clentAddr, &clientAddr, addr_len);
	
		//关联之前创建好的完成端口与client套接字，注意第三参数传入的是指针值
		CreateIoCompletionPort((HANDLE)hClientSock, hComPort, (DWORD)&clientData, 0); 

		ioData.buff.buf = ioData.buffer;
		ioData.buff.len = 64;
		ioData.rwMode = READ;
		
		DWORD recvSize = 0;

		/** 接收数据 */
		WSARecv(clientData.hClientSock, &ioData.buff, 1, &recvSize, &flags, &ioData.overlapped, NULL);


	}




	system("pause");
	WSACleanup();
	return 0;
}