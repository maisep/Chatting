#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <thread>

#define MAX_SIZE 1024

SOCKET client_sock;

void chat_recv() {
	char buf[MAX_SIZE] = { };
	std::string msg;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			msg = buf;
			std::cout << buf << std::endl;
		}
		else {
			std::cout << "Server Off" << std::endl;
			return;
		}
	}
}

int main() {
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	std::string nick = "";

	if (!code) {
		std::cout << "사용할 닉네임 입력 >> ";
		std::cin >> nick;

		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(7777);
		InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

		while (1) {
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
				std::cout << "Server Connect" << std::endl;
				send(client_sock, nick.c_str(), nick.length(), 0);
				break;
			}
			std::cout << "Connecting..." << std::endl;
		}

		std::thread th2(chat_recv);

		char buffer[MAX_SIZE] = { };
		while (1) {
			std::cin >> buffer;
			send(client_sock, buffer, strlen(buffer), 0);
		}
		th2.join();
		closesocket(client_sock);
	}

	WSACleanup();
	return 0;
}
