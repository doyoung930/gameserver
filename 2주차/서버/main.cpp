#include "header.h"
typedef struct move_pakcet {
    short size;
    char type;
    int x;
    int y;
}MOVEPACKET;

enum class MOVETYPE
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

MOVEPACKET player_move;

// 에러검사
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] ", msg);
    std::wcout << L"에러 " << (LPTSTR)lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
}

int main()
{
    wcout.imbue(std::locale("korean"));
    WSADATA WSAData;
    if(WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
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
        char recv_buf[BUFSIZE];

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
