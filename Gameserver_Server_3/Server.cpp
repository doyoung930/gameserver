#include "Server.h"
#include"SESSION.h"
Server* Server::m_pInst = NULL;
Server::Server() :m_recv_flag(0)
{


}

Server::~Server()
{
	for (auto& a : m_clients)
	{

		delete a.second;
	}
	WSACleanup();
}

void Server::InitServerAddr()
{
	ZeroMemory(&m_server_addr, sizeof(m_server_addr));
	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(SERVER_PORT);
	m_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}




void Server::Move(int key)
{
	switch (static_cast<MOVETYPE>(m_clients[key]->m_player_move.move_type))
	{
	case MOVETYPE::LEFT:
		m_clients[key]->m_player_move.x -= 1;
		if (m_clients[key]->m_player_move.x < 0)
			m_clients[key]->m_player_move.x = 0;
		break;
	case MOVETYPE::RIGHT:
		m_clients[key]->m_player_move.x += 1;
		if (m_clients[key]->m_player_move.x > 7)
			m_clients[key]->m_player_move.x = 7;
		break;
	case MOVETYPE::UP:
		m_clients[key]->m_player_move.y -= 1;
		if (m_clients[key]->m_player_move.y < 0)
			m_clients[key]->m_player_move.y = 0;
		break;
	case MOVETYPE::DOWN:

		m_clients[key]->m_player_move.y += 1;
		if (m_clients[key]->m_player_move.y > 7)
			m_clients[key]->m_player_move.y = 7;
		break;
	}
}

bool Server::Init()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
		return false;
	m_s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	int ret;
	InitServerAddr();
	ret = bind(m_s_socket, reinterpret_cast<sockaddr*>(&m_server_addr), sizeof(m_server_addr));
	if (ret == SOCKET_ERROR)
	{
		err_display("bind()");
		return false;
	}
	ret = listen(m_s_socket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		err_display("listen()");
		return false;
	}

	m_addr_size = sizeof(m_server_addr);

	return true;
}

bool Server::Run()
{
	for (int i = 1; i <= 10; ++i)
	{
		SOCKET c_socket = WSAAccept(m_s_socket, reinterpret_cast<sockaddr*>(&m_server_addr), &m_addr_size, 0, 0);

		if (m_c_socket == INVALID_SOCKET) {
			err_display("accept()");
		}
		else
			cout << "연결성공" << endl;
		int tcp_option = 1;
		setsockopt(c_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));

		SESSION* p = new SESSION(i, c_socket);
		m_clients.emplace(i, p);
		m_clients[i]->do_send((char)i, m_clients[i]->m_player_start.packet_type,
			m_clients[i]->m_player_start.size, (char*)&m_clients[i]->m_player_start);
		m_clients[i]->do_recv();

	}
	while (m_shutdown == false)
		SleepEx(100, true);
	return false;
}


