#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;
int main()
{
	WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult = NULL;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;

	const char* sendBuffer = "Hello from server!";

	char recvBuffer[512];

	int result;



	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		cout << "WSAStartup failed, result = " << result << endl;
		return 1;
	}


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // Сервер ждет соединение (если активная сторона (клиент) знает адрес сервера, то сервер не знает куда идти)
	
	result = getaddrinfo(NULL, "1945", &hints, &addrResult);


	if (result != 0)
	{
		cout << "getaddrinfo faild with error: " << result << endl;
		WSACleanup();
		return 1;
	}
	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		cout << "Binding socket failed :(" << endl;
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = listen(ListenSocket, SOMAXCONN);// SOMAXCONN (смотри количество в 16ти ричной, можешь задать 1 соединение если требуется)
	if (result == SOCKET_ERROR)
	{
		cout << "Listening socket failed" << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "Accepting socket failed!" << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);

	

	do
	{
		ZeroMemory(recvBuffer, 512);
		result = recv(ClientSocket, recvBuffer, 512, 0); // Отрицательное - ошибка, 0 - сокет закрыт с той стороны, положительное - ОК
		if (result > 0)
		{
			cout << "Received" << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;

			result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			if (result == SOCKET_ERROR)
			{
				cout << "Failed to send data back" << endl;
				closesocket(ClientSocket);
				freeaddrinfo(addrResult);
				WSACleanup();
				return 1;
			}
		}

		else if (result == 0)
			cout << "Connection closing..." << endl;
		else
		{
			cout << "recv failed with error" << endl;
		closesocket(ClientSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
		}
	} while (result > 0);


	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		cout << "Shutdown client socked failed" << endl;
		closesocket(ClientSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ClientSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;
}
