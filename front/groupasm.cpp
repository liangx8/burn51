// -*- coding:utf-8 -*-
#include <libintl.h>
#include "groupasm.h"

GroupAsm::GroupAsm(HWND hwnd,HINSTANCE hInst):m_parent(hwnd),m_hInst(hInst){
  CreateWindow("STATIC",gettext("Template"),WS_CHILD|WS_VISIBLE,
						1,// x
						30,// y
						80,//width
						20,//height
						hwnd,
						NULL,
						hInst,
						NULL);
}
void GroupAsm::ReSize(RECT *prc){
}
void GroupAsm::Hide(){
}
