// -*- coding:utf-8 -*-
#include <windows.h>
class Group{
 public:
 Group(){}
  virtual ~Group(){}
  virtual void ReSize(RECT *)=0;
  virtual void Hide()=0;
};

