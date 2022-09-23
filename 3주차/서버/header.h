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
    // 클라이언의 정보를 저장할 버퍼

    SESSION() { cout << "Unexpected Constructor Call Error!\n"; exit(-1); } // 생성자 적지 않으면 오류
    SESSION(int id, SOCKET socket) : m_id(id), c_socket(socket)     // 진짜 생성자.
    {
        // recv 정보를 버퍼에 저장
    }
    ~SESSION() { closesocket(c_socket); }       // 소멸자

    // recv
    void do_recv()
    {

    }

    // send
    void do_send( /*버퍼의 사이즈*/)
    {

    }
};

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