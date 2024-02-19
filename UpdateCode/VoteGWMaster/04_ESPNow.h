#ifndef _ESPNow_h
#define _ESPNow_h

#include "VoteGWMaster.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>
#include <espnow.h>

const int DATA_SIZE = 18;  // Kích thước chuỗi data được gửi từ Slave hoặc Node lên cho Master

class ESPNOW {
public:
  static char MAC[13];                    // Số MAC của node hiện đang gửi dữ liệu.
  static char DuLieuNhanDuoc[DATA_SIZE];  // Mảng ký tự nhận được từ Slave hoặc Node
  static char DapAn;                      // Đáp án 0-1-2-3 tương ứng A-B-C-D do node gửi lên.
  static bool FlagKiemTraDaThamGiaGame;

public:
  void KhoiTao(int role);  // Khởi tạo ESP-NOW với role (vai trò master-slave).

private:
  static void NhanDuLieuQuaESPNOW(uint8_t* MAC, uint8_t* DuLieu, uint8_t ChieuDaiDuLieu);
};

#endif
