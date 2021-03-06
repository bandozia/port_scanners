#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <stdio.h>
#include <vector>
#include <thread>
#include <algorithm>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

enum Mode
{
	SINGLE,
	MULTI
};

int Scan(PCSTR ip, PCSTR port)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "Deu pau na inicializacao" << endl;
		return -1;
	}
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(ip, port, &hints, &result);
	if (iResult != 0)
	{
		cout << "deu pau no getAddr...[" << iResult << "]" << endl;
		return -1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) 
		{
			cout << "deu pau na criação do socket [" << WSAGetLastError() << "]" << endl;
			WSACleanup();
			return -1;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << " --> porta fechada: [" << port << "]" << endl;
		WSACleanup();
		return -1;
	}
	else 
	{
		cout << " ==> PORTA ABERTA: [" << port << "]" << endl;
		return 1;
	}	
}


int main(int argc, char* argv[])
{
	
	/*thread t(Scan, "domi.ni.o", "80");	
	cout << "exec" << endl;
	
	t.join();*/
	
	if (argc < 5)
	{
		cout << "ta errado..." << endl;
		return -1;
	}

	PCSTR adr = NULL, port = NULL;
	Mode mode;
				
	for (int i = 0; i < argc; i++)
	{
		if (string(argv[i]) == "-a")
			adr = argv[i + 1];

		if (string(argv[i]) == "-p")
		{
			string argStr(argv[i + 1]);
									
			std::replace(argStr.begin(), argStr.end(), ',', ' ');

			stringstream iss;
			iss.str(argStr);					


			string portMin;
			iss >> portMin;
			string portMax;
			iss >> portMax;
			

			cout << portMin << " : " << portMax;

			return 0;
			port = argv[i + 1];
		}
			

		if (string(argv[i]) == "-m")
		{
			if (string(argv[i + 1]) == "single")
			{
				mode = Mode::SINGLE;
			}
			else if (string(argv[i + 1]) == "multi")
			{
				mode = Mode::MULTI;
			}
		}
	}
	
	if (adr == NULL || port == NULL)
	{
		cout << "ta errado..." << endl;
		return -1;
	}

	if (mode == Mode::SINGLE)
	{
		cout << "Escaneando: " << adr << endl;
		Scan(adr, port);
	}

	//TODO: escrever modo multithread

    return 0;
}

