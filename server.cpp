#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

#define MAX_SIZE 1024
#define MAX_CLIENT 6

typedef struct socket_INFO {
  SOCKET sck;
  std::string user;
  HANDLE ev;
}SOCKET_INFO;

std::vector<SOCKET_INFO> sck_list;
SOCKET_INFO server_sock;
int client_count = 0;

void server_init();
void add_client();
void send_msg(const char* msg);
void recv_msg(int idx);
void del_client(int idx);

int main() {
  WSADATA wsa;
  
  int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (!code) {
		server_init();

		char buf[MAX_SIZE] = { };
		std::string msg = "";

		std::thread th1(add_client);
		
		while (1) {
			std::cin >> buf;
			msg = server_sock.user + " : " + buf;
			send_msg(msg.c_str());
		}
		th1.join();
    
		closesocket(server_sock.sck);
	}

	WSACleanup();

	return 0;
}

void server_init() {
	WSADATA wsa;

	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (!code) {
		server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		SOCKADDR_IN server_addr = {};
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(7777);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));
		listen(server_sock.sck, SOMAXCONN);

		server_sock.user = "server";

		std::cout << "Server On" << std::endl;
	}
}

void add_client() {
	while (client_count < MAX_CLIENT) {
		SOCKADDR_IN addr = {};
		int addrsize = sizeof(addr);
		char buf[MAX_SIZE] = { };

		ZeroMemory(&addr, addrsize);
		SOCKET_INFO new_client = {};

		new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
		recv(new_client.sck, buf, MAX_SIZE, 0);
		new_client.user = std::string(buf);

		std::string msg = new_client.user + " 접속";
		std::cout << msg << std::endl;
		sck_list.push_back(new_client);

		std::thread th(recv_msg, client_count);
		client_count++;
		send_msg(msg.c_str());

		th.join();
	}
}

void send_msg(const char* msg) {
	for (int i = 0; i < client_count; i++) {
		send(sck_list[i].sck, msg, MAX_SIZE, 0);
	}
}

void recv_msg(int idx) {
	char buf[MAX_SIZE] = { };
	std::string msg = "";

	std::cout << sck_list[idx].user << std::endl;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
			msg = sck_list[idx].user + " : " + buf;
			std::cout << msg << std::endl;
			send_msg(msg.c_str());
		}
		else {
			msg = sck_list[idx].user + " disconnected";
			std::cout << msg << std::endl;
			send_msg(msg.c_str());
			del_client(idx);
			return;
		}
	}
}

void del_client(int idx) {
	closesocket(sck_list[idx].sck);
	sck_list.erase(sck_list.begin() + idx);
	client_count--;
}
