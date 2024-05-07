#include <Arduino.h>
#include <EEPROM.h>  // Thư viện để lưu dữ liệu thông số board vào bộ nhớ ROM

#include "00_ChuongTrinhChinh.h"
#include "01_Flags.h"
#include "02_WIFI.h"
#include "04_ESPNow.h"
#include "05_ThongSoBoard.h"

#define codeNodeToSlave
#define NutBoot 0

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
  strncpy(_ESPNow.MACMaster, _ThongSoBoard.MACMaster, sizeof(_ESPNow.MACMaster));

#ifdef debug
  Serial.print("_ESPNow.MACMaster: ");
  Serial.println(_ESPNow.MACMaster);
#endif

  // Khởi tạo ESP-NOW.
  _ESPNow.KhoiTao(ESP_NOW_ROLE_COMBO);

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma endregion KhoiTao
}

void ChayChuongTrinhChinh(void) {
#pragma region ChayChuongTrinhChinh

#pragma region sendDataToMaster
  if (_ESPNow.FIFODataCuaNodeGuiLenMaster.size() > 0) {
    char DuLieuDangXuLyTuFIFO[DATA_SIZE];
    std::strcpy(DuLieuDangXuLyTuFIFO, _ESPNow.FIFODataCuaNodeGuiLenMaster[0].c_str());
    DuLieuDangXuLyTuFIFO[sizeof(DuLieuDangXuLyTuFIFO) - 1] = '\0';  // Ký tụ NULL kết thúc chuỗi

    esp_now_send(_ESPNow.MACMasterDungDinhDang, (uint8_t*)DuLieuDangXuLyTuFIFO, strlen(DuLieuDangXuLyTuFIFO));

    _ESPNow.FIFODataCuaNodeGuiLenMaster.erase(_ESPNow.FIFODataCuaNodeGuiLenMaster.begin());

  }
#pragma endregion sendDataToMaster

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
      strncpy(_ESPNow.MACMaster, _ThongSoBoard.MACMaster, sizeof(_ESPNow.MACMaster));
      _WiFi.DaLuuMACVaoEEPROM = false;
#ifdef debug
      Serial.print("_ESPNow.MACMaster: ");
      Serial.println(_ESPNow.MACMaster);
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
#pragma region Flag750ms
#ifdef _Flag_750ms
  if (_Flags.Flag.t750ms) {
    if (_ESPNow.FlagBatTay == true) {
      char temp = 0x0D;  // 0x0D là số 13 theo bảng mã ascii
      esp_now_send(_ESPNow.MACMasterDungDinhDang, (uint8_t*)&temp, 1);
    } else {
      wifi_promiscuous_enable(1);
      wifi_set_channel(_ESPNow.channels[_ESPNow.currentChannelIndex]);
      wifi_promiscuous_enable(0);

      if (esp_now_add_peer(_ESPNow.MACMasterDungDinhDang, ESP_NOW_ROLE_SLAVE, _ESPNow.channels[_ESPNow.currentChannelIndex], NULL, 0)) {
        char temp = 0x0D;  // 0x0D là số 13 theo bảng mã ascii
        esp_now_send(_ESPNow.MACMasterDungDinhDang, (uint8_t*)&temp, 1);
      }

      // time out expired,  try next channel
      Serial.print("Channel sau 750ms: ");
      Serial.println(_ESPNow.channels[_ESPNow.currentChannelIndex]);
      // _ESPNow.WiFiChannel++;
      // if (_ESPNow.WiFiChannel > MAX_CHANNEL) {
      //   _ESPNow.WiFiChannel = 1;
      // }
    }
  }
#endif
#pragma endregion Flag750ms
//------------------------------------------------------------------------------
#pragma endregion ThucThiTacVuTheoFLAG
#pragma endregion CÁC HÀM THỰC THI TÁC VỤ THEO FLAG
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============================ End: CÁC HÀM THỰC THI TÁC VỤ THEO FLAG ===============================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}