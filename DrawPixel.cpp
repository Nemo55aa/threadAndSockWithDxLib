// https://www.geekpage.jp/programming/winsock/gethostbyname.php
// https://www.oji-koji.com/?p=1092
// �ŕ׋����Ă�

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <process.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
//
#include "DxLib.h"
#include "Libs/utils.h"

typedef struct
{
	char    cData;
	DWORD   dwTimer;
}MyDataRec;


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

volatile BOOL gfStopFlag;                    // �C����ʒm���邽�߂̃t���b�O
unsigned int __stdcall Func(void* pvoid) {
	MyDataRec* pData = (MyDataRec*)pvoid;
	unsigned int dwRet = 0;

	while (!gfStopFlag)
	{
		printfDx(" % c\n", pData->cData);
		Sleep(pData->dwTimer);
	}
	dwRet = 1200;
	_endthreadex(dwRet);        // �Ԃ��l�͂��̊֐��ɓn��
	return(0);                  // _endthreadex�������Ȃ炱���ւ͗��Ȃ�

}


volatile BOOL rcvStopFlag;
typedef struct
{
	SOCKET   sock;
	char*    rBuf;
	unsigned int len;
}rcvDataStr;
// ��M�p�X���b�h
unsigned int __stdcall rcvFunc(void* pvoid) {
	rcvDataStr* pData = (rcvDataStr*)pvoid;
	char funcbuf[32];
	int n;
	while (!rcvStopFlag) {
		memset(funcbuf, 0, sizeof(funcbuf));
		n = recv(pData->sock, funcbuf, sizeof(funcbuf), 0);
		if (n > 0) {
			memset(pData->rBuf, 0, sizeof(pData->rBuf));
			memcpy(pData->rBuf, funcbuf, n);
		}
		 printfDx(pData->rBuf);
		 Sleep(500);

	}
	_endthreadex(n);        // �Ԃ��l�͂��̊֐��ɓn��
	return(0);                  // _endthreadex�������Ȃ炱���ւ͗��Ȃ�

}
// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ==================== Dxlib window/fonts init ====================
	ChangeFont("������");
	SetFontSize(20);
	int fullscrnFlg = IDNO;
	//int fullscrnFlg = IDYES;
	//fullscrnFlg = MessageBox(NULL, TEXT("�t���X�N���[�����[�h�ŋN�����܂����H"), TEXT("sample"), MB_YESNO | MB_ICONQUESTION);
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


	// ==================== thread start ====================
	MyDataRec        MyData = { 'A', 1000 };
	unsigned int    threadID;
	DWORD            dwRet;
	gfStopFlag = FALSE;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Func, &MyData, 0, &threadID);

	rcvStopFlag = FALSE;
	rcvDataStr		myrcvData = { s, buf, sizeof(buf) };
	HANDLE rcvFncHd = (HANDLE)_beginthreadex(NULL, 0, rcvFunc, &myrcvData, 0, &threadID);

	while (ProcessMessage() == 0) {
		if (WaitForSingleObject(hThread, 50) == WAIT_OBJECT_0)
		{
			GetExitCodeThread(hThread, &dwRet);  // �X���b�h�֐�����̖߂�l�擾
			//fprintf(stderr, "Thread ended.endcode: % d\n", dwRet);
			DrawFormatString(0, 150, 0xffff, "Thread ended.endcode: % d.",dwRet);
			ScreenFlip();
			Sleep(1000);
			CloseHandle(hThread);
			break;
		}

		if (CheckKeyInput(InputHandle) != 0) { // enter ���������܂ŊY���n���h���ɓ��͎�t
			GetKeyInputString(String, InputHandle);
			DeleteKeyInput(InputHandle);

			InputHandle = MakeKeyInput(50, FALSE, FALSE, FALSE);
			SetActiveKeyInput(InputHandle);


			if (strcmp(String, "send foo") == 0) {
				//send foo 
			}
			if (strcmp(String, "stopThread") == 0) {
				gfStopFlag = TRUE;
			}
			if (strcmp(String, "stopRcv") == 0) {
				rcvStopFlag = TRUE;
			}
			if (strcmp(String, "exit") == 0) {
				break;
			}
			if (strcmp(String, "foo") == 0) {
				DrawString(0, 100, "foo was entered.", 0xffff);
			}
		}
		//DrawKeyInputModeString(0, 0);	// ���̓��[�h��`��
		DrawKeyInputString(0, 25, InputHandle); 	// ���͓r���̕������`��

		DrawFormatString(0, 100, 0xff00ff, buf);
		//memset(buf, 0, sizeof(buf)); 
		//int n = recv(s, buf, sizeof(buf), 0);

		/*if (n != -1) {
			printfDx("%d, %s\n", n, buf);
		}*/

		//DrawFormatString(0, 50, 0x33ffcc, "%d bytesReceived, \n received str: %s\n", n, buf);
		scn.lend();
	}


	closesocket(s);

	WSACleanup();

	
	
	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}
