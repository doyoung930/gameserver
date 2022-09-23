#include "header.h"


class SESSION;
unordered_map<long long, SESSION> clients;


void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);

class SESSION {
    SOCKET c_socket;
    WSAOVERLAPPED c_over;
    WSABUF c_wsabuf[1];
    long long m_id;
    MOVEPACKET player_move;
public:
    // 클라이언의 정보를 저장할 버퍼

    SESSION() { cout << "Unexpected Constructor Call Error!\n"; exit(-1); } // 생성자 적지 않으면 오류
    SESSION(int id, SOCKET socket) : m_id(id), c_socket(socket)     // 진짜 생성자.
    {
        // recv 정보를 버퍼에 저장
        c_wsabuf[0].buf = reinterpret_cast<char*>(&player_move);

    }
    ~SESSION() { closesocket(c_socket); }       // 소멸자

    // recv
    void do_recv()
    {
        c_wsabuf[0].len = sizeof(player_move);
        DWORD recv_flag = 0;
        memset(&c_socket, 0, sizeof(c_over));
        c_over.hEvent = reinterpret_cast<HANDLE>(m_id);
        int ret = WSARecv(c_socket, c_wsabuf, 1, 0, &recv_flag, &c_over, recv_callback);
        if (ret == SOCKET_ERROR) err_display("Recv");
    }

    // send
    void do_send( int num_bytes )
    {
        c_wsabuf[0].len = num_bytes;
        memset(&c_over, 0, sizeof(c_over));
        c_over.hEvent = reinterpret_cast<HANDLE>(m_id);
        int ret = WSASend(c_socket, c_wsabuf, 1, 0, 0, &c_over, send_callback);
        if (ret == SOCKET_ERROR) err_display("Send");
    }
};

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
    long long client_id = reinterpret_cast<long long> (over->hEvent);
    cout << "Client[" << client_id << "], Sent x :" << ;
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{

}

int main()
{
    wcout.imbue(std::locale("korean"));
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
        return false;

    int ret;

    // 접속을 위한 것이 아닌 운영체제 에서 받기 위해 만드는 것임.
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




    INT addr_size = sizeof(server_addr);

    //&addr_size 반드시 포인터를 넘겨 주어야함. 서버가 여기까지 썻다 서버에서 사용한 데이터양을 앎.
    SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
    if (c_socket == INVALID_SOCKET) {
        err_display("accept()");
    }
    else
        cout << "연결성공" << endl;

    for (;;) {

        WSABUF mybuf;
        mybuf.buf = reinterpret_cast<char*>(&player_move);
        mybuf.len = sizeof(player_move);
        DWORD recv_byte;
        DWORD recv_flag = 0;
        ret = WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
        if (ret == SOCKET_ERROR) err_display("Recv");

        cout << "Player 현재좌표 [" << player_move.x << ", " << player_move.y << " ] " << endl;

        // 소켓의 데이터를 받아 플레이어 이동
        if (player_move.type == static_cast<char>(MOVETYPE::LEFT))
        {
            if (0 < player_move.x) {
                player_move.x--;
            }
        }
        if (player_move.type == static_cast<char>(MOVETYPE::RIGHT))
        {
            if (7 > player_move.x) {
                player_move.x++;
            }
        }
        if (player_move.type == static_cast<char>(MOVETYPE::UP))
        {
            if (0 < player_move.y) {
                player_move.y--;
            }
        }
        if (player_move.type == static_cast<char>(MOVETYPE::DOWN))
        {
            if (7 > player_move.y) {
                player_move.y++;
            }
        }

        DWORD sent_byte;
        mybuf.buf = (char*)&player_move;
        mybuf.len = sizeof(player_move);
        int ret = WSASend(c_socket, &mybuf, 1, &sent_byte, 0, 0, 0);

        // 에러 검수
        if (ret == SOCKET_ERROR)
        {
            err_display("send()");
        }
    }
    closesocket(c_socket);
    closesocket(s_socket);
    WSACleanup();
}
