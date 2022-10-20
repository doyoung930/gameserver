//#include "header.h"
//#include "Board.cpp"
//
//HINSTANCE g_hInst;
//LPCTSTR lpszClass = L"Window Class Name";
//LPCTSTR lpszWindowName = L"GameServer";
//LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX WndClass;
//	g_hInst = hInstance;
//	// 윈도우 클래스 구조체 값 설정
//	WndClass.cbSize = sizeof(WndClass);
//	WndClass.style = CS_HREDRAW | CS_VREDRAW;
//	WndClass.lpfnWndProc = (WNDPROC)WndProc;
//	WndClass.cbClsExtra = 0;
//	WndClass.cbWndExtra = 0;
//	WndClass.hInstance = hInstance;
//	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	WndClass.lpszMenuName = NULL;
//	WndClass.lpszClassName = lpszClass;
//	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//	// 윈도우 클래스 등록
//	RegisterClassEx(&WndClass);
//}