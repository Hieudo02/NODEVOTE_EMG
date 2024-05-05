#ifndef _ESPNow_h
#define _ESPNow_h

#include "VoteNode.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>

#define MAX_CHANNEL 14  // for North America // 13 in Europe
const int DATA_SIZE = 18;  // Kích thước chuỗi data được gửi từ Slave hoặc Node lên cho Master

#define _ChanDocPin 17
#define LEDBat 16

class ESPNOW {
public:
  static int32_t WiFiChannel;
  static int8_t channels[];
  static int currentChannelIndex;

  static char MACSlave[13];
  static uint8_t MACSlaveDinhDangDung[6];
  
  static bool FlagBatTay;
  static bool FlagNhayLED;
  bool FlagPinYeu = false;
  uint8_t PhatBuzzer = 0;

public:
  void KhoiTao(int role);  // Khởi tạo ESP-NOW với role (vai trò master-slave).
  void KiemTraDungLuongPin(void);
  void GuiDuLieuQuaESPNOW(void);  // Gửi đáp án lên slave
  void NhayLED(void);
  void BuzzerKhiPinYeu(void);

private:
  int _ChanLedBaoDapAnDung = 16;  // Chân LED dùng để nháy khi trả lời đúng
  char _GiaTriNutNhan;

  static void TrangThaiGuiGoiTinCuaNutNhan(uint8_t* mac_addr, uint8_t sendStatus);
  static void NhanDuLieuQuaESPNOW(uint8_t* MAC, uint8_t* DuLieu, uint8_t ChieuDaiDuLieu);
};

#endif
