// https://www.geekpage.jp/programming/winsock/gethostbyname.php
// で勉強してる

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
//
#include "DxLib.h"
#include "Libs/utils.h"


int initWsock(SOCKET &rtnSock) {
	WSADATA d;
	SOCKET s;

	struct sockaddr_in srv;
	char* deststr;

	deststr = (char*)"127.0.0.1";

	WSAStartup(MAKEWORD(2, 0), &d);

	s = socket(AF_INET, SOCK_STREAM, 0);

	srv.sin_family = AF_INET;
	srv.sin_port = htons(80000);

	srv.sin_addr.S_un.S_addr = inet_addr(deststr);

	if (srv.sin_addr.S_un.S_addr == 0xffffffff) {
		struct hostent* host;

		host = gethostbyname(deststr);
		if (host == NULL) {
			return 1;
		}
		srv.sin_addr.S_un.S_addr =
			*(unsigned int*)host->h_addr_list[0];
	}

	connect(s, (struct sockaddr*)&srv, sizeof(srv));
	rtnSock = s;
	return 0;
}

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ==================== Dxlib window/fonts init ====================
	ChangeFont("游明朝");
	SetFontSize(20);
	int fullscrnFlg = IDNO;
	//int fullscrnFlg = IDYES;
	//fullscrnFlg = MessageBox(NULL, TEXT("フルスクリーンモードで起動しますか？"), TEXT("sample"), MB_YESNO | MB_ICONQUESTION);
	if (fullscrnFlg == IDYES) { ChangeWindowMode(false); }
	else { ChangeWindowMode(true); }
	if (DxLib_Init() == -1) { return -1; }
	SetMouseDispFlag(false);
	scnControl scn;

	char buf[32];
	memset(buf, 0, sizeof(buf));

	// ==================== key input ====================
	bool inputEn = true;
	int InputHandle = MakeKeyInput(50, FALSE, FALSE, FALSE);
	char String[256];
	memset(String, 0, sizeof(buf));
	SetActiveKeyInput(InputHandle);


	// ==================== socket init ====================
	SOCKET s;
	int e = initWsock(s);
	DrawFormatString(0, 0, 0x33ffcc, "winsock initialized : %d", e);

	while (ProcessMessage() == 0) {
		if (CheckKeyInput(InputHandle) != 0) { // enter が押されるまで該当ハンドルに入力受付
			GetKeyInputString(String, InputHandle);
			DeleteKeyInput(InputHandle);

			InputHandle = MakeKeyInput(50, FALSE, FALSE, FALSE);
			SetActiveKeyInput(InputHandle);


			if (strcmp(String, "exit") == 0) {
				break;
			}
			if (strcmp(String, "foo") == 0) {
				DrawString(0, 100, "foo was entered.", 0xffff);
			}
		}
		//DrawKeyInputModeString(0, 0);	// 入力モードを描画
		DrawKeyInputString(0, 25, InputHandle); 	// 入力途中の文字列を描画

		memset(buf, 0, sizeof(buf)); 
		int n = recv(s, buf, sizeof(buf), 0);

		if (n != -1) {
			printfDx("%d, %s\n", n, buf);
		}

		DrawFormatString(0, 50, 0x33ffcc, "%d bytesReceived, \n received str: %s\n", n, buf);
		scn.lend();
	}


	closesocket(s);

	WSACleanup();

	
	
	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}