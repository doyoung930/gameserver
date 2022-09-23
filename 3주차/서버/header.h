#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <unordered_map>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")

const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

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


class SESSION {
    SOCKET c_socket;
    WSAOVERLAPPED c_over;
    WSABUF c_wsabuf[1];
    long long m_id;
public:
    // Ŭ���̾��� ������ ������ ����

    SESSION() { cout << "Unexpected Constructor Call Error!\n"; exit(-1); } // ������ ���� ������ ����
    SESSION(int id, SOCKET socket) : m_id(id), c_socket(socket)     // ��¥ ������.
    {
        // recv ������ ���ۿ� ����
    }
    ~SESSION() { closesocket(c_socket); }       // �Ҹ���

    // recv
    void do_recv()
    {

    }

    // send
    void do_send( /*������ ������*/)
    {

    }
};

// �����˻�
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] ", msg);
    std::wcout << L"���� " << (LPTSTR)lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
}