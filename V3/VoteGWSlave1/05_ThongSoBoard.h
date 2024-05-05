#include "c_types.h"
#ifndef _ThongSoBoard_h
#define _ThongSoBoard_h

#include "VoteGWSlave1.h"
#include <Arduino.h>  // Để khai báo kiểu String

// Từ vị trí 0-409 là vùng EEPROM chứa thông số board.
#define _ChieuDaiMaxThongSoBoard 409

class ThongSoBoard {
public:
  char MACMaster[_ChieuDaiMaxThongSoBoard];

public:
  void KhoiTao(void);

private:
  void XoaEEPROM(int start, int end);
  void LayThongSoBoardTuROM(void);
};

#endif
