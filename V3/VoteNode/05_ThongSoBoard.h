#include "c_types.h"
#ifndef _ThongSoBoard_h
#define _ThongSoBoard_h

#include "VoteNode.h"
#include <Arduino.h>  // Để khai báo kiểu String

// Từ vị trí 0-13 là vùng EEPROM chứa thông số board.
#define _ChieuDaiMaxThongSoBoard 13

class ThongSoBoard {
public:
  char MACSlave[_ChieuDaiMaxThongSoBoard];

public:
  void KhoiTao(void);
  void XoaEEPROM(int start, int end);

private:
  void LayThongSoBoardTuROM(void);
};

#endif
