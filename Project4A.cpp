// Project4A.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "Project4A.h"
#include "ClangAST.h"
#include "ReadFile.cpp"

//#include <Windows.h>
#include <Commdlg.h>
#pragma comment(lib, "Comdlg32.lib")

using namespace std;

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;                                // ���݂̃C���^�[�t�F�C�X
WCHAR szTitle[MAX_LOADSTRING];                  // �^�C�g�� �o�[�̃e�L�X�g
WCHAR szWindowClass[MAX_LOADSTRING];            // ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND mainHwnd, Dxhwnd;
HWND texthwnd;

char ReadFiles::filepath[100] = "TestCode\\Sample.cpp";
int ReadFiles::fileHandle = 0;

int DXInit(int x, int y) {
	SetOutApplicationLogValidFlag(FALSE);//Log.txt�̏o��
	ChangeWindowMode(TRUE);

	//SetUserWindow(Dxhwnd);
	//SetDrawArea(0, 0, 700, 700);
	SetWindowSizeChangeEnableFlag(TRUE, FALSE);
	SetGraphMode(x, y, 16);//�`��͈͂̎w��
	//SetWindowText("TITLE");
	SetAlwaysRunFlag(TRUE);//�ŏ�����̕\���Ɏg�p
	SetWindowVisibleFlag(FALSE);//DXLib window�̍폜
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
// �q�E�C���h�E�̃v���[�W��
LRESULT CALLBACK ChildProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// �W�������ɂ��C��
	return DefWindowProc(hWnd, msg, wp, lp);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �����ɃR�[�h��}�����Ă��������B

    // �O���[�o������������������Ă��܂��B
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT4A, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // �A�v���P�[�V�����̏����������s���܂�:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	DXInit(700, 750);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT4A));

    MSG msg;

	//ReadFiles::SetFilePath(filepath);
	ReadFiles::PrintFile(texthwnd);

	PrintAST(ReadFiles::GetFilepath());
	ScreenFlip();

    // ���C�� ���b�Z�[�W ���[�v:
    while (GetMessage(&msg, nullptr, 0, 0) && ProcessMessage() == 0)
    {
		clsDx();
		ClearDrawScreen();
		PrintAST(ReadFiles::GetFilepath());
		ScreenFlip();//��ɕ\�������邽��
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	//WaitKey(); // �L�[���͑҂�
	DxLib_End(); // �c�w���C�u�����g�p�̏I������
	
    return (int) msg.wParam;
}

//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
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
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

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
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:    ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND  - �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT    - ���C�� �E�B���h�E�̕`��
//  WM_DESTROY  - ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR strFile[MAX_PATH];
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			static OPENFILENAME ofn = { 0 };
            // �I�����ꂽ���j���[�̉��:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			
			/*�t�@�C���_�C�A���O*/
			case IDM_FILEOPEN:
			case IDM_FILESAVE:
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = TEXT("C++ SorceFIle {*.cpp}\0*.cpp\0")
					TEXT("All files {*.*}\0*.*\0\0");
				ofn.lpstrFile = strFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
				switch (wmId) {
					case IDM_FILEOPEN:
						GetOpenFileName(&ofn);
						ReadFiles::SetFilePath(ofn.lpstrFile);
						ReadFiles::PrintFile(texthwnd);
						break;
					case IDM_FILESAVE:
						GetSaveFileName(&ofn);
						break;
					default:
						return 0;
				}
				//MessageBox(0, strFile, _TEXT("�I�����ꂽ�t�@�C����"), MB_OK);
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
            // TODO: HDC ���g�p����`��R�[�h�������ɒǉ����Ă�������...
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
		SetWindowVisibleFlag(FALSE);//DXLib window�̍폜
		SetWindowPos(
			Dxhwnd,               // �E�C���h�E�̃n���h��
			NULL,    // �z�u�����̃n���h��
			0,                  // �������̈ʒu
			0,                  // �c�����̈ʒu
			width / 2,                 // ��
			height,                 // ����
			NULL//SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED             // �E�C���h�E�ʒu�̃I�v�V����
		);
		//SetWindowSizeChangeEnableFlag(TRUE);
		SetGraphMode(width / 2, height, 16);//�`��͈͂̎w��
		SetAlwaysRunFlag(TRUE);//�ŏ�����̕\���Ɏg�p
		SetWindowVisibleFlag(FALSE);//DXLib window�̍폜
		SetScreenFlipTargetWindow(Dxhwnd);
		//SetNotWinFlag(FALSE);
		
		SetWindowPos(
			texthwnd,               // �E�C���h�E�̃n���h��
			NULL,    // �z�u�����̃n���h��
			width / 2,                  // �������̈ʒu
			0,                  // �c�����̈ʒu
			width / 2,                 // ��
			height,                 // ����
			NULL//SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED             // �E�C���h�E�ʒu�̃I�v�V����
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
			25,                   // �t�H���g�̍���(�傫��)�B
			0,                    // �t�H���g�̕��B���ʂO�B
			0,                    // �p�x�B�O�łn�j�B
			0,                    // �������p�x�B������O�B
			FW_DONTCARE,          // �����̑����B
			FALSE,                // �t�H���g���C�^���b�N�Ȃ�TRUE���w��B
			FALSE,                // �����������Ȃ�TRUE�B
			FALSE,                // ���������������Ȃ�TRUE�B
			SHIFTJIS_CHARSET,     // �t�H���g�̕����Z�b�g�B���̂܂܂łn�j�B
			OUT_DEFAULT_PRECIS,   // �o�͐��x�̐ݒ�B���̂܂܂łn�j�B
			CLIP_DEFAULT_PRECIS,  // �N���b�s���O���x�B���̂܂܂łn�j�B
			DRAFT_QUALITY,        // �t�H���g�̏o�͕i���B���̂܂܂łn�j�B
			DEFAULT_PITCH,        // �t�H���g�̃s�b�`�ƃt�@�~�����w��B���̂܂܂łn�j�B
			_T("Consolas") // �t�H���g�̃^�C�v�t�F�C�X���̎w��B����͌����܂�܁B
		);
		SendMessage(texthwnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
	}
	break;
	
	/*menu�o�[�̔������Ȃ��Ȃ�*/
	/*
	case WM_SYSCOMMAND:
	
		if (LOWORD(wParam) == IDM_KITTY) {
			GetSystemMenu(mainHwnd, TRUE);
		}
	
	break;
	*/
	
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
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
