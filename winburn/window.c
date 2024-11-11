#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"
#include "burn.h"

wchar_t *wcb;
struct data *pm;

HWND g_hStatus;

void a2u(wchar_t *wc,const char *sn);

const wchar_t g_szClassName[] = L"myWindowClass";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
		case WM_CREATE:
		{
			wcb=malloc(256);
			LPCREATESTRUCTA cs=(LPCREATESTRUCTA)lParam;
			wprintf(L"main window created!\n[cx:%d][cy:%d][x:%d][y:%d]\n",cs->cx,cs->cy,cs->x,cs->y);
			g_hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
				hwnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

			int statwidths[] = {50,150,-1};

			SendMessage(g_hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
			if(pm->str){
				a2u(wcb,pm->str);
				SendMessage(g_hStatus, SB_SETTEXT, 0, (LPARAM)wcb);
			} else {
				SendMessage(g_hStatus, SB_SETTEXT, 0, (LPARAM)L"准备");
			}
			
			SendMessage(g_hStatus, SB_SETTEXT, 1, (LPARAM)L"ihex");
			
			HWND hStatic = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", L"文本", 
            WS_CHILD | WS_VISIBLE  | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
            0, 0, 180, 100, hwnd, (HMENU)IDC_MAIN_STATIC, GetModuleHandle(NULL), NULL);
			if(hStatic==NULL){
				wprintf(L"static 控件创建错误\n");
			}
			
		}
			break;
		case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_EXIT:
					PostQuitMessage(0);
                break;
                case ID_STUFF_GO:

                break;
            }
        break;
        case WM_CLOSE:
			free(wcb);
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
		case WM_SIZE:
		{

			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			wprintf(L"width: %d,height: %d\n",width,height);

			SendMessage(g_hStatus,WM_SIZE,0,0);
		}
		break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI GuiMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   struct data *pd, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;
	pm=pd;
	InitCommonControls();

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(
			NULL, 
			L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"单片机烧写",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
