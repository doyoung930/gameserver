#include "header.h"

class EXP_OVER {
public:
    WSAOVERLAPPED wsa_over;
    int s_id;
    WSABUF wsa_buf;
    char send_msg[BUFSIZE];

public:
    EXP_OVER(char id, char packet_type, char size, char* addr) : s_id(id)
    {
        ZeroMemory(&wsa_over, sizeof(wsa_over));
        addr[0] = size;
        addr[1] = packet_type;
        addr[2] = id;
        wsa_buf.buf = addr;
        wsa_buf.len = size;

        cout << "EXP_OVER " << "size : " << (int)addr[0] << "type : " << (int)addr[1] << "id : " << (int)addr[2] << endl;
    }

    ~EXP_OVER() {}
};

// SESSION
class SESSION;
unordered_map <long long, SESSION> clients;
class SESSION {
private:
    int _id;
    WSAOVERLAPPED _recv_over;
    SOCKET _socket;
    WSABUF _recv_wsabuf;
    WSABUF _send_wsabuf;
public:
    char _recv_buf[BUFSIZE];
    char _send_buf[BUFSIZE];
    bool DetroySession = false;
    MOVEPACKET m_player_move;
    STARTPACKET m_player_start;
    ENDPACKET m_player_end;

public:
    SESSION();
    SESSION(int id, SOCKET s);
    ~SESSION();

    void do_recv();
    void do_send(char s_id, char packet_type, char size, char* addr);

    static void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD recv_flag);
    static void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD recv_flag);

};

SESSION::SESSION() { cout << "Unexpected Constructor Call Error." << endl; exit(-1); }

SESSION::SESSION(int id, SOCKET socket) : _id(id), _socket(socket)
{
    _recv_wsabuf.buf = _recv_buf;
    _recv_wsabuf.len = BUFSIZE;
    _send_wsabuf.buf = _send_buf;
    _send_wsabuf.len = 0;

    m_player_move.x = 0;
    m_player_move.y = 0;

    m_player_start.id = _id;
}

SESSION::~SESSION() { if (_socket) closesocket(_socket); }

void SESSION::do_recv()
{
    DWORD recv_flag = 0;
    memset(&_recv_over, 0, sizeof(_recv_over));
    _recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
    WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, recv_callback);
}

void SESSION::do_send(char s_id, char packet_type, char size, char* addr)
{
    EXP_OVER* ex_over = new EXP_OVER(s_id, packet_type, size, addr);
    WSASend(_socket, &ex_over->wsa_buf, 1, 0, 0, &ex_over->wsa_over, SESSION::send_callback);
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
    char* p = clients[s_id]._recv_buf;

    while (p < clients[s_id]._recv_buf + num_byte) {
        switch (static_cast<PACKET_TYPE>(*(p + 1)))
        {
        case PACKET_TYPE::START:
            clients[s_id].m_player_start.id = reinterpret_cast<STARTPACKET*>(p)->id;

            // 정보 주기
            for (auto& c : clients) {
                c.second.do_send(s_id, (char)PACKET_TYPE::START, clients[s_id].m_player_start.size,
                    (char*)&clients[s_id].m_player_start);
            }
            for (auto& c : clients) {
                clients[s_id].do_send(c.first, (char)PACKET_TYPE::MOVE, clients[s_id].m_player_move.size,
                    (char*)&clients[s_id].m_player_move);
            }
            p = p + clients[s_id].m_player_start.size;

            cout << "시작" << endl;
            break;
        case PACKET_TYPE::MOVE:
            // map에 플레이어 움직임 저장
            clients[s_id].m_player_move.id = reinterpret_cast<MOVEPACKET*>(p)->id;
            clients[s_id].m_player_move.x = reinterpret_cast<MOVEPACKET*>(p)->x;
            clients[s_id].m_player_move.y = reinterpret_cast<MOVEPACKET*>(p)->y;
            clients[s_id].m_player_move.type = reinterpret_cast<MOVEPACKET*>(p)->type;

            // 플레이어 이동
            if (clients[s_id].m_player_move.type == static_cast<char>(MOVETYPE::LEFT))
            {
                if (0 < clients[s_id].m_player_move.x) {
                    clients[s_id].m_player_move.x--;
                }
            }
            if (clients[s_id].m_player_move.type == static_cast<char>(MOVETYPE::RIGHT))
            {
                if (7 > clients[s_id].m_player_move.x) {
                    clients[s_id].m_player_move.x++;
                }
            }
            if (clients[s_id].m_player_move.type == static_cast<char>(MOVETYPE::UP))
            {
                if (0 < clients[s_id].m_player_move.y) {
                    clients[s_id].m_player_move.y--;
                }
            }
            if (clients[s_id].m_player_move.type == static_cast<char>(MOVETYPE::DOWN))
            {
                if (7 > clients[s_id].m_player_move.y) {
                    clients[s_id].m_player_move.y++;
                }
            }

            cout << "Player ID [" << s_id << "] 좌표 x :" << clients[s_id].m_player_move.x << " y : " << clients[s_id].m_player_move.y << endl;

            // 바뀐 정보 send
            for (auto& c : clients)
            {
                c.second.do_send(s_id, (char)PACKET_TYPE::MOVE, clients[s_id].m_player_move.size, (char*)clients[s_id].m_player_move.size);
            }
            p = p + clients[s_id].m_player_move.size;

            break;
        case PACKET_TYPE::END:
            clients[s_id].m_player_end.id = reinterpret_cast<ENDPACKET*>(p)->id;

            for (auto& c : clients)
            {
                c.second.do_send(s_id, (char)PACKET_TYPE::END, clients[s_id].m_player_end.size, (char*)&clients[s_id].m_player_end);
            }
            clients[s_id].DetroySession = true;
            cout << "Player [ " << s_id << " ] 로그아웃" << endl;
            break;
        }
    }
    if (clients[s_id].DetroySession)
    {
        delete& clients[s_id];
        clients.erase(s_id);
    }
    if (clients.find(s_id) != clients.end())
        clients[s_id].do_recv();
}

//**********
int main()
//**********
{
    wcout.imbue(std::locale("korean"));
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) return false;
    int ret;
    SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
    SOCKADDR_IN server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    if (ret == SOCKET_ERROR)
    {
        err_display("bind()");
        return false;
    }
    ret = listen(s_socket, SOMAXCONN);
    if (ret == SOCKET_ERROR)
    {
        err_display("listen()");
        return false;
    }
    SOCKADDR_IN cl_addr;
    int addr_size = sizeof(cl_addr);
    for (int i = 1; ; ++i) {
        //&addr_size 반드시 포인터를 넘겨 주어야함. 서버가 여기까지 썻다 서버에서 사용한 데이터양을 앎.
        SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&cl_addr), &addr_size, NULL, NULL);
        if (c_socket == INVALID_SOCKET) err_display("accept()");
        else cout << "연결성공" << endl;
        clients.try_emplace(i, i, clients);
        clients[i].SESSION::do_recv();
    }


    closesocket(s_socket);
    WSACleanup();
}


