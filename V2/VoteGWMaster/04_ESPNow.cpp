#include "04_ESPNow.h"
#include <ESP8266WiFi.h>

char ESPNOW::MAC[13] = { '\0' };                    // Số MAC của node hiện đang gửi dữ liệu.
char ESPNOW::DuLieuNhanDuoc[DATA_SIZE] = { '\0' };  // Mảng ký tự nhận được từ Slave hoặc Node
char ESPNOW::DapAn;                                 // Đáp án 0-1-2-3 tương ứng A-B-C-D do node gửi lên.

void ESPNOW::KhoiTao(int role) {
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Khởi tạo ESP-NOW thành công");
  }

  // Khởi tạo ESP-NOW với role (vai trò master-slave)
  esp_now_set_self_role(role);  // COMBO

  // Register function NhanDuLieuQuaESPNOW
  esp_now_register_recv_cb(ESPNOW::NhanDuLieuQuaESPNOW);
}

void ESPNOW::NhanDuLieuQuaESPNOW(uint8_t* MAC, uint8_t* DuLieu, uint8_t ChieuDaiDuLieu) {
  /***SLAVE to MASTER***/
  if (ChieuDaiDuLieu > DATA_SIZE - 1) {
    ChieuDaiDuLieu = DATA_SIZE - 1;  // Giới hạn chuỗi nhận được để vừa với buffer
  }

  if (*DuLieu != 13) {
    memcpy(DuLieuNhanDuoc, DuLieu, ChieuDaiDuLieu);
    DuLieuNhanDuoc[ChieuDaiDuLieu] = '\0';  // Ký tụ NULL kết thúc chuỗi
    strncpy(ESPNOW::MAC, DuLieuNhanDuoc + 3, 12);
    ESPNOW::MAC[12] = '\0';  // Ký tụ NULL kết thúc chuỗi
    DapAn = DuLieuNhanDuoc[ChieuDaiDuLieu - 1];

// #ifdef debug
//     Serial.print("Dữ liệu từ slaver gửi tới master: ");
//     Serial.println(DuLieuNhanDuoc);
//     Serial.print("Số MAC của node hiện đang gửi dữ liệu: ");
//     Serial.println(ESPNOW::MAC);
//     Serial.print("Đáp án do node gửi lên: ");
//     Serial.println(ESPNOW::DapAn);
// #endif
  }
}
