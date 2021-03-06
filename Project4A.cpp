// Project4A.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include <windows.h>
#include "Project4A.h"
#include "ClangAST.h"
#include "ReadFile.cpp"

using namespace std;

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND mainHwnd, Dxhwnd;
HWND texthwnd;

int DXInit(int x, int y) {
	SetOutApplicationLogValidFlag(FALSE);//Log.txtの出力
	ChangeWindowMode(TRUE);

	//SetUserWindow(Dxhwnd);
	//SetDrawArea(0, 0, 700, 700);
	SetWindowSizeChangeEnableFlag(TRUE, FALSE);
	SetGraphMode(x, y, 16);//描画範囲の指定
	//SetWindowText("TITLE");
	SetAlwaysRunFlag(TRUE);//最小化後の表示に使用
	SetWindowVisibleFlag(FALSE);//DXLib windowの削除
	//SetNotWinFlag(FALSE);

	if (Dxhwnd == NULL || DxLib_Init() == -1) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);

	HWND hMainWnd = GetMainWindowHandle();
	//SetUserChildWindow(Dxhwnd);
	SetScreenFlipTargetWindow(Dxhwnd);
	ScreenFlip();
	//CloseWindow(hMainWnd);
	//DestroyWindow(hMainWnd);
	return 1;
}
// 子ウインドウのプロージャ
LRESULT CALLBACK ChildProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 標準処理にお任せ
	return DefWindowProc(hWnd, msg, wp, lp);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT4A, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	DXInit(700, 700);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT4A));

    MSG msg;

	ReadFiles readfile;

	char* filepath = "TestCode\\Sample.cpp";
	readfile.SetFilePath(filepath);
	readfile.PrintFile(texthwnd);

	clsDx();
	PrintAST(filepath);
	ScreenFlip();

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0) && ProcessMessage() == 0)
    {
		clsDx();
		PrintAST(filepath);
		ScreenFlip();//常に表示させるため
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	//WaitKey(); // キー入力待ち
	DxLib_End(); // ＤＸライブラリ使用の終了処理
	
    return (int) msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT4A));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECT4A);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   mainHwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
  
   if (!mainHwnd)
   {
      return FALSE;
   }

   ShowWindow(mainHwnd, nCmdShow);
   UpdateWindow(mainHwnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		if (hWnd == mainHwnd) {
			PostQuitMessage(0);
		}
        break;
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		clsDx();
		SetWindowVisibleFlag(FALSE);//DXLib windowの削除
		SetWindowPos(
			Dxhwnd,               // ウインドウのハンドル
			NULL,    // 配置順序のハンドル
			0,                  // 横方向の位置
			0,                  // 縦方向の位置
			width / 2,                 // 幅
			height,                 // 高さ
			NULL//SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED             // ウインドウ位置のオプション
		);
		//SetWindowSizeChangeEnableFlag(TRUE);
		SetGraphMode(width / 2, height, 16);//描画範囲の指定
		SetAlwaysRunFlag(TRUE);//最小化後の表示に使用
		SetWindowVisibleFlag(FALSE);//DXLib windowの削除
		SetScreenFlipTargetWindow(Dxhwnd);
		//SetNotWinFlag(FALSE);
		
		SetWindowPos(
			texthwnd,               // ウインドウのハンドル
			NULL,    // 配置順序のハンドル
			width / 2,                  // 横方向の位置
			0,                  // 縦方向の位置
			width / 2,                 // 幅
			height,                 // 高さ
			NULL//SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED             // ウインドウ位置のオプション
		);

	}
		break;
	case WM_CREATE:
	{
		Dxhwnd = CreateWindow(TEXT("STATIC"), TEXT("ERROR"),
			WS_CHILD | WS_VISIBLE,
			0, 0, 700, 700, hWnd, (HMENU)1,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
		);

		texthwnd = CreateWindow(
			TEXT("EDIT"), TEXT("default Text"),
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL |
			ES_LEFT | ES_MULTILINE | WS_BORDER,
			700, 0, 500, 700, hWnd, (HMENU)2,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
		);

		HFONT font = CreateFont(
			25,                   // フォントの高さ(大きさ)。
			0,                    // フォントの幅。普通０。
			0,                    // 角度。０でＯＫ。
			0,                    // 同じく角度。これも０。
			FW_DONTCARE,          // 文字の太さ。
			FALSE,                // フォントがイタリックならTRUEを指定。
			FALSE,                // 下線を引くならTRUE。
			FALSE,                // 取り消し線を引くならTRUE。
			SHIFTJIS_CHARSET,     // フォントの文字セット。このままでＯＫ。
			OUT_DEFAULT_PRECIS,   // 出力精度の設定。このままでＯＫ。
			CLIP_DEFAULT_PRECIS,  // クリッピング精度。このままでＯＫ。
			DRAFT_QUALITY,        // フォントの出力品質。このままでＯＫ。
			DEFAULT_PITCH,        // フォントのピッチとファミリを指定。このままでＯＫ。
			_T("Consolas") // フォントのタイプフェイス名の指定。これは見たまんま。
		);
		SendMessage(texthwnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
	}
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
