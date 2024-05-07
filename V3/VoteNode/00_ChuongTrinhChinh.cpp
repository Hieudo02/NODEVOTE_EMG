#include <Arduino.h>
#include <EEPROM.h>  // Thư viện để lưu dữ liệu thông số board vào bộ nhớ ROM

#include "00_ChuongTrinhChinh.h"
#include "01_Flags.h"
#include "02_WIFI.h"
#include "04_ESPNow.h"
#include "05_ThongSoBoard.h"

#define NutBoot 0
#define _ChanDocPin 17

Flags _Flags;                // Cờ định thời gian thực thi cách lệnh.
WIFI _WiFi;                  // Kết nối WiFi cho board.
ESPNOW _ESPNow;              // Kết nối ESP-NOW giữa các ESP với nhau
ThongSoBoard _ThongSoBoard;  // Thông số cài đặt cho board lưu trong EEPROM.

String _ID;  // Số ID của ESP32, đây là số IMEI của board.
uint8_t _GiuBootDePhatAP = 0;
bool _FlagThietLapAP = false;

void KhoiTao(void) {
#pragma region KhoiTao
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  // Xoa EEPROM
  // _ThongSoBoard.XoaEEPROM(0, 512);

  pinMode(NutBoot, INPUT_PULLUP);

  //----------------------------------------------------
  // Khởi tạo giao tiếp SERIAL
  Serial.begin(115200);
  Serial.println("");

  // Khởi tạo bộ nhớ ROM của ESP32
  EEPROM.begin(512);
  delay(10);
  //----------------------------------------------------


  // Số ID (ID) của ESP32.
  _ID = _WiFi.LaySoMAC();
  Serial.print("ID: ");
  Serial.println(_ID);

  _ThongSoBoard.KhoiTao();
  strncpy(_ESPNow.MACSlave, _ThongSoBoard.MACSlave, sizeof(_ESPNow.MACSlave));
#ifdef debug
  Serial.print("_ESPNow.MACSlave: ");
  Serial.println(_ESPNow.MACSlave);
#endif

  // Khởi tạo ESP-NOW.
  _ESPNow.KhoiTao(ESP_NOW_ROLE_COMBO);

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma endregion KhoiTao
}

void ChayChuongTrinhChinh(void) {
#pragma region ChayChuongTrinhChinh
  /*-----Gửi dữ liệu nút nhấn qua Slave hoặc Master*/
  _ESPNow.GuiDuLieuQuaESPNOW();
  // _ESPNow.NhayLEDPinYeu(_ChanDocPin);

  // Nháy LED khi đáp án đúng
  if (_ESPNow.FlagNhayLED_Correct == true && _ESPNow.NutDuocNhan > 0) {
    _ESPNow.NhayLEDGreenTheoNutNhan(_ESPNow.NutDuocNhan);
    // _ESPNow.NhayLEDGreen(_ESPNow.LEDDapAnDungKhiChonSai);

    // if (_ESPNow.LEDDapAnDungKhiChonSai == 0) _ESPNow.NhayLEDGreen(0);
    // else if (_ESPNow.LEDDapAnDungKhiChonSai == 1) _ESPNow.NhayLEDGreen(1);
    // else if (_ESPNow.LEDDapAnDungKhiChonSai == 2) _ESPNow.NhayLEDGreen(2);
    // else if (_ESPNow.LEDDapAnDungKhiChonSai == 3) _ESPNow.NhayLEDGreen(3);

    _ESPNow.FlagNhayLED_Correct = false;
    _ESPNow.NutDuocNhan = 0;
  } else if (_ESPNow.FlagNhayLED_Wrong == true && _ESPNow.NutDuocNhan > 0) {
    _ESPNow.NhayLEDRed(_ESPNow.NutDuocNhan, _ESPNow.LEDDapAnDungKhiChonSai);

    // if (_ESPNow.LEDDapAnDungKhiChonSai == 0) _ESPNow.NhayLEDRed(_ESPNow.NutDuocNhan, 0);
    // else if (_ESPNow.LEDDapAnDungKhiChonSai == 1) _ESPNow.NhayLEDRed(_ESPNow.NutDuocNhan, 1);
    // else if (_ESPNow.LEDDapAnDungKhiChonSai == 2) _ESPNow.NhayLEDRed(_ESPNow.NutDuocNhan, 2);
    // else if (_ESPNow.LEDDapAnDungKhiChonSai == 3) _ESPNow.NhayLEDRed(_ESPNow.NutDuocNhan, 3);

    _ESPNow.FlagNhayLED_Wrong = false;
    _ESPNow.NutDuocNhan = 0;
    _ESPNow.LEDDapAnDungKhiChonSai = 10; // khacs 0,1,2,3
  }

  if (_GiuBootDePhatAP >= 10 && _FlagThietLapAP == false) {
    _GiuBootDePhatAP = 0;
    _FlagThietLapAP = true;
  }

  if (_FlagThietLapAP == true) {
    _WiFi.ThietLapAP();
    if (_WiFi.DaBatAP == false) {
      _FlagThietLapAP = _WiFi.DaBatAP;

      // Khởi tạo ESP-NOW.
      _ESPNow.KhoiTao(ESP_NOW_ROLE_COMBO);
    }

    if (_WiFi.DaLuuMACVaoEEPROM == true) {
      _ThongSoBoard.KhoiTao();
      strncpy(_ESPNow.MACSlave, _ThongSoBoard.MACSlave, sizeof(_ESPNow.MACSlave));
      _WiFi.DaLuuMACVaoEEPROM = false;
#ifdef debug
      Serial.print("_ESPNow.MACSlave: ");
      Serial.println(_ESPNow.MACSlave);
#endif
    }
  }



  //===================================================================================
  //--------------- Begin: THỰC THI CHƯƠNG TRÌNH CHÍNH ------------------------------//
  //===================================================================================
  // Bật các cờ lấy mốc thời gian thực hiện các tác vụ.
  // Luôn luôn gọi ở đầu vòng loop().
  _Flags.TurnONFlags();

  ThucThiTacVuTheoFLAG();

  // Tắt các cờ lấy mốc thời gian thực hiện các tác vụ.
  // Luôn luôn gọi ở cuối vòng loop().
  _Flags.TurnOFFFlags();
//===================================================================================
//--------------- End: THỰC THI CHƯƠNG TRÌNH CHÍNH --------------------------------//
//===================================================================================
#pragma endregion ChayChuongTrinhChinh
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============================ Begin: CÁC HÀM THỰC THI TÁC VỤ THEO FLAG =============================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma region CÁC HÀM THỰC THI TÁC VỤ THEO FLAG
void ThucThiTacVuTheoFLAG(void) {
#pragma region ThucThiTacVuTheoFLAG
//------------------------------------------------------------------------------
#pragma region Flag100ms
#ifdef _Flag_100ms
  if (_Flags.Flag.t100ms) {
    // _WiFi.KiemTraKetNoiWiFi();
    if (digitalRead(NutBoot) == LOW) {
      _GiuBootDePhatAP++;
      Serial.print("NHAN GIU LAN THU: ");
      Serial.println(_GiuBootDePhatAP);
    }
  }
#endif
#pragma endregion Flag100ms
//------------------------------------------------------------------------------
#pragma region Flag250ms
#ifdef _Flag_250ms
  if (_Flags.Flag.t250ms) {
    if (_ESPNow.FlagBatTay == true) {

      char temp = 0x0D;  // 0x0D là số 13 theo bảng mã ascii
      esp_now_send(_ESPNow.MACSlaveDinhDangDung, (uint8_t*)&temp, 1);
    } else {

      wifi_promiscuous_enable(1);
      wifi_set_channel(ESPNOW::channels[ESPNOW::currentChannelIndex]);
      wifi_promiscuous_enable(0);

      if (esp_now_add_peer(_ESPNow.MACSlaveDinhDangDung, ESP_NOW_ROLE_SLAVE, ESPNOW::channels[ESPNOW::currentChannelIndex], NULL, 0)) {
        char temp = 0x0D;  // 0x0D là số 13 theo bảng mã ascii
        esp_now_send(_ESPNow.MACSlaveDinhDangDung, (uint8_t*)&temp, 1);
      }

      Serial.print("Channel sau 250ms: ");
      Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);
    }
  }
#endif
#pragma endregion Flag250ms
//------------------------------------------------------------------------------
#pragma region Flag1s
#ifdef _Flag_1s
  if (_Flags.Flag.t1s) {
    // Báo buzzer khi pin yếu
    _ESPNow.KiemTraDungLuongPin();
  }
#endif
#pragma endregion Flag1s
//------------------------------------------------------------------------------
#pragma endregion ThucThiTacVuTheoFLAG
}
#pragma endregion CÁC HÀM THỰC THI TÁC VỤ THEO FLAG
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============================ End: CÁC HÀM THỰC THI TÁC VỤ THEO FLAG ===============================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM