// drawing.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

//#include "stdafx.h"
#include <windows.h>		//Windows環境
#include <windowsx.h>		//Windows環境
#include <stdio.h>			//入出力用
#include <process.h>		//スレッド用
#include <stdlib.h>

#include "header.h"			//ヘッダーファイル
#include "resource.h"		//リソースファイル

#pragma comment(lib,"winmm.lib")//高精度タイマ

//構造体
typedef struct {
	HWND	hWnd;
	HWND	hwnd[2];
}SEND_POINTER_STRUCT;



//構造体


//メイン関数(ダイアログバージョン)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HANDLE hMutex;

	//多重起動判定
	hMutex = CreateMutex(NULL, TRUE, DEF_MUTEX_NAME);		//ミューテックスオブジェクトの生成
	if (GetLastError() == ERROR_ALREADY_EXISTS) {				//2重起動の有無を確認
		MessageBox(NULL, TEXT("既に起動されています．"), NULL, MB_OK | MB_ICONERROR);
		return 0;											//終了
	}

	//ダイアログ起動
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, MainDlgProc);

	return FALSE;			//終了
}

//メインプロシージャ（ダイアログ）
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;				//フォント
	static HANDLE hThread;
	static UINT thID;
	static SEND_POINTER_STRUCT Sps;
	//static HWND hEdit;

	static HWND hPict[4];
	//static HWND hWnd[4];

	switch (uMsg) {
	case WM_INITDIALOG:		//ダイアログ初期化
		Sps.hWnd = hDlg;
		//hEdit = GetDlgItem(hDlg, IDC_EDIT1);

		//背景，軸を表示
		hPict[0] = GetDlgItem(hDlg, IDC_PICTBOX1);
		WinInitialize(NULL, hDlg, (HMENU)110, "TEST1", hPict[0], WndProc, &Sps.hwnd[0]);
		//初期化
		//WinInitialize関数によって子ウィンドウプロシージャにhPict1がhWnd1としてセットされました．
		hPict[1] = GetDlgItem(hDlg, IDC_PICTBOX2);
		WinInitialize(NULL, hDlg, (HMENU)110, "TEST2", hPict[1], WndProc, &Sps.hwnd[1]);
		//初期化
		//WinInitialize関数によって子ウィンドウプロシージャにhPict2がhWnd2としてセットされました．
		//hPict[2] = GetDlgItem(hDlg, IDC_PICTBOX3);
		//WinInitialize(NULL, hDlg, (HMENU)110, "TEST3", hPict[2], WndProc, &Sps.hwnd[2]);
		//初期化
		//WinInitialize関数によって子ウィンドウプロシージャにhPict3がhWnd3としてセットされました．
		//hPict[3] = GetDlgItem(hDlg, IDC_PICTBOX4);
		//WinInitialize(NULL, hDlg, (HMENU)110, "TEST4", hPict[3], WndProc, &Sps.hwnd[3]);
		//初期化
		//WinInitialize関数によって子ウィンドウプロシージャにhPict4がhWnd4としてセットされました．
		return TRUE;

	case WM_COMMAND:		//ボタンが押された時
		switch (LOWORD(wParam)) {
		case ID_START:			//OKボタン



								//データ読み込みスレッド起動
			hThread = (HANDLE)_beginthreadex(NULL, 0, TFunc, (PVOID)&Sps, 0, &thID);   //_beginthreadex→スレッドを立ち上げる関数	
			EnableWindow(GetDlgItem(hDlg, ID_START), FALSE);						//開始ボタン無効化　　　　//EnableWindowで入力を無効または有効にする。

			return TRUE;

		case ID_STOP:	//停止・再開ボタン


			if (ResumeThread(hThread) == 0) {					//停止中かを調べる(サスペンドカウントを１減らす)
				SetDlgItemText(hDlg, ID_STOP, TEXT("restart"));	//再開に変更　　　　　　　　　　　　　　　　　　　//SetDlgItemTextでダイアログ内のテキストなどを変更することができる
				SuspendThread(hThread);						//スレッドの実行を停止(サスペンドカウントを１増やす)
			}
			else
				SetDlgItemText(hDlg, ID_STOP, TEXT("stop"));	//停止に変更

			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, 0);			//ダイアログ終了
		return TRUE;
	}

	return FALSE;
}

//データ読み込み用スレッド
UINT WINAPI TFunc(LPVOID thParam)
{
	static SEND_POINTER_STRUCT *FU = (SEND_POINTER_STRUCT*)thParam;        //構造体のポインタ取得

	FILE *fp;			//ファイルポインタ
	BOOL Flag = TRUE;		//ループフラグ
	HFONT hFont;		//フォント

	double data1, data2;		//データ(ファイルから得る値)



	int width1, width2, hight1, hight2, width3, width4, hight3, hight4;				//PICTBOXの幅
	RECT rect1, rect2, rect3, rect4;
	GetClientRect(FU->hwnd[0], &rect1);	//PICTBOXの情報を入手(大きさ一緒だけど一応)
	GetClientRect(FU->hwnd[1], &rect2);

	width1 = rect1.left + rect1.right;
	width2 = rect2.left + rect2.right;
	hight1 = rect1.top + rect1.bottom;
	hight2 = rect2.top + rect2.bottom;

	DWORD DNum = 0, beforeTime;

	int time = 0;

	int y1 = 0, y2 = 0, time1 = 0, time2 = 0;				//プロットするために変換する値

	int by1 = hight1 / 2, by2 = hight2 / 2;
	int btime1 = width1*0.1, btime2 = width2*0.1;			//前回の値

	beforeTime = timeGetTime();						//現在の時刻計算（初期時間）

													//ファイルオープン
	if ((fp = fopen("data.txt", "r")) == NULL) {
		MessageBox(NULL, TEXT("Input File Open ERROR!"), NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//データ読み込み・表示
	while (Flag == TRUE) {
		DWORD nowTime, progress, idealTime;

		//時間の調整
		nowTime = timeGetTime();					//現在の時刻計算
		progress = nowTime - beforeTime;				//処理時間を計算
		idealTime = (DWORD)(DNum * (1000.0F / (double)DEF_DATAPERS));	//理想時間を計算
		if (idealTime > progress) Sleep(idealTime - progress);			//理想時間になるまで待機

																		//データの読み込み
		if (fscanf(fp, "%lf %lf", &data1, &data2) == EOF) {
			MessageBox(NULL, TEXT("終了"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
			EnableWindow(GetDlgItem(FU->hwnd[0], ID_START), TRUE);		//開始ボタン有効
			Flag = FALSE;												//ループ終了フラグ
			return FALSE;
		}

		//表示
		//読み込んだ値を変換
		//データをPICTBOXに合わせて変形(*5してるのは適当です…波形見て判断してください)
		y1 = data1 * 0.8 * hight1 / 2 * 0.8;
		y2 = data2 * 0.8 * hight2 / 2 * 0.8;


		//データの表示(プロット)
		//一回に3秒(3000データ)分表示するとき(幅によって被って描画されるかも)
		PLOT(FU->hwnd[0], width1*0.1 + time*width1 / (3 * DEF_DATAPERS), hight1 / 2 - y1, btime1, by1);
		PLOT(FU->hwnd[1], width2*0.1 + time*width2 / (3 * DEF_DATAPERS), hight2 / 2 - y2, btime2, by2);


		by1 = hight1 / 2 - y1;
		by2 = hight2 / 2 - y2;

		btime1 = width1*0.1 + time*width1 / (3 * DEF_DATAPERS);
		btime2 = width2*0.1 + time*width2 / (3 * DEF_DATAPERS);

		time++;
		DNum++;

		//一秒経過時
		if (progress >= 1000.0) {
			beforeTime = nowTime;
			DNum = 0;
		}

		if (time > 0.8*DEF_DATAPERS * 3) {
			//if(time >0.8*width1){		//1msごとに+1
			//横軸を0に戻し、背景を黒くし軸を描く
			time = 0;
			btime1 = width1*0.1;
			btime2 = width2*0.1;
			REPaint(FU->hwnd[0]);
			REPaint(FU->hwnd[1]);
		}


	}

	return 0;
}


//子ウィンドウプロシージャ
//軸を描く
HRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン

	if (uMsg == WM_PAINT) {
		HDC hdc = BeginPaint(hWnd, &ps);
		int width, hight;

		color = RGB(0, 0, 0);		//黒
		colorPen = RGB(255, 255, 255);
		hBrush = CreateSolidBrush(color);				//ブラシ生成
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
		hPen = CreatePen(PS_SOLID, 2, colorPen);		//ペン生成
		hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

														//PICTBOXの大きさ
		RECT rect;
		GetClientRect(hWnd, &rect);
		width = rect.left + rect.right;
		hight = rect.top + rect.bottom;

		//黒く塗りつぶす
		Rectangle(hdc, 0, 0, width + 1, hight + 1);

		LPPOINT IpPoint = (0, 0);
		MoveToEx(hdc, width*0.1, hight*0.1, IpPoint);
		LineTo(hdc, width*0.1, hight*0.9);
		MoveToEx(hdc, width*0.1, hight*0.5, IpPoint);
		LineTo(hdc, width*0.9, hight*0.5);


		//横軸のタイトル
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(0, 0, 0));
		TextOut(hdc, rect.right*0.42, rect.bottom*0.8, TEXT("Time(s)"), 7);
		//TextOut(hdc, 10, 10, TEXT("ch1"), 3);

		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
		//デバイスコンテキストのハンドル破棄
		EndPaint(hWnd, &ps);

	}
	return TRUE;
}

//波形描画
BOOL PLOT(HWND hWnd, int sec, int data, int bsec, int bdata) {
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = GetDC(hWnd);
	//hdc = BeginPaint(hWnd,&ps);
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン
	colorPen = RGB(0, 255, 255);
	hBrush = CreateSolidBrush(color);				//ブラシ生成
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
	hPen = CreatePen(PS_SOLID, 2, colorPen);		//ペン生成
	hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

	LPPOINT IpPoint = (0, 0);
	MoveToEx(hdc, bsec, bdata, IpPoint);
	LineTo(hdc, sec, data);


	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	ReleaseDC(hWnd, hdc);

	return TRUE;
}
//背景、軸の再描画
BOOL REPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HBRUSH		hBrush, hOldBrush;	//ブラシ
	HPEN		hPen, hOldPen;		//ペン

	HDC hdc;
	hdc = GetDC(hWnd);
	//hdc = BeginPaint(hWnd, &ps);
	int width, hight;

	color = RGB(0, 0, 0);		//黒
	colorPen = RGB(255, 255, 255);
	hBrush = CreateSolidBrush(color);				//ブラシ生成
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);	//ブラシ設定
	hPen = CreatePen(PS_SOLID, 2, colorPen);		//ペン生成
	hOldPen = (HPEN)SelectObject(hdc, hPen);		//ペン設定

													//PICTBOXの大きさ
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = rect.left + rect.right;
	hight = rect.top + rect.bottom;

	//黒く塗りつぶす
	Rectangle(hdc, 0, 0, width + 1, hight + 1);

	//軸を描く
	LPPOINT IpPoint = (0, 0);
	MoveToEx(hdc, width*0.1, hight*0.1, IpPoint);
	LineTo(hdc, width*0.1, hight*0.9);
	MoveToEx(hdc, width*0.1, hight*0.5, IpPoint);
	LineTo(hdc, width*0.9, hight*0.5);

	//横軸のタイトル
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkColor(hdc, RGB(0, 0, 0));
	TextOut(hdc, rect.right*0.42, rect.bottom*0.8, TEXT("Time[s]"), 7);
	//TextOut(hdc, 10, 10, TEXT("ch1"), 3);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	//デバイスコンテキストのハンドル破棄
	EndPaint(hWnd, &ps);

	return TRUE;
}




BOOL WinInitialize(HINSTANCE hInst, HWND hPaWnd, HMENU chID, char *cWinName, HWND PaintArea, WNDPROC WndProc, HWND *hWnd)
{
	WNDCLASS wc;			//ウィンドウクラス
	WINDOWPLACEMENT	wplace;	//子ウィンドウ生成領域計算用（画面上のウィンドウの配置情報を格納する構造体）
	RECT WinRect;			//子ウィンドウ生成領域
	ATOM atom;				//アトム

							//ウィンドウクラス初期化
	wc.style = CS_HREDRAW ^ WS_MAXIMIZEBOX | CS_VREDRAW;	//ウインドウスタイル
	wc.lpfnWndProc = WndProc;									//ウインドウのメッセージを処理するコールバック関数へのポインタ
	wc.cbClsExtra = 0;											//
	wc.cbWndExtra = 0;
	wc.hCursor = NULL;										//プログラムのハンドル
	wc.hIcon = NULL;										//アイコンのハンドル
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		//ウインドウ背景色
	wc.hInstance = hInst;										//ウインドウプロシージャがあるインスタンスハンドル
	wc.lpszMenuName = NULL;										//メニュー名
	wc.lpszClassName = (LPCTSTR)cWinName;									//ウインドウクラス名

	if (!(atom = RegisterClass(&wc))) {
		MessageBox(hPaWnd, TEXT("ウィンドウクラスの生成に失敗しました．"), NULL, MB_OK | MB_ICONERROR);
		return false;
	}

	GetWindowPlacement(PaintArea, &wplace);	//描画領域ハンドルの情報を取得(ウィンドウの表示状態を取得)
	WinRect = wplace.rcNormalPosition;		//描画領域の設定

											//ウィンドウ生成
	*hWnd = CreateWindow(
		(LPCTSTR)atom,
		(LPCTSTR)cWinName,
		WS_CHILD | WS_VISIBLE,//| WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME |WS_VISIBLE, 
		WinRect.left, WinRect.top,
		WinRect.right - WinRect.left, WinRect.bottom - WinRect.top,
		hPaWnd, chID, hInst, NULL
	);

	if (*hWnd == NULL) {
		MessageBox(hPaWnd, TEXT("ウィンドウの生成に失敗しました．"), NULL, MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}
