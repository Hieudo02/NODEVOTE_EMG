#ifndef _ESPNow_h
#define _ESPNow_h

#include "VoteNode.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include "74HC595.h"

#define MAX_CHANNEL 14  // for North America // 13 in Europe
const int DATA_SIZE = 18;  // Kích thước chuỗi data được gửi từ Slave hoặc Node lên cho Master

class ESPNOW:public LIB_74HC595 {
public:
  static int32_t WiFiChannel;
  static int8_t channels[];
  static int currentChannelIndex;

  static char MACSlave[13];
  static uint8_t MACSlaveDinhDangDung[6];
  
  static bool FlagBatTay;
  
  // Cờ xử lý LED
  static bool FlagNhayLED_Correct;
  static bool FlagNhayLED_Wrong;
  static bool FlagGiuLedSangLienTuc;
  static uint8_t LEDDapAnDungKhiChonSai;
  
  static uint8_t NutDuocNhan;    // Dùng cho LED

public:
  void KhoiTao(int role);  // Khởi tạo ESP-NOW với role (vai trò master-slave).
  void GuiDuLieuQuaESPNOW(void);  // Gửi đáp án lên slave
  // void KiemTraDungLuongPin(void);

private:
  int _ChanLedBaoDapAnDung = 16;  // Chân LED dùng để nháy khi trả lời đúng
  char _GiaTriNutNhan;

  static void TrangThaiGuiGoiTinCuaNutNhan(uint8_t* mac_addr, uint8_t sendStatus);
  static void NhanDuLieuQuaESPNOW(uint8_t* MAC, uint8_t* DuLieu, uint8_t ChieuDaiDuLieu);
};

#endif
