#include "header.h"

// 선언 
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"NAME";
LPCTSTR lpszWindowName = L"GameServer";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMSG, WPARAM wParam, LPARAM lParam);

// 함수 선언
void RenderBoard(HDC hdc);
void RenderPlayer(HDC hdc, RECT m_size);
void err_display(const char* msg);
void DeletePlayer(int key);
bool AllRecv();
bool AddPlayer(int);
bool Send_Start(int id);
bool Send_End();
// 보드
RECT m_Tile[8][8];

// 서버 변수 선언
WSADATA m_WsaData;
SOCKET m_s_socket;
SOCKADDR_IN m_server_addr;
WSABUF m_rWsaBuf;
WSABUF m_sWsaBuf;
DWORD m_recv_byte;
DWORD m_recv_flag;
DWORD m_sent_byte;
char SERVER_ADDR[256] = { 0, };


//플레이어 상태
MOVEPACKET player_move;
STARTPACKET m_start_packet;
ENDPACKET m_end_packet;

// 플레이어
RECT m_size;
// OVERLAPPED
WSAOVERLAPPED s_over;
WSAOVERLAPPED r_over;
// SOCKET
SOCKET s_socket;
// 버퍼
char m_temp_buf[BUFSIZE] = { 0, };
int option;
int _id;

//콜백 함수
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD falgs);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD falgs);
// do_send
void do_send();

// 서버 오류검사 리턴
int ret;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int       nCmdShow)
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
		SleepEx(100, true);
	}

	return Message.wParam;
}

// 플레이어 상태 클래스
class PlayerComponent
{
public:
	int x, y;
	int id;
	int myid;
	RECT m_size;
	TCHAR str[100];
public:
	PlayerComponent() { cout << "잘못된 객체가 들어갔습니다." << endl; };
	PlayerComponent(int id, int myid) :x(0), y(0), id(id), myid(myid)
	{
		m_size = { 100,100,150,150 };
		ZeroMemory(str, sizeof(str));
	}
	~PlayerComponent() {};

};
unordered_map<int, PlayerComponent*> m_players;


//bool send_start(int id);

// CALLBACK
//-------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	//m_size 초기화
	m_size = { 100,100,150,150 };
	// 메시지 처리하기
	switch (uMsg) {

	case WM_CREATE:
		//m_Tile 초기화
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				m_Tile[i][j] = { 100 + j * 50, 100 + i * 50, 150 + j * 50, 150 + i * 50 };
			}
		}

		//플레이어 상태 초기화
		player_move.size = sizeof(player_move);
		player_move.x = 0;
		player_move.y = 0;

		// 서버주소 받기
		wcout.imbue(locale("korean"));
		cout << "IP주소를 입력하세요:";
		cin >> SERVER_ADDR;

		// 소켓 선언
		if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0) return false;
		m_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
		// 초기화
		ZeroMemory(&m_server_addr, sizeof(m_server_addr));
		m_server_addr.sin_family = AF_INET;
		m_server_addr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, SERVER_ADDR, &m_server_addr.sin_addr);

		// Connect
		ret = WSAConnect(m_s_socket, reinterpret_cast<sockaddr*>(&m_server_addr), sizeof(m_server_addr),0,0,0,0);
		if (ret == SOCKET_ERROR)	err_display("connect()");
		else		cout << "연결성공" << endl;
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			// Send
			player_move.type = static_cast<char>(MOVETYPE::LEFT);
			player_move.id = _id;
			m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
			m_sWsaBuf.len = player_move.size;
			ZeroMemory(&s_over, sizeof(s_over));
			ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
			if (ret == SOCKET_ERROR) err_display("send()");

			break;
		case VK_UP:
			player_move.type = static_cast<char>(MOVETYPE::UP);
			player_move.id = _id;
			m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
			m_sWsaBuf.len = player_move.size;
			ZeroMemory(&s_over, sizeof(s_over));
			ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
			if (ret == SOCKET_ERROR) err_display("send()");

			break;
		case VK_RIGHT:
			player_move.type = static_cast<char>(MOVETYPE::RIGHT);
			player_move.id = _id;
			m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
			m_sWsaBuf.len = player_move.size;
			ZeroMemory(&s_over, sizeof(s_over));
			ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
			if (ret == SOCKET_ERROR) err_display("send()");

			break;
		case VK_DOWN:
			player_move.type = static_cast<char>(MOVETYPE::DOWN);
			player_move.id = _id;
			m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
			m_sWsaBuf.len = player_move.size;
			ZeroMemory(&s_over, sizeof(s_over));
			ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
			if (ret == SOCKET_ERROR) err_display("send()");

			break;
		case VK_ESCAPE:
			m_start_packet.id = _id;
			m_sWsaBuf.buf = reinterpret_cast<char*>(&m_end_packet);
			m_sWsaBuf.len = m_end_packet.size;
			//DWORD end_sent_byte;
			ZeroMemory(&s_over, sizeof(s_over));
			ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
			if (ret == SOCKET_ERROR) err_display("SendEnd()");

			break;

			//InvalidateRect(hwnd, NULL, true);
		}
		std::cout << "x " << player_move.x << " y " << player_move.y << endl;
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
	

	return DefWindowProc(hwnd, uMsg, wParam, lParam); // 위의 세 메시지 외의 나머지 메시지는 OS로
}

// 함수
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	cout << "recv_callback" << endl;
	char* p = m_temp_buf;
	while (p < m_temp_buf + num_bytes) {
		switch (static_cast<PACKET_TYPE>(*(p + 1)))
		{
		case PACKET_TYPE::START:
		{
			m_start_packet.id = reinterpret_cast<STARTPACKET*>(p)->id;
			cout << "ID : " << (int)m_start_packet.id << endl;
			// 시작 플레이어 정보
			if (AddPlayer(m_start_packet.id))
			{
				Send_Start(m_start_packet.id);
				_id = reinterpret_cast<STARTPACKET*>(p)->id;
			}
			p = p + m_start_packet.size;
			cout << "player [ " << (int)m_start_packet.id << " ] 로그인" << endl;
			break;
		}	
		case PACKET_TYPE::MOVE:
		{	
			int move_id = reinterpret_cast<MOVEPACKET*>(p)->id;
			AddPlayer(move_id);

			m_players[move_id]->x = reinterpret_cast<MOVEPACKET*>(p)->x;
			m_players[move_id]->y = reinterpret_cast<MOVEPACKET*>(p)->y;
			cout << "현재 좌표 x ," << m_players[move_id]->x << " y " << m_players[move_id]->y << endl;
			m_players[move_id]->m_size = m_Tile[reinterpret_cast<MOVEPACKET*>(p)->x][reinterpret_cast<MOVEPACKET*>(p)->y];
			if (move_id == player_move.id)
			{
				player_move.x = reinterpret_cast<MOVEPACKET*>(p)->x;
				player_move.y = reinterpret_cast<MOVEPACKET*>(p)->y;
			}
			p = p + player_move.size;
			cout << " MOVE" << (int*) p << endl;
			
			break;
		}
		case PACKET_TYPE::END:
		{
			m_end_packet.id = reinterpret_cast<ENDPACKET*>(p)->id;

			if (m_end_packet.id == _id)
			{
				cout << "END" << endl;
				closesocket(m_s_socket);
				WSACleanup();
				PostQuitMessage(0);
			}
			else
			{
				DeletePlayer(m_end_packet.id);
				cout << "Player [ " << m_end_packet.id << " ] end" << endl;
			}
			p = p + m_end_packet.size;
			break;
		}
		}
	}

	cout << "recv_callback end" << endl;

	AllRecv();
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	cout << " [ " << num_bytes << " ] 바이트 send" << endl;
}

void do_send()
{

}

bool Send_Start(int id)
{
	m_start_packet.id = id;
	m_sWsaBuf.buf = reinterpret_cast<char*>(&m_start_packet);
	m_sWsaBuf.len = m_start_packet.size;
	//DWORD start_sent_byte;
	ZeroMemory(&s_over, sizeof(s_over));
	cout << "로그인" << endl;
	ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
	if (ret == SOCKET_ERROR)
	{
		err_display("SendStart()");
		return false;
	}
	return true;
}

bool Send_End()
{
	m_start_packet.id = _id;
	m_sWsaBuf.buf = reinterpret_cast<char*>(&m_end_packet);
	m_sWsaBuf.len = m_end_packet.size;
	//DWORD end_sent_byte;
	ZeroMemory(&s_over, sizeof(s_over));
	ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &s_over, send_callback);
	if (ret == SOCKET_ERROR)
	{
		err_display("SendEnd()");
		return false;
	}
	return true;
}

// 플레이어 렌더링
void RenderPlayer(HDC hdc, RECT m_size)
{
	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(100, 100, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
	Ellipse(hdc, m_size.left + (player_move.x * 50), m_size.top + (player_move.y * 50), m_size.right + (player_move.x * 50), m_size.bottom + (player_move.y * 50));
	SelectObject(hdc, oldBrush);
	DeleteObject(myBrush);

}

// 보드 랜더링
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

// 플레이어 추가 함수
bool AddPlayer(int id)
{
	if (m_players.find(id) == m_players.end()) 
	{
		if (m_players.empty())
		{
			cout << "ID 추가 " << endl;
			PlayerComponent* p = new PlayerComponent(id, id);
			m_players.emplace(id, p);
			
			return true;
		}
		else
		{
			PlayerComponent* p = new PlayerComponent(id, 0);
			m_players.emplace(id, p);

			return false;
		}
	}
	return false;
}

// 플레이어 삭제
void DeletePlayer(int key)
{
	if (m_players.find(key) != m_players.end())
		delete m_players[key];
	m_players.erase(key);
}

// Recv
bool AllRecv()
{
	m_rWsaBuf.buf = m_temp_buf;
	m_rWsaBuf.len = BUFSIZE;
	ZeroMemory(&r_over, sizeof(r_over));
	ret = WSARecv(m_s_socket, &m_rWsaBuf, 1, 0, &m_recv_flag, &r_over, recv_callback);
	if (ret == SOCKET_ERROR)
	{
		int err_num = WSAGetLastError();
		if (WSA_IO_PENDING != err_num) {
			err_display("Recv");
			return false;
		}
	}
	return false;
}