// -*-coding:utf-8-*-
// 生成烧写模板
/*
xgettext -o fw.pot fw.c
msginit -l zh_CN -i fw.pot
msgfmt zh_CN.po -o locale/zh_CN/LC_MESSAGES/fw.mo
*/

#include <stdio.h>
#include <libintl.h>
#include <windows.h>
#include <commctrl.h>

#include "groupasm.h"
#define LOCALEDIR "locale/"
#define PACKAGE "fw"

#ifndef UNICODE
#define UNICODE
#endif
HINSTANCE appInst;
HWND g_hTab;
const static char g_szClassName[]="FrameworkWindowClass";

Group *g1=NULL;
void CreateComponent(HWND hwnd){
  TC_ITEM t1;
  RECT rc;
  

  GetClientRect(hwnd,&rc);
  g_hTab=CreateWindow(WC_TABCONTROL,
					  "",
					  WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
					  0,//X
					  0,//Y
					  rc.right,//WIDTH
					  rc.bottom,//HEIGHT
					  hwnd,
					  NULL,
					  appInst,
					  NULL);
  if(g_hTab == NULL){
	printf("error");
	return;
  }
  g1=new GroupAsm(g_hTab,appInst);
  t1.mask=TCIF_TEXT;
  t1.pszText=gettext("Burn with template");
  TabCtrl_InsertItem(g_hTab,0,&t1);
  t1.pszText=gettext("Burn");
  TabCtrl_InsertItem(g_hTab,1,&t1);

}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_SIZE:
	  {
		RECT rc;
		HDWP hdwp;
		SetRect(&rc,0,0,LOWORD(lParam),HIWORD(lParam));
		TabCtrl_AdjustRect(g_hTab,FALSE,&rc);
		// size the tab control to fit the client area
		hdwp = BeginDeferWindowPos(2);
		DeferWindowPos(hdwp,g_hTab,NULL,0,0,
					   LOWORD(lParam),HIWORD(lParam),
					   SWP_NOMOVE | SWP_NOZORDER);
		g1->ReSize(&rc);
		EndDeferWindowPos(hdwp);
	  }
	  break;
	case WM_CREATE:
	  CreateComponent(hwnd);
	  break;
	case WM_CLOSE:
	  DestroyWindow(hwnd);
	  break;
	case WM_DESTROY:
	  PostQuitMessage(0);
	  break;
	default:
	  return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow){
	bindtextdomain(PACKAGE,LOCALEDIR);
	textdomain(PACKAGE);
	appInst=hInstance;
	InitCommonControls();

	WNDCLASSEX wc;
	MSG msg;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style =0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance,NULL);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(hInstance,NULL);

    if(!RegisterClassEx(&wc))
    {
	  MessageBox(NULL, gettext("Window Registration Failed!"), gettext("Error!"),MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,//WS_EX_CLIENTEDGE,
        g_szClassName,
        gettext("Burn file utility"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL){
	  MessageBox(NULL, gettext("Window Creation Failed!"), gettext("Error!"),MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);


    // Step 3: The Message Loop

    while(GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	return 0;

}
