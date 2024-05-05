#include <Arduino.h>
#include <EEPROM.h>  // Thư viện để lưu dữ liệu thông số board vào bộ nhớ ROM

#include "00_ChuongTrinhChinh.h"
#include "01_Flags.h"
#include "02_WIFI.h"
#include "03_POSTGET.h"
#include "04_ESPNow.h"
#include "05_ThongSoBoard.h"

#define NutBoot 0
uint8_t _GiuBootDePhatAP = 0;

// #define codePOSTGET
#define defFIFO
char MACNodeTraLoiDung[13];
uint8_t MACNodeTraLoiDungDinhDangDung[6];  // = {0x58, 0xbf, 0x25, 0x49, 0xae, 0xd8};
char correct = 0x0A;                       // 0x0A là số 10 theo bảng mã ascii cho Node biết đáp án là đúng
char wrong = 0x0B;                         // 0x0B là số 11 theo bảng mã ascii cho Node biết đáp án là đúng

char flagGiuLedSang = 0x0C;                   // 0x0C là số 12 theo bảng mã ascii cho Node biết đáp án là đúng
char flagKhongGiuLedSang = 0x0D;              // 0x0D là số 13 theo bảng mã ascii cho Node biết đáp án là đúng
char MACNodeTrongDanhSach[13];                // Dùng cho việc cho phép đèn led sáng liên tục hoặc không
uint8_t MACNodeTrongDanhSachDinhDangDung[6];  // Dùng cho việc cho phép đèn led sáng liên tục hoặc không
bool flagLED = true;

bool flagXacNhanDaGuiDapAnLenServer = false;

char flagReset_code99 = 0x63;

Flags _Flags;                // Cờ định thời gian thực thi cách lệnh.
WIFI _WiFi;                  // Kết nối WiFi cho board.
POSTGET _POSTGET;            // Các hàm thực thi POST - GET giữa device và server.
ESPNOW _ESPNow;              // Kết nối ESP-NOW giữa các ESP với nhau
ThongSoBoard _ThongSoBoard;  // Thông số cài đặt cho board lưu trong EEPROM.

String _ID;  // Số ID của ESP32, đây là số IMEI của board.

void KhoiTao(void) {
  pinMode(_ChanBuzzer, OUTPUT);

#pragma region KhoiTao
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma region Các khởi tạo cơ bản của máy
  //----------------------------------------------------
  // Khởi tạo giao tiếp SERIAL
  Serial.begin(115200);
  Serial.println("");

  // Khởi tạo bộ nhớ ROM của ESP32
  EEPROM.begin(512);
  delay(10);
  //----------------------------------------------------
#pragma endregion Các khởi tạo cơ bản của máy

#pragma region Khởi tạo WIFI
  //======================================================================
  //------ Begin: Khởi tạo để có thể cấu hình kết nối WiFi tự động -----//
  //======================================================================
  // Nếu muốn xóa thông tin WIFI đã lưu trong EEPROM thì mở dòng code này.
  // _WiFi.XoaWiFiDaLuuTrongEEPROM();

  // _ThongSoBoard.XoaEEPROM(0, 512);

  // Đoạn code này phải được gọi ở cuối cùng ở hàm setup().
  _WiFi.DocWiFiDaLuuTrongEEPROM();

  // Dành 10s để kết nối WiFI
  // Lưu ý: Phải có thời gian chờ cho việc kết nối WIFI nếu không sẽ
  // gây ra tình trạng board bị reset và không thể phát access point (AP).
  _WiFi.KetNoiWiFi(10);

  delay(1000);
  //======================================================================
  //------ End: Khởi tạo để có thể cấu hình kết nối WiFi tự động -------//
  //======================================================================
#pragma endregion Khởi tạo WIFI


  // Số ID (ID) của ESP32.
  _ID = _WiFi.LaySoMAC();
  Serial.print("ID: ");
  Serial.println(_ID);

  _ThongSoBoard.KhoiTao();

  _POSTGET.realm = _ThongSoBoard.Realm;
  _POSTGET.ip_host = _ThongSoBoard.IPHost;
  _POSTGET.port = _ThongSoBoard.Port;
  _POSTGET.position = _ThongSoBoard.Position;
  _POSTGET.gid = _WiFi.LaySoMAC();

  _POSTGET.KetNoiVaoWebSocket();

  Serial.print("Kenh Cua Wifi: ");
  Serial.println(WiFi.channel());

  // Khởi tạo ESP-NOW.
  _ESPNow.KhoiTao(ESP_NOW_ROLE_COMBO);

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma endregion KhoiTao
}

void ChayChuongTrinhChinh(void) {
#pragma region ChayChuongTrinhChinh
  if (_GiuBootDePhatAP >= 10) {
    _WiFi.NgatKetNoiWiFi();
  }

  if (_WiFi.DaBatAP == true && _WiFi.DaLuuMACVaoEEPROM == true) {
    _ThongSoBoard.KhoiTao();
    _WiFi.DaLuuMACVaoEEPROM = false;
  }

#pragma region FIFO
#ifdef defFIFO
  // Vòng lặp quét websocket.
  _POSTGET.webSocket.loop();

#pragma region Xử lý FIFO

#pragma region playerJoin
  /*Kiểm tra điều kiện trả về từ server - phản hồi json tương ứng*/
  if (_POSTGET.FlagChoPhepThamGiaGame == true) {
    while (_ESPNow.FIFODuLieu.size() > 0) {
      char MACLuuVaoFIFO[13];
      memcpy(MACLuuVaoFIFO, _ESPNow.FIFODuLieu[0].c_str() + 3, 12);  // Bắt đầu từ vị trí thứ 3, lấy 10 ký tự (MAC address)
      MACLuuVaoFIFO[12] = '\0';                                      // Ký tụ NULL kết thúc chuỗi

      char IndexCuaNode = _ESPNow.FIFODuLieu[0].back();

      _POSTGET.playerJoin(MACLuuVaoFIFO, IndexCuaNode);
      _ESPNow.FIFODuLieu.erase(_ESPNow.FIFODuLieu.begin());

      bool found = std::any_of(
        _POSTGET.DanhSachMACsThamGiaGame.begin(),
        _POSTGET.DanhSachMACsThamGiaGame.end(),
        [MACLuuVaoFIFO](const std::string& s) {
          return s == MACLuuVaoFIFO;
        });

      if (found == false) {
        _POSTGET.DanhSachMACsThamGiaGame.push_back(MACLuuVaoFIFO);
      }
      Serial.printf("%s Đã tham gia vào game\n", _ESPNow.DuLieuNhanDuoc);
    }
  }
#pragma endregion playerJoin

#pragma region Nhận tín hiệu cho phép LED sáng liên tục hoặc không
  if (_POSTGET.FlagLedSangLienTuc == true && flagLED == true) {
    Serial.print("Cho phép đèn led giữ sáng: ");
    for (uint8_t i = 0; i < _POSTGET.DanhSachMACsThamGiaGame.size(); i++) {
      memcpy(MACNodeTrongDanhSach, _POSTGET.DanhSachMACsThamGiaGame[i].c_str(), 13);
      MACNodeTrongDanhSach[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

      for (int j = 0; j < 6; ++j) {
        sscanf(MACNodeTrongDanhSach + 2 * j, "%2hhx", &MACNodeTrongDanhSachDinhDangDung[j]);
      }
      Serial.print(esp_now_send(MACNodeTrongDanhSachDinhDangDung, (uint8_t*)&flagGiuLedSang, 1));
      delay(10);
    }

    flagLED = false;
  } else if (_POSTGET.FlagLedSangLienTuc == false && flagLED == false) {
    Serial.print("Ngưng cho phép đèn led giữ sáng: ");
    for (uint8_t i = 0; i < _POSTGET.DanhSachMACsThamGiaGame.size(); i++) {
      memcpy(MACNodeTrongDanhSach, _POSTGET.DanhSachMACsThamGiaGame[i].c_str(), 13);
      MACNodeTrongDanhSach[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

      for (int j = 0; j < 6; ++j) {
        sscanf(MACNodeTrongDanhSach + 2 * j, "%2hhx", &MACNodeTrongDanhSachDinhDangDung[j]);
      }
      Serial.print(esp_now_send(MACNodeTrongDanhSachDinhDangDung, (uint8_t*)&flagKhongGiuLedSang, 1));
      delay(10);
    }

    flagLED = true;
  }
#pragma endregion Nhận tín hiệu cho phép LED sáng liên tục hoặc không

#pragma region Gửi đáp án lên Server
  if (_POSTGET.cur == showQ) {
    _ESPNow.FIFODuLieu.clear(); // Xóa data spam trước khi câu hỏi xuất hiện

    // Reset Correct and Wrong Flag in Node
    for (uint8_t i = 0; i < _POSTGET.DanhSachMACsThamGiaGame.size(); i++) {
      memcpy(MACNodeTrongDanhSach, _POSTGET.DanhSachMACsThamGiaGame[i].c_str(), 13);
      MACNodeTrongDanhSach[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

      for (int j = 0; j < 6; ++j) {
        sscanf(MACNodeTrongDanhSach + 2 * j, "%2hhx", &MACNodeTrongDanhSachDinhDangDung[j]);
      }
      esp_now_send(MACNodeTrongDanhSachDinhDangDung, (uint8_t*)&flagReset_code99, 1);
      delay(10);
    }

    flagXacNhanDaGuiDapAnLenServer = false;
    _POSTGET.resetAnswer(47);  //  Reset câu trả lời trước đó.
    _POSTGET.cur = ts;
  }

  if (_POSTGET.FlagChoPhepGuiDapAnLenServer == true) {
    if (_ESPNow.FIFODuLieu.size() > 0) {
      char DuLieuDangXuLyTuFIFO[DATA_SIZE];
      char MACDangXuLyTuFIFO[13];
      char DapAnGuiLenServerTuFIFO;

      memcpy(DuLieuDangXuLyTuFIFO, _ESPNow.FIFODuLieu[0].c_str(), sizeof(DuLieuDangXuLyTuFIFO) - 1);
      DuLieuDangXuLyTuFIFO[sizeof(DuLieuDangXuLyTuFIFO) - 1] = '\0';  // Ký tụ NULL kết thúc chuỗi

      strncpy(MACDangXuLyTuFIFO, DuLieuDangXuLyTuFIFO + 3, 12);
      MACDangXuLyTuFIFO[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

      DapAnGuiLenServerTuFIFO = DuLieuDangXuLyTuFIFO[sizeof(DuLieuDangXuLyTuFIFO) - 2];

      bool found = std::any_of(
        _POSTGET.DanhSachMACsThamGiaGame.begin(),
        _POSTGET.DanhSachMACsThamGiaGame.end(),
        [MACDangXuLyTuFIFO](const std::string& s) {
          return s == MACDangXuLyTuFIFO;
        });

      if (found == true) {
        _POSTGET.setAnswer(MACDangXuLyTuFIFO, DapAnGuiLenServerTuFIFO);  // Trả lời đáp án

        flagXacNhanDaGuiDapAnLenServer = true;
        _ESPNow.FIFODuLieu.erase(_ESPNow.FIFODuLieu.begin());
      }
    }
  }
#pragma endregion Gửi đáp án lên Server

#pragma region Nhận Correct và Wrong Answer từ Server gửi về
  if (_POSTGET.cur == ansC && (_POSTGET.prev == cl || _POSTGET.prev == ansC || _POSTGET.prev == ansW) && flagXacNhanDaGuiDapAnLenServer == true) {
    for (int i = 0; i < _POSTGET.FIFONhanDapAn.size(); i++) {
      memcpy(MACNodeTraLoiDung, _POSTGET.FIFONhanDapAn[i].c_str(), 13);
      MACNodeTraLoiDung[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

      for (int j = 0; j < 6; ++j) {
        sscanf(MACNodeTraLoiDung + 2 * j, "%2hhx", &MACNodeTraLoiDungDinhDangDung[j]);
      }
      esp_now_send(MACNodeTraLoiDungDinhDangDung, (uint8_t*)&correct, 1);
      Serial.print(esp_now_send(MACNodeTraLoiDungDinhDangDung, (uint8_t*)&_POSTGET.DapAnDungGuiTuServer, 1));
      delay(10);
    }

    // flagXacNhanDaGuiDapAnLenServer = false;
    // _POSTGET.FIFONhanDapAn.clear();
  } else if (_POSTGET.cur == ansW && (_POSTGET.prev == cl || _POSTGET.prev == ansC || _POSTGET.prev == ansW) && flagXacNhanDaGuiDapAnLenServer == true) {
    for (int i = 0; i < _POSTGET.FIFONhanDapAn.size(); i++) {
      memcpy(MACNodeTraLoiDung, _POSTGET.FIFONhanDapAn[i].c_str(), 13);
      MACNodeTraLoiDung[12] = '\0';  // Ký tụ NULL kết thúc chuỗi
      for (int j = 0; j < 6; ++j) {
        sscanf(MACNodeTraLoiDung + 2 * j, "%2hhx", &MACNodeTraLoiDungDinhDangDung[j]);
      }

      esp_now_send(MACNodeTraLoiDungDinhDangDung, (uint8_t*)&_POSTGET.DapAnDungGuiTuServer, 1);
      // Serial.print("Dap An Dung: ");
      // Serial.println(_POSTGET.DapAnDungGuiTuServer);

      Serial.print(esp_now_send(MACNodeTraLoiDungDinhDangDung, (uint8_t*)&wrong, 1));
      delay(10);
    }

    // flagXacNhanDaGuiDapAnLenServer = false;
  }
  _POSTGET.FIFONhanDapAn.clear();
#pragma endregion Nhận Correct và Wrong Answer từ Server gửi về

#pragma endregion Xử lý FIFO

#endif
#pragma endregion FIFO

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
//================================================w===================================
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
#pragma region Flag500ms
#ifdef _Flag_500ms
  if (_Flags.Flag.t500ms) {
    _WiFi.KiemTraKetNoiWiFi();
  }
#endif
#pragma endregion Flag500ms
//------------------------------------------------------------------------------
#pragma region Flag1s
#ifdef _Flag_1s
  if (_Flags.Flag.t1s) {
    Serial.print("Dữ liệu từ slaver gửi tới master (Chương trình chính): ");
    Serial.println(_ESPNow.DuLieuNhanDuoc);
  }
#endif
#pragma endregion Flag500ms
  //------------------------------------------------------------------------------

#pragma endregion ThucThiTacVuTheoFLAG
}
#pragma endregion CÁC HÀM THỰC THI TÁC VỤ THEO FLAG
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============================ End: CÁC HÀM THỰC THI TÁC VỤ THEO FLAG ===============================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM