#include "04_ESPNow.h"

/***** IC Dịch Bit ******/
#define LATCH_pin 2
#define CLOCK_pin 15
#define DATA_pin 5
#define EN_pin 16

const int MangLuuChanNutNhan[] = { 13, 12, 14, 4 };  // {A, B, C, D}
char ESPNOW::MACSlave[13];
uint8_t ESPNOW::MACSlaveDinhDangDung[6];
int32_t ESPNOW::WiFiChannel = 1;
int8_t ESPNOW::channels[] = { 1, 6, 11 };
int ESPNOW::currentChannelIndex = 0;

bool ESPNOW::FlagBatTay = false;

// Flag xử lý LED
bool ESPNOW::FlagNhayLED_Correct = false;
bool ESPNOW::FlagNhayLED_Wrong = false;
bool ESPNOW::FlagGiuLedSangLienTuc = true;
uint8_t ESPNOW::LEDDapAnDungKhiChonSai;  // 0, 1, 2, 3 -> A, B, C, D
uint8_t ESPNOW::NutDuocNhan;    // Dùng cho LED

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
// unsigned long previousMillis_2 = 0;

void ESPNOW::KhoiTao(int role) {
  LIB_74HC595::setup(LATCH_pin, DATA_pin, CLOCK_pin, EN_pin);

  /*-----Khởi tạo nút nhấn là INPUT_PULLUP-----*/
  for (uint8_t i = 0; i < sizeof(MangLuuChanNutNhan) / sizeof(MangLuuChanNutNhan[0]); i++) {
    pinMode(MangLuuChanNutNhan[i], INPUT_PULLUP);
  }

  // Chuyển sang MODE STA (Stations) để dừng phát Access Point.
  WiFi.mode(WIFI_STA);
  delay(100);

  for (int i = 0; i < 6; ++i) {
    sscanf(MACSlave + 2 * i, "%2hhx", &MACSlaveDinhDangDung[i]);
  }

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Khởi tạo ESP-NOW thành công");
  }

  // Khởi tạo ESP-NOW với role (vai trò master-slave)
  esp_now_set_self_role(role);  // COMBO

  esp_now_register_send_cb(ESPNOW::TrangThaiGuiGoiTinCuaNutNhan);
  esp_now_register_recv_cb(ESPNOW::NhanDuLieuQuaESPNOW);

  // time out to allow receiving response from server
  while (ESPNOW::FlagBatTay == false) {
    currentMillis = millis();
    if (currentMillis - previousMillis > 100) {
      previousMillis = currentMillis;

      wifi_promiscuous_enable(1);
      wifi_set_channel(ESPNOW::channels[ESPNOW::currentChannelIndex]);
      // wifi_set_channel(ESPNOW::WiFiChannel);
      wifi_promiscuous_enable(0);

      // Add peer
      if (esp_now_add_peer(MACSlaveDinhDangDung, ESP_NOW_ROLE_SLAVE, ESPNOW::channels[ESPNOW::currentChannelIndex], NULL, 0) != 0) {
        char temp = 0x0D;  // 0x0D là số 13 theo bảng mã ascii
        esp_now_send(MACSlaveDinhDangDung, (uint8_t *)&temp, 1);
      }

      // time out expired,  try next channel
      Serial.print("Channel sau 100ms: ");
      Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);
    }

    // // Sau 10s mà node không liên kết được với slave (kênh rác) thì cho out luôn
    // if (currentMillis - previousMillis_2 > 10000) {
    //   ESPNOW::FlagBatTay = true;
    //   previousMillis_2 = currentMillis;
    // }
  }
}

/*-----Hàm trả về trạng thái gói tin gửi từ Node lên Slave hoặc Master-----*/
void ESPNOW::TrangThaiGuiGoiTinCuaNutNhan(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Trang Thai Cua Goi Tin: ");
  if (sendStatus == 0) {
    ESPNOW::FlagBatTay = true;
    // Serial.println("Thanh Cong !");

    Serial.print("Bat Tay Thanh Cong Voi Channel: ");
    Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);
  } else {
    ESPNOW::FlagBatTay = false;
    Serial.println("That Bai !");

    ESPNOW::currentChannelIndex = (ESPNOW::currentChannelIndex + 1) % (sizeof(ESPNOW::channels) / sizeof(ESPNOW::channels[0]));
  }
}

void ESPNOW::NhanDuLieuQuaESPNOW(uint8_t *MAC, uint8_t *DuLieu, uint8_t ChieuDaiDuLieu) {
  Serial.println(*DuLieu);
  if (*DuLieu == 10) {
    Serial.println("Answer Correct");
    ESPNOW::FlagNhayLED_Correct = true;
  }

  else if (*DuLieu == 11) {
    Serial.println("Answer Wrong");
    ESPNOW::FlagNhayLED_Wrong = true;
  }

  else if (*DuLieu == 12) {
    Serial.println("Cho Phep Sang Den Lien Tuc");
    ESPNOW::FlagGiuLedSangLienTuc = false;
  }

  else if (*DuLieu == 13) {
    Serial.println("Khong Cho Phep Sang Den Lien Tuc");
    ESPNOW::FlagGiuLedSangLienTuc = true;
  }

  else if (*DuLieu == 99) {
    Serial.println("Reset 2 bien FlagNhayLED_Correct va FlagNhayLED_Wrong");
    ESPNOW::FlagNhayLED_Correct = false;
    ESPNOW::FlagNhayLED_Wrong = false;
    LIB_74HC595::TatLED();
    ESPNOW::NutDuocNhan = 0;
    ESPNOW::LEDDapAnDungKhiChonSai = 10; // khacs 0,1,2,3
  }

  // Nhận đáp án đúng để sáng LED Node trả lời sai
  if (*DuLieu == 0) ESPNOW::LEDDapAnDungKhiChonSai = 0;
  else if (*DuLieu == 1) ESPNOW::LEDDapAnDungKhiChonSai = 1;
  else if (*DuLieu == 2) ESPNOW::LEDDapAnDungKhiChonSai = 2;
  else if (*DuLieu == 3) ESPNOW::LEDDapAnDungKhiChonSai = 3;
}

void ESPNOW::GuiDuLieuQuaESPNOW(void) {
  for (int i = 0; i < sizeof(MangLuuChanNutNhan) / sizeof(MangLuuChanNutNhan[0]); i++) {
    // LIB_74HC595::TatLED();
    if (!digitalRead(MangLuuChanNutNhan[i])) {
      delay(20);
      if (!digitalRead(MangLuuChanNutNhan[i])) {
        if (ESPNOW::FlagGiuLedSangLienTuc == false) {
          LIB_74HC595::BatLED(MangLuuChanNutNhan[i], LIB_74HC595::BLUE);
        } else if (ESPNOW::FlagGiuLedSangLienTuc == true) {
          LIB_74HC595::BatLED(MangLuuChanNutNhan[i], LIB_74HC595::BLUE);
          delay(100);
          LIB_74HC595::TatLED();
        }
        ESPNOW::NutDuocNhan = MangLuuChanNutNhan[i];

        _GiaTriNutNhan = 0x30 + i;  // Giá trị nút nhấn (0x30 là "0" cho nút A, 1 cho nút B, ...)
        // }
        esp_now_send(MACSlaveDinhDangDung, (uint8_t *)&_GiaTriNutNhan, 1);

        while (!digitalRead(MangLuuChanNutNhan[i])) {
          delay(50);
        }
      }
    }
  }
}

// void ESPNOW::KiemTraDungLuongPin(void) {
//     820 -> 3,7V
//     1000 -> 4,2V
// }
