#include <windows.h>
#include <tchar.h>
#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define boardSize 500
#define oneSize 50
#define obstacleNumber 5

using namespace std;

// 선언
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"GameServer";
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 함수 선언
void GameBoard(HDC hdc, int boardCount, int xS, int yS);
//void GamePlayer(HDC hdc);
void MovePlayer(HDC hdc);
// 플레이어
void GamePlayer(HDC hdc, int xStart, int yStart);

// 구조체 선언
typedef struct {
	int xPos;
	int yPos;
	COLORREF pColor;
}PLAYER;

PLAYER player;

// main
//---------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
//---------------------------------------------------------------------------------------
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	// 윈도우 클래스 구조체 값 설정
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	// 윈도우 클래스 등록
	RegisterClassEx(&WndClass);

	// 윈도우 생성
	hWnd = CreateWindow(
		lpszClass, 
		lpszWindowName, 
		WS_OVERLAPPEDWINDOW, 
		300, 0, 
		800, 800, 
		NULL, 
		(HMENU)NULL, 
		hInstance, 
		NULL
	);
	
	// 윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// 이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}


// CALLBACK
//-------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH hBrush, oldBrush;
	string playerPos;
	TCHAR lpOut[21]{};

	int xStart = 50, yStart = 50;
	int bSize = boardSize / oneSize;
	int x = 100, y = 100;
	// 메시지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		xStart = 50; yStart = 50;
		bSize = boardSize / oneSize;
		player.xPos = 0;
		player.yPos = 0;
		player.pColor = RGB(255, 255, 0);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			if (player.xPos > 0) {
				player.xPos--;
			}
			break;
		case VK_UP:
			if (player.yPos > 0) {
				player.yPos--;
			}
			break;
		case VK_RIGHT:
			if (9 > player.xPos ) {
				player.xPos++;
			}
			break;
		case VK_DOWN:
			if (9 > player.yPos ) {
				player.yPos++;
			}
			break;
		}
		InvalidateRect(hwnd, NULL, true);

		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GameBoard(hdc, bSize, xStart, yStart);
		
		// 플레이어
		GamePlayer(hdc,xStart, yStart);
		
		// 현재 플레이어의 위치를 나타내는 문자
		wsprintf(lpOut, TEXT(" x pos : %d , y pos : %d"), player.xPos, player.yPos);
		TextOut(hdc, 600, 600, lpOut, lstrlen(lpOut));
		
		EndPaint(hwnd, &ps);
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // 위의 세 메시지 외의 나머지 메시지는 OS로
}


// FUCNTION
// ----------------------------------------------------------------------------------------------
void GameBoard(HDC hdc, int boardCount, int xS, int yS)
{
	int i;
	for (i = 0; i <= boardCount; i++) {
		MoveToEx(hdc, i * oneSize + xS, 0 + yS, NULL);
		LineTo(hdc, i * oneSize + xS, boardCount * oneSize + yS);
		MoveToEx(hdc, 0 + xS, i * oneSize + yS, NULL);
		LineTo(hdc, boardCount * oneSize + xS, i * oneSize + yS);
	}

}

void GamePlayer(HDC hdc, int xStart, int yStart)
{
	HBRUSH hBrush, oldBrush;
	hBrush = CreateSolidBrush(player.pColor);
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Ellipse(hdc, player.xPos * oneSize + xStart, player.yPos * oneSize + yStart, (player.xPos + 1) * oneSize + xStart, (player.yPos + 1) * oneSize + yStart);
	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

}

