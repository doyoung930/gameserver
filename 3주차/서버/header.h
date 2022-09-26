#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <unordered_map>
#include "type.h"

#define BUFSIZE 256

using namespace std;
#pragma comment (lib, "WS2_32.LIB")

const short SERVER_PORT = 4000;
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