#include "04_ESPNow.h"

#define _ChanBuzzer 5

const int MangLuuChanNutNhan[] = { 13, 12, 14, 4 };  // {A, B, C, D}
char ESPNOW::MACSlave[13];
uint8_t ESPNOW::MACSlaveDinhDangDung[6];
int32_t ESPNOW::WiFiChannel = 1;
int8_t ESPNOW::channels[] = { 1, 6, 11 };
int ESPNOW::currentChannelIndex = 0;

bool ESPNOW::FlagBatTay = false;
bool ESPNOW::FlagNhayLED = false;
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;  // Stores last time temperature was published

void ESPNOW::KhoiTao(int role) {
  /*-----Khởi tạo nút nhấn là INPUT_PULLUP-----*/
  for (uint8_t i = 0; i < sizeof(MangLuuChanNutNhan) / sizeof(MangLuuChanNutNhan[0]); i++) {
    pinMode(MangLuuChanNutNhan[i], INPUT_PULLUP);
  }

  /*-----Khởi tạo Buzzer-----*/
  pinMode(_ChanBuzzer, OUTPUT);
  pinMode(LEDBat, OUTPUT);

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
      digitalWrite(LEDBat, 0);
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

      // ESPNOW::WiFiChannel++;
      // if (ESPNOW::WiFiChannel > MAX_CHANNEL) {
      //   ESPNOW::WiFiChannel = 1;
      // }

      // time out expired,  try next channel
      Serial.print("Channel sau 100ms: ");
      Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);
    }
  }
}

/*-----Hàm trả về trạng thái gói tin gửi từ Node lên Slave hoặc Master-----*/
void ESPNOW::TrangThaiGuiGoiTinCuaNutNhan(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Trang Thai Cua Goi Tin: ");
  if (sendStatus == 0) {
    ESPNOW::FlagBatTay = true;
    Serial.println("Thanh Cong !");

    Serial.print("Bat Tay Thanh Cong Voi Channel: ");
    Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);

    // digitalWrite(LEDBat, 1);

  } else {
    ESPNOW::FlagBatTay = false;
    Serial.println("That Bai !");

    ESPNOW::currentChannelIndex = (ESPNOW::currentChannelIndex + 1) % (sizeof(ESPNOW::channels) / sizeof(ESPNOW::channels[0]));
  }
}

void ESPNOW::NhanDuLieuQuaESPNOW(uint8_t *MAC, uint8_t *DuLieu, uint8_t ChieuDaiDuLieu) {
  if (*DuLieu == 10) {
    Serial.println("Nháy đi làm ơn");
    FlagNhayLED = true;
  }
}

void ESPNOW::GuiDuLieuQuaESPNOW(void) {
  for (int i = 0; i < sizeof(MangLuuChanNutNhan) / sizeof(MangLuuChanNutNhan[0]); i++) {
    if (!digitalRead(MangLuuChanNutNhan[i])) {
      delay(20);
      if (!digitalRead(MangLuuChanNutNhan[i])) {
        // Khi nút nhấn i được nhấn, còi sẽ kêu
        digitalWrite(_ChanBuzzer, HIGH);
        delay(50);
        digitalWrite(_ChanBuzzer, LOW);
        delay(50);

        _GiaTriNutNhan = 0x30 + i;  // Giá trị nút nhấn (0x30 là "0" cho nút A, 1 cho nút B, ...)

        // Gửi dữ liệu giá trị nút nhấn
        esp_now_send(MACSlaveDinhDangDung, (uint8_t *)&_GiaTriNutNhan, 1);

        while (!digitalRead(MangLuuChanNutNhan[i])) {
          delay(50);  // Trường hợp nhấn giữ nút
        }
      }
    }
  }
}

void ESPNOW::BuzzerKhiPinYeu(void) {
  digitalWrite(_ChanBuzzer, PhatBuzzer);
  PhatBuzzer = !PhatBuzzer;
}

void ESPNOW::KiemTraDungLuongPin(void) {
  if (analogRead(_ChanDocPin) < 500)
    FlagPinYeu = true;
  else FlagPinYeu = false;
  // Serial.print("Gia Tri ADC: ");
  // Serial.println(analogRead(ChanDocPin));
  // if (analogRead(_ChanDocPin) < 960) {
  //   digitalWrite(_ChanBuzzer, 1);
  //   delay(50);
  //   digitalWrite(_ChanBuzzer, 0);
  //   delay(50);
  // }
  /*
    820 -> 3,7V
    1000 -> 4,2V
  */
}

void ESPNOW::NhayLED(void) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(LEDBat, 1);
    digitalWrite(_ChanBuzzer, 1);
    delay(100);
    digitalWrite(LEDBat, 0);
    digitalWrite(_ChanBuzzer, 0);
    delay(100);
  }
}
