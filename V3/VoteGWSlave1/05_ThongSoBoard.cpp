#include "05_ThongSoBoard.h"
#include <EEPROM.h>             // Thư viện để lưu dữ liệu thông số board vào bộ nhớ ROM

void ThongSoBoard::KhoiTao(void) {
#pragma region KhoiTao 
  // Lấy thông số cài đặt board từ bộ nhớ ROM để sử dụng.
  LayThongSoBoardTuROM();
#pragma endregion KhoiTao
}

// Lấy bộ thông số cài đặt của board từ trong EFROM. Thông số này
// được lấy ra để sử dụng trong quá trình thực thi của board.
void ThongSoBoard::LayThongSoBoardTuROM(void) {
#pragma region LayThongSoBoardTuROM
  for (int i = 0; i < _ChieuDaiMaxThongSoBoard; ++i) {
    MACMaster[i] = char(EEPROM.read(i));
  }

  #ifdef debug
  Serial.print("MACMaster: ");
  Serial.println(MACMaster);
  #endif
  // return thongso.c_str();
#pragma endregion LayThongSoBoardTuROM
}

// Xóa thông tin lưu trong bộ nhớ EFROM của ESP8266 từ start đến end.
void ThongSoBoard::XoaEEPROM(int start, int end) {
  for (int i = start; i < end; ++i) {
    EEPROM.write(i, 0);
    delay(10);
  }

  // The write command does not write directly to EEPROM. It just prepares an array
  // of bytes which must then be saved in the memory area using the commit() command.
  EEPROM.commit();
  delay(500);
}
