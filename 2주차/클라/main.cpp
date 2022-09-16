#include "header.h"

// ���� 
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"NAME";
LPCTSTR lpszWindowName = L"GameServer";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMSG, WPARAM wParam, LPARAM lParam);

// �Լ� ����
void RenderBoard(HDC hdc);
void RenderPlayer(HDC hdc, RECT m_size);
void err_display(const char* msg);


// ����
RECT m_Tile[8][8];

// ���� ���� ����
WSADATA m_WsaData;
SOCKET m_s_socket;
SOCKADDR_IN m_server_addr;
WSABUF m_rWsaBuf;
WSABUF m_sWsaBuf;
DWORD m_recv_byte;
DWORD m_recv_flag;
DWORD sent_byte;
char SERVER_ADDR[256] = { 0, };

//�÷��̾� ����
MOVEPACKET player_move;

// �÷��̾�
RECT m_size;



// ���� �����˻� ����
int ret;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	// ������ Ŭ���� ����ü �� ����
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
	// ������ Ŭ���� ���
	RegisterClassEx(&WndClass);

	// ������ ����
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



	// ������ ���
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);



	// �̺�Ʈ ���� ó��
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


	//m_size �ʱ�ȭ
	m_size = { 100,100,150,150 };
	


	// �޽��� ó���ϱ�
	switch (uMsg) {

	case WM_CREATE:
		//m_Tile �ʱ�ȭ
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				m_Tile[i][j] = { 100 + j * 50, 100 + i * 50, 150 + j * 50, 150 + i * 50 };
			}
		}

		//�÷��̾� ���� �ʱ�ȭ
		player_move.size = sizeof(player_move);
		player_move.x = 0;
		player_move.y = 0;

		// �����ּ� �ޱ�
		wcout.imbue(locale("korean"));
		cout << "IP�ּҸ� �Է��ϼ���:";
		cin >> SERVER_ADDR;

		// ���� ����
		if (WSAStartup(MAKEWORD(2, 0), &m_WsaData) != 0) return false;
		m_s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
		// �ʱ�ȭ
		ZeroMemory(&m_server_addr, sizeof(m_server_addr));
		m_server_addr.sin_family = AF_INET;
		m_server_addr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, SERVER_ADDR, &m_server_addr.sin_addr);

		// Connect
		ret = connect(m_s_socket, reinterpret_cast<sockaddr*>(&m_server_addr), sizeof(m_server_addr));
		if (ret == SOCKET_ERROR)
			err_display("connect()");
		else
			cout << "���Ἲ��" << endl;

		break;
	case WM_KEYDOWN:
		switch (wParam) {
			case VK_LEFT:
				// Send
				
				player_move.type = static_cast<char>(MOVETYPE::LEFT);
				m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
				m_sWsaBuf.len = player_move.size;
				ret = WSASend(m_s_socket, &m_sWsaBuf, 1, &sent_byte, 0, 0, 0);
				if (ret == SOCKET_ERROR) err_display("send()");
				
				m_rWsaBuf.buf = (char*)&player_move;
				m_rWsaBuf.len = sizeof(player_move);
				ret = WSARecv(m_s_socket, &m_rWsaBuf, 1, &m_recv_byte, &m_recv_flag, 0, 0);
				if (ret == SOCKET_ERROR) err_display("Recv");

				break;
			case VK_UP:
				player_move.type = static_cast<char>(MOVETYPE::UP);
				m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
				m_sWsaBuf.len = player_move.size;
				ret = WSASend(m_s_socket, &m_sWsaBuf, 1, &sent_byte, 0, 0, 0);
				if (ret == SOCKET_ERROR) err_display("send()");

				m_rWsaBuf.buf = (char*)&player_move;
				m_rWsaBuf.len = sizeof(player_move);
				ret = WSARecv(m_s_socket, &m_rWsaBuf, 1, &m_recv_byte, &m_recv_flag, 0, 0);
				if (ret == SOCKET_ERROR) err_display("Recv");

				break;
			case VK_RIGHT:
				player_move.type = static_cast<char>(MOVETYPE::RIGHT);
				m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
				m_sWsaBuf.len = player_move.size;
				ret = WSASend(m_s_socket, &m_sWsaBuf, 1, &sent_byte, 0, 0, 0);
				if (ret == SOCKET_ERROR) err_display("send()");

				m_rWsaBuf.buf = (char*)&player_move;
				m_rWsaBuf.len = sizeof(player_move);
				ret = WSARecv(m_s_socket, &m_rWsaBuf, 1, &m_recv_byte, &m_recv_flag, 0, 0);
				if (ret == SOCKET_ERROR) err_display("Recv");

				break;
			case VK_DOWN:
				player_move.type = static_cast<char>(MOVETYPE::DOWN);
				m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
				m_sWsaBuf.len = player_move.size;
				ret = WSASend(m_s_socket, &m_sWsaBuf, 1, &sent_byte, 0, 0, 0);
				if (ret == SOCKET_ERROR) err_display("send()");

				m_rWsaBuf.buf = (char*)&player_move;
				m_rWsaBuf.len = sizeof(player_move);
				ret = WSARecv(m_s_socket, &m_rWsaBuf, 1, &m_recv_byte, &m_recv_flag, 0, 0);
				if (ret == SOCKET_ERROR) err_display("Recv");

				break;
			std::cout << "x " << player_move.x << " y " << player_move.y << endl;
			InvalidateRect(hwnd, NULL, true);
		}
		InvalidateRect(hwnd, NULL, true);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		RenderBoard(hdc);
		RenderPlayer(hdc, m_size);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		WSACleanup();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // ���� �� �޽��� ���� ������ �޽����� OS��
}

// �Լ�

// �÷��̾� ������
void RenderPlayer(HDC hdc, RECT m_size)
{
	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 100, 100));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
	Ellipse(hdc, m_size.left+(player_move.x*50), m_size.top+(player_move.y * 50), m_size.right+(player_move.x * 50), m_size.bottom+ (player_move.y * 50));
	SelectObject(hdc, oldBrush);
	DeleteObject(myBrush);

}

// ���� ������
void RenderBoard(HDC hdc)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (i % 2 == 0)
			{
				if (j % 2 == 0)
				{
					HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));

					HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
					Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
					SelectObject(hdc, oldBrush);
					DeleteObject(myBrush);
				}
				else
					Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
			}
			else
			{
				if (j % 2 == 1)
				{
					HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));

					HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
					Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
					SelectObject(hdc, oldBrush);
					DeleteObject(myBrush);
				}
				else
					Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
			}
		}
	}
}

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

