#include "client.h"
#pragma warning(disable: 4996)

using namespace ObjectModel;

namespace Net
{
	Client::Client(int port, std::string ipaddress)
		:
		wsa{ 0 },
		port(port),
		ipaddress(ipaddress),
		clientsocket(INVALID_SOCKET),
		info{ 0 },
		infolength(sizeof(info)) {}

	void Client::init()
	{
		info.sin_family = AF_INET; //ip4
		info.sin_port = htons(port); //host to network (short)
		info.sin_addr.s_addr = inet_addr(ipaddress.c_str()); //ip adress

		printf("WSA init\n");
		assert(!(WSAStartup(514, &wsa)) && "Couldn`t init wsa"); //WSAStratrup return value 0
		printf("WSA success\n");

		printf("Creating socket\n");
		assert(!((clientsocket = socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR) && "Couldn`t create socket");
		printf("Success!\n");
	}

	void Client::connect()
	{
		init();

		for (;;)
		{
			send();
			receive();
			proccess();
		}
	}

	void Client::send()
	{
		printf("Enter a message:");
		std::getline(std::cin, message);

		if (message == "prim")
		{
			int32_t foo = 53;
			int16_t it = 0;
			std::unique_ptr<Primitive> p = Primitive::create("int32", Type::I32, foo);
			std::vector<uint8_t> result(p->getSize());
			p->pack(result, it);
			std::copy(result.begin(), result.end(), buffer);

			if ((sendto(clientsocket, buffer, p->getSize(), 0, (struct sockaddr*)&info, infolength)) == SOCKET_ERROR)
			{
				printf("send() failed...%d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if ((sendto(clientsocket, message.c_str(), message.size(), 0, (struct sockaddr*)&info, infolength)) == SOCKET_ERROR)
			{
				printf("recv() failed...%d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}
	}

	void Client::receive()
	{
		if ((recvlength = recvfrom(clientsocket, buffer, SIZE, 0, (struct sockaddr*)&info, &infolength)) == SOCKET_ERROR)
		{
			printf("recv() failed...%d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	void Client::proccess()
	{
		printf("packet from:%s:%d\n", inet_ntoa(info.sin_addr), ntohs(info.sin_port));
		if (buffer[0] == 0x1)
		{
			std::vector<uint8_t> result;
			for (unsigned i = 0; i < recvlength; i++)
			{
				result.push_back(buffer[i]);
			}

			int16_t it = 0;
			Primitive p = Primitive::unpack(result, it);

			printf("Primitive:\n");
			printf("\t |Name:%s\n", p.getName().c_str());
			printf("\t |Size:%d\n", p.getSize());
			printf("\t |Data:");

			for (auto i : p.getData())
			{
				printf("[%d]", i);
			}

			printf("\n");
		}
		else
		{
			printf("data:");
			for (unsigned i = 0; i < recvlength; i++)
			{
				printf("%c", buffer[i]);
			}
			printf("\n");
		}
	}

	Client::~Client()
	{
		WSACleanup();
		closesocket(clientsocket);
	}
}