#ifndef _ThongSoBoard_h
#define _ThongSoBoard_h

#include "VoteGWMaster.h"
#include <Arduino.h>  // Để khai báo kiểu String

class ThongSoBoard {
public:
  String Realm;
  String IPHost;
  String Port;
  String Position;

public:
  void KhoiTao(void);
  void XoaEEPROM(int start, int end);

private:
  void LayThongSoBoardTuROM(void);
};

#endif
