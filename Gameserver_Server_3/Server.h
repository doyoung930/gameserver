#pragma once
#include "header.h"
class SESSION;
class Server
{
private:
	static Server* m_pInst;
	static bool m_bLoop;
public:
	static Server* GetInst()
	{
		if (!m_pInst)
			m_pInst = new Server;
		return m_pInst;
	}

	static void DestroyInst()
	{
		if (m_pInst)
		{
			delete m_pInst;
			m_pInst = NULL;
		}
	}
public:
	Server();
	~Server();
private:
	WSADATA m_WsaData;
	SOCKET m_s_socket;
	SOCKADDR_IN m_server_addr;
	INT m_addr_size;

	SOCKET m_c_socket;
	WSABUF m_rWsaBuf;
	WSABUF m_sWsaBuf;

	WSAOVERLAPPED send_over;
	WSAOVERLAPPED recv_over;

	DWORD m_recv_byte;
	DWORD m_recv_flag;
	DWORD sent_byte;
	bool m_shutdown = false;

	MOVEPACKET player_move;
	STARTPACKET m_start_packet;
	ENDPACKET m_end_packet;
public:
	unordered_map<int, SESSION*>m_clients;
private:
	void InitServerAddr();
	void RecvMove();
	void SendMove();

public:
	bool Init();
	bool Run();
	void Move(int key);
	void err_display(const char* msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		printf("[%s] %s", msg, (char*)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}


};

