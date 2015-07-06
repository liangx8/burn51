// -*- coding:utf-8 -*-
#include "group.h"

/*
 * 根据模板生成ASM文件
 */
class GroupAsm: public Group{
private:
  HWND m_parent;
  HINSTANCE m_hInst;
public:
  GroupAsm(HWND hwnd,HINSTANCE hInst);
  void ReSize(RECT *);
  void Hide();
	
};
