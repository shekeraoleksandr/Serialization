#include "server.h"
#pragma warning(disable: 4996)


namespace Net
{
	Server::Server(int port, std::string ipaddress)
		:
		wsa{ 0 },
		port(port),
		ipaddress(ipaddress),
		serversocket(INVALID_SOCKET),
		info{ 0 },
		infolength(sizeof(info)) {}

	void Server::init()
	{
		info.sin_family = AF_INET; //ip4
		info.sin_port = htons(port); //host to network (short)
		info.sin_addr.s_addr = inet_addr(ipaddress.c_str()); //ip adress

		printf("WSA init\n");
		assert(!(WSAStartup(514, &wsa)) && "Couldn`t init wsa"); //WSAStratrup return value 0
		printf("WSA success\n");

		printf("Creating socket\n");
		assert(!((serversocket = socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR) && "Couldn`t create socket");
		printf("Success!\n");

		printf("bind socket\n");
		assert(!(bind(serversocket, (struct sockaddr*)&info, infolength)) && "Couldn`t bind socket");
		printf("socket binded\n");

		printf("Server started at:%s:%d\n", inet_ntoa(info.sin_addr), ntohs(info.sin_port));
	}

	void Server::start()
	{
		init();

		for (;;)
		{
			receive();
			proccess();
			send();
		}
	}

	void Server::receive()
	{
		if ((recvlength = recvfrom(serversocket, buffer, SIZE, 0, (struct sockaddr*)&info, &infolength)) == SOCKET_ERROR)
		{
			printf("recv() failed...%d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	void Server::proccess()
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
			primitives.insert(std::make_pair(p.getName(), p));
			current = p.getName();


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

	void Server::send()
	{
		if (primitives.empty())
		{
			if ((sendto(serversocket, buffer, recvlength, 0, (struct sockaddr*)&info, infolength)) == SOCKET_ERROR)
			{
				printf("send() failed...%d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			int16_t it = 0;
			std::unique_ptr<Primitive> p = modify(current);
			std::vector<uint8_t> result(p->getSize());
			p->pack(result, it);
			std::copy(result.begin(), result.end(), buffer);

			if ((sendto(serversocket, buffer, p->getSize(), 0, (struct sockaddr*)&info, infolength)) == SOCKET_ERROR)
			{
				printf("send() failed...%d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			primitives.erase(current);
		}
	}

	std::unique_ptr<Primitive> Server::modify(std::string key)
	{
		primitives.erase(key);

		int16_t bar = 75;
		std::unique_ptr<Primitive> p(Primitive::create("int16", Type::I16, bar));
		primitives.insert(std::make_pair(p->getName(), *p));

		current = p->getName();

		return std::move(p);
	}

	Server::~Server()
	{
		WSACleanup();
		closesocket(serversocket);
	}
}