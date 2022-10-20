#include "SESSION.h"
#include"Server.h"

SESSION::SESSION(int id, SOCKET s) :_id(id), _socket(s)
{
	_recv_wsabuf.buf = _recv_buf;
	_recv_wsabuf.len = BUFSIZE;
	_send_wsabuf.buf = _send_buf;
	_send_wsabuf.len = 0;

	m_player_move.x = 0;
	m_player_move.y = 0;

	m_player_start.id = _id;


}

void SESSION::do_recv()
{
	DWORD recv_flag = 0;

	ZeroMemory(&_recv_over, sizeof(_recv_over));
	_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
	WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, SESSION::recv_callback);
}

void SESSION::do_send(char s_id, char packet_type, char size, char* addr)
{

	EXP_OVER* ex_over = new EXP_OVER(s_id, packet_type, size, addr);

	WSASend(_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, SESSION::send_callback);
}

void SESSION::send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	cout << "보내기 성공" << endl;
	EXP_OVER* exp_over = reinterpret_cast<EXP_OVER*>(send_over);
	delete exp_over;

}



void SESSION::recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	cout << num_byte << endl;
	int s_id = reinterpret_cast<int>(recv_over->hEvent);
	char* p = Server::GetInst()->m_clients[s_id]->_recv_buf;
	while (p < Server::GetInst()->m_clients[s_id]->_recv_buf + num_byte) {
		switch (static_cast<PACKET_TYPE>(*(p + 1)))
		{
		case PACKET_TYPE::START:
			Server::GetInst()->m_clients[s_id]->m_player_start.id = reinterpret_cast<STARTPACKET*>(p)->id;
			//전체 정보 넘겨주기
			//char s_id, char packet_type, char size, char* addr
			for (auto& cl : Server::GetInst()->m_clients)
			{
				cl.second->do_send(s_id, (char)PACKET_TYPE::START, Server::GetInst()->m_clients[s_id]->m_player_start.size,
					(char*)&Server::GetInst()->m_clients[s_id]->m_player_start);


			}
			for (auto& cl : Server::GetInst()->m_clients)
			{
				Server::GetInst()->m_clients[s_id]->do_send(cl.first, (char)PACKET_TYPE::MOVE, Server::GetInst()->m_clients[s_id]->m_player_move.size,
					(char*)&Server::GetInst()->m_clients[s_id]->m_player_move);
			}
			p = p + Server::GetInst()->m_clients[s_id]->m_player_start.size;
			cout << "start!" << endl;
			break;
		case PACKET_TYPE::MOVE:
			Server::GetInst()->m_clients[s_id]->m_player_move.id = reinterpret_cast<MOVEPACKET*>(p)->id;
			Server::GetInst()->m_clients[s_id]->m_player_move.x = reinterpret_cast<MOVEPACKET*>(p)->x;
			Server::GetInst()->m_clients[s_id]->m_player_move.y = reinterpret_cast<MOVEPACKET*>(p)->y;
			Server::GetInst()->m_clients[s_id]->m_player_move.move_type = reinterpret_cast<MOVEPACKET*>(p)->move_type;
			cout << "Player[" << s_id << "] 현재좌표" << " x : " << Server::GetInst()->m_clients[s_id]->m_player_move.x << " y : "
				<< Server::GetInst()->m_clients[s_id]->m_player_move.y << endl;

			Server::GetInst()->Move(s_id);

			for (auto& cl : Server::GetInst()->m_clients)
			{
				cl.second->do_send(s_id, (char)PACKET_TYPE::MOVE, Server::GetInst()->m_clients[s_id]->m_player_move.size,
					(char*)&Server::GetInst()->m_clients[s_id]->m_player_move);
			}
			p = p + Server::GetInst()->m_clients[s_id]->m_player_move.size;
			break;
		case PACKET_TYPE::END:
			Server::GetInst()->m_clients[s_id]->m_player_end.id = reinterpret_cast<ENDPACKET*>(p)->id;
			for (auto& cl : Server::GetInst()->m_clients)
			{

				cl.second->do_send(s_id, (char)PACKET_TYPE::END, Server::GetInst()->m_clients[s_id]->m_player_end.size,
					(char*)&Server::GetInst()->m_clients[s_id]->m_player_end);

			}
			p = p + Server::GetInst()->m_clients[s_id]->m_player_end.size;

			Server::GetInst()->m_clients[s_id]->DetroySession = true;
			cout << "player[" << s_id << "]로그아웃" << endl;
			break;
		}
	}
	if (Server::GetInst()->m_clients[s_id]->DetroySession)
	{
		delete Server::GetInst()->m_clients[s_id];
		Server::GetInst()->m_clients.erase(s_id);
	}
	if (Server::GetInst()->m_clients.find(s_id) != Server::GetInst()->m_clients.end())
		Server::GetInst()->m_clients[s_id]->do_recv();
}