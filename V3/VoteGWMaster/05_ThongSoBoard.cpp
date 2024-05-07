#include "05_ThongSoBoard.h"
#include <EEPROM.h>  // Thư viện để lưu dữ liệu thông số board vào bộ nhớ ROM

// Từ vị trí 0-409 là vùng EEPROM chứa thông số board.
#define _ChieuDaiMaxThongSoBoard 409
#define EEPROM_REALM 50
#define EEPROM_IPHOST 150
#define EEPROM_PORT 200
#define EEPROM_POSITION 250

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
  if (EEPROM.read(0) != 0) {
    for (int i = 0; i < EEPROM_REALM; ++i) {
      Realm += char(EEPROM.read(i));
    }

    for (int i = EEPROM_REALM; i < EEPROM_IPHOST; ++i) {
      IPHost += char(EEPROM.read(i));
    }

    for (int i = EEPROM_IPHOST; i < EEPROM_PORT; ++i) {
      Port += char(EEPROM.read(i));
    }

    for (int i = EEPROM_PORT; i < EEPROM_POSITION; ++i) {
      Position += char(EEPROM.read(i));
    }

    Realm = Realm.c_str();
    IPHost = IPHost.c_str();
    Port = Port.c_str();
    Position = Position.c_str();
  }

#ifdef debug
  Serial.print("Realm: ");
  Serial.println(Realm);
  Serial.print("IPHost: ");
  Serial.println(IPHost);
  Serial.print("Port: ");
  Serial.println(Port);
  Serial.print("Position: ");
  Serial.println(Position);
#endif
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
