// https://www.geekpage.jp/programming/winsock/gethostbyname.php
// �ŕ׋����Ă�

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
//
#include "DxLib.h"


int initWsock() {
	WSADATA d;

	struct sockaddr_in srv;
	SOCKET s;
	char buf[32];
	char* deststr;

	/*
	if (argc != 2) {
		printf("Usage : %s dest\n", argv[0]);
		return 1;
	}*/

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

	memset(buf, 0, sizeof(buf));
	int n = recv(s, buf, sizeof(buf), 0);

	printfDx("%d, %s\n", n, buf);

	closesocket(s);

	WSACleanup();
}

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeFont("������");
	SetFontSize(20);

	int fullscrnFlg = IDNO;
	//int fullscrnFlg = IDYES;
	//fullscrnFlg = MessageBox(NULL, TEXT("�t���X�N���[�����[�h�ŋN�����܂����H"),
		//TEXT("sample"), MB_YESNO | MB_ICONQUESTION);
	if (fullscrnFlg == IDYES) {
		ChangeWindowMode(false);
	}
	else
	{
		ChangeWindowMode(true);
	}
	if (DxLib_Init() == -1) { return -1; }
	SetMouseDispFlag(false);


	int e = initWsock();
	DrawFormatString(10, 10, 0x33ffcc,"winsock initialized : %d", e);
	
	DrawPixel(320, 240, GetColor(255, 255, 255));	// �_��ł�

	WaitKey();				// �L�[���͑҂�

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}