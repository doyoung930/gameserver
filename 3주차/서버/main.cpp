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
    // Ŭ���̾��� ������ ������ ����

    SESSION() { cout << "Unexpected Constructor Call Error!\n"; exit(-1); } // ������ ���� ������ ����
    SESSION(int id, SOCKET socket) : m_id(id), c_socket(socket)     // ��¥ ������.
    {
        // recv ������ ���ۿ� ����
        c_wsabuf[0].buf = reinterpret_cast<char*>(&player_move);

    }
    ~SESSION() { closesocket(c_socket); }       // �Ҹ���

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

    // ������ ���� ���� �ƴ� �ü�� ���� �ޱ� ���� ����� ����.
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

    //&addr_size �ݵ�� �����͸� �Ѱ� �־����. ������ ������� ���� �������� ����� �����;��� ��.
    SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
    if (c_socket == INVALID_SOCKET) {
        err_display("accept()");
    }
    else
        cout << "���Ἲ��" << endl;

    for (;;) {

        WSABUF mybuf;
        mybuf.buf = reinterpret_cast<char*>(&player_move);
        mybuf.len = sizeof(player_move);
        DWORD recv_byte;
        DWORD recv_flag = 0;
        ret = WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
        if (ret == SOCKET_ERROR) err_display("Recv");

        cout << "Player ������ǥ [" << player_move.x << ", " << player_move.y << " ] " << endl;

        // ������ �����͸� �޾� �÷��̾� �̵�
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

        // ���� �˼�
        if (ret == SOCKET_ERROR)
        {
            err_display("send()");
        }
    }
    closesocket(c_socket);
    closesocket(s_socket);
    WSACleanup();
}
