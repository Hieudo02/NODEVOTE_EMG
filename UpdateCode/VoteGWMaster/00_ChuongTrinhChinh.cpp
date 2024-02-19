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
char temp = 0x0A;                          // 0x0A là số 10 theo bảng mã ascii cho Node biết đáp án là đúng

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
    // Liên tục kiểm tra dữ liệu từ các node gửi lên.
    if (_ESPNow.DuLieuNhanDuoc[0] != '\0') {  // Nếu ký tự đầu tiên khác NULL
      char MACLuuVaoFIFO[13];
      Serial.println(_ESPNow.DuLieuNhanDuoc);
      strcpy(MACLuuVaoFIFO, _ESPNow.MAC);

      // 21-11
      char IndexCuaNode;
      IndexCuaNode = _ESPNow.DapAn;

      bool found = std::any_of(
        _POSTGET.DanhSachMACsThamGiaGame.begin(),
        _POSTGET.DanhSachMACsThamGiaGame.end(),
        [MACLuuVaoFIFO](const std::string& s) {
          return s == MACLuuVaoFIFO;
        });

      // Kiểm tra xem MACLuuVaoFIFO có tồn tại trong mảng lớn hay không
      _POSTGET.playerJoin(MACLuuVaoFIFO, IndexCuaNode);
      if (found == false) {
        // _POSTGET.playerJoin(MACLuuVaoFIFO, IndexCuaNode);
        // Thêm MACLuuVaoFIFO vào mảng lớn
        _POSTGET.DanhSachMACsThamGiaGame.push_back(MACLuuVaoFIFO);
#ifdef debug
        Serial.printf("%s Đã tham gia vào game\n", _ESPNow.DuLieuNhanDuoc);
#endif
      }

      // Clear array char.
      _ESPNow.DuLieuNhanDuoc[0] = '\0';
    }
  }
#pragma endregion playerJoin

#pragma region playGame
  // Chỉ cho phép Node tồn tại trong Vector trả lời
  if (_POSTGET.FlagChoPhepGuiDapAnLenServer == true) {
    // Liên tục kiểm tra dữ liệu từ các node gửi lên và lưu vào FIFO
    if (_ESPNow.DuLieuNhanDuoc[0] != '\0') {           // Nếu ký tự đầu tiên khác NULL
      char DuLieuNhanDuocTuNodeLuuVaoFIFO[DATA_SIZE];  // Mảng ký tự nhận được từ Slave hoặc Node
      strcpy(DuLieuNhanDuocTuNodeLuuVaoFIFO, _ESPNow.DuLieuNhanDuoc);
      bool found = std::any_of(
        _POSTGET.FIFODuLieu.begin(),
        _POSTGET.FIFODuLieu.end(),
        [DuLieuNhanDuocTuNodeLuuVaoFIFO](const std::string& s) {
          return s == DuLieuNhanDuocTuNodeLuuVaoFIFO;
        });

      // Kiểm tra xem DuLieuNhanDuocTuNodeLuuVaoFIFO có tồn tại trong mảng lớn hay không
      if (found == false) {
        // Thêm DuLieuNhanDuocTuNodeLuuVaoFIFO vào mảng lớn
        _POSTGET.FIFODuLieu.push_back(DuLieuNhanDuocTuNodeLuuVaoFIFO);
#ifdef debug
        Serial.printf("%s Đã gửi đáp án\n", _ESPNow.DuLieuNhanDuoc);
#endif
      }

      // Clear array char.
      _ESPNow.DuLieuNhanDuoc[0] = '\0';
    }
  }
#pragma endregion playGame

#pragma region Gửi đáp án lên Server
  if(_POSTGET.prev == ts && (_POSTGET.cur == showQ || _POSTGET.cur == fn)) {
    _POSTGET.resetAnswer(47);  //  Reset câu trả lời trước đó.
    _POSTGET.cur = ts;
  }

  if (_POSTGET.FIFODuLieu.size() > 0) {
    char DapAnGuiLenServerTuFIFO;
    char MACDangXuLyTuFIFO[13];
    char DuLieuDangXuLyTuFIFO[DATA_SIZE];

    memcpy(DuLieuDangXuLyTuFIFO, _POSTGET.FIFODuLieu[0].c_str(), sizeof(DuLieuDangXuLyTuFIFO) - 1);
    DuLieuDangXuLyTuFIFO[sizeof(DuLieuDangXuLyTuFIFO) - 1] = '\0';  // Ký tụ NULL kết thúc chuỗi

    strncpy(MACDangXuLyTuFIFO, DuLieuDangXuLyTuFIFO + 3, 12);
    MACDangXuLyTuFIFO[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

    DapAnGuiLenServerTuFIFO = DuLieuDangXuLyTuFIFO[sizeof(DuLieuDangXuLyTuFIFO) - 2];

    _POSTGET.FIFODuLieu.erase(_POSTGET.FIFODuLieu.begin());

    bool found = std::any_of(
      _POSTGET.DanhSachMACsThamGiaGame.begin(),
      _POSTGET.DanhSachMACsThamGiaGame.end(),
      [MACDangXuLyTuFIFO](const std::string& s) {
        return s == MACDangXuLyTuFIFO;
      });

    if (found == true) {
      // _POSTGET.setAnswer(MACDangXuLyTuFIFO, 47);                       //  Reset câu trả lời trước đó.
      _POSTGET.setAnswer(MACDangXuLyTuFIFO, DapAnGuiLenServerTuFIFO);  // Trả lời đáp án
    }
  }
#pragma endregion Gửi đáp án lên Server

#pragma region Nhận Correct Answer từ Server gửi về
  if (_POSTGET.prev == ansC && (_POSTGET.cur == ts || _POSTGET.cur == fn)) {
    for (int i = 0; i < _POSTGET.FIFODapAnDung.size(); i++) {
      memcpy(MACNodeTraLoiDung, _POSTGET.FIFODapAnDung[i].c_str(), 13);
      MACNodeTraLoiDung[12] = '\0';  // Ký tụ NULL kết thúc chuỗi

      for (int j = 0; j < 6; ++j) {
        sscanf(MACNodeTraLoiDung + 2 * j, "%2hhx", &MACNodeTraLoiDungDinhDangDung[j]);
      }
      Serial.print(esp_now_send(MACNodeTraLoiDungDinhDangDung, (uint8_t*)&temp, 1));
      delay(10);
    }

    _POSTGET.FIFODapAnDung.clear();
  }
#pragma endregion Nhận Correct Answer từ Server gửi về

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