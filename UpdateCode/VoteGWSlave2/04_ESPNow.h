#ifndef _ESPNow_h
#define _ESPNow_h

#include "VoteGWSlave2.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>

#define MAX_CHANNEL 14  // for North America // 13 in Europe

class ESPNOW {
public:
  static int32_t WiFiChannel;
  static int8_t channels[];
  static int currentChannelIndex;
  
  static char MACMaster[13];
  static uint8_t MACMasterDungDinhDang[6];
  static char DapAn;                      // Đáp án 0-1-2-3 -> A-B-C-D
  static char IDTuyChinhCuaSlave;
  static uint8_t MacCuaNodeGuiToiSlave[6];
  static char IDCuaSlave[3];
  static char DuLieuGuiToiMasTer[20];
  static bool FlagBatTay;

public:
  void KhoiTao(int role);  // Khởi tạo ESP-NOW với role (vai trò master-slave).

private:
  static void TrangThaiGuiGoiTinCuaSlave(uint8_t *mac_addr, uint8_t sendStatus);
  static void DuLieuNhanTuNode(uint8_t* MAC, uint8_t* DuLieu, uint8_t ChieuDaiDuLieu);
};

#endif
