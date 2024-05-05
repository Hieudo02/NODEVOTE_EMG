#include "04_ESPNow.h"
#define BatTay

char ESPNOW::MACMaster[13];  // = "58BF254A8E5F"; // = { 0x58, 0xBF, 0x25, 0x4A, 0x6F, 0x70 };
uint8_t ESPNOW::MACMasterDungDinhDang[6];

char ESPNOW::DapAn;  // 0-1-2-3 -> A-B-C-D
uint8_t ESPNOW::MacCuaNodeGuiToiSlave[6];
char ESPNOW::IDTuyChinhCuaSlave = 1;  // Tùy vào Slave mà chỉnh số ở đây
char ESPNOW::IDCuaSlave[3];
char ESPNOW::DuLieuGuiToiMasTer[20];

int32_t ESPNOW::WiFiChannel = 1;
int8_t ESPNOW::channels[] = { 1, 6, 11 };
int ESPNOW::currentChannelIndex = 0;

#ifdef BatTay
bool ESPNOW::FlagBatTay = false;
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;  // Stores last time temperature was published
#endif

void ESPNOW::KhoiTao(int role) {
  // Chuyển sang MODE STA (Stations) để dừng phát Access Point.
  WiFi.mode(WIFI_STA);
  delay(100);

  for (int i = 0; i < 6; ++i) {
    sscanf(MACMaster + 2 * i, "%2hhx", &MACMasterDungDinhDang[i]);
  }

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Khởi tạo ESPNOW Function thành công");
  }

  // Khởi tạo ESP-NOW với role (vai trò master-slave)
  esp_now_set_self_role(role);  // COMBO

#ifdef BatTay
  esp_now_register_send_cb(ESPNOW::TrangThaiGuiGoiTinCuaSlave);
#endif

  // Register function NhanDuLieuQuaESPNOW
  esp_now_register_recv_cb(ESPNOW::DuLieuNhanTuNode);

#ifdef BatTay
  // time out to allow receiving response from server
  while (ESPNOW::FlagBatTay == false) {
    currentMillis = millis();
    if (currentMillis - previousMillis > 100) {
      previousMillis = currentMillis;

      wifi_promiscuous_enable(1);
      // wifi_set_channel(ESPNOW::WiFiChannel);
      wifi_set_channel(ESPNOW::channels[ESPNOW::currentChannelIndex]);
      wifi_promiscuous_enable(0);

      if (esp_now_add_peer(MACMasterDungDinhDang, ESP_NOW_ROLE_SLAVE, ESPNOW::channels[ESPNOW::currentChannelIndex], NULL, 0)) {
        char temp = 0x0D;  // 0x0D là số 13 theo bảng mã ascii
        esp_now_send(MACMasterDungDinhDang, (uint8_t*)&temp, 1);
      }

      // time out expired,  try next channel
      Serial.print("Channel sau 100ms: ");
      Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);
    }
  }
#endif
}

#ifdef BatTay

/*-----Hàm trả về trạng thái gói tin gửi từ Slave lên Master-----*/
void ESPNOW::TrangThaiGuiGoiTinCuaSlave(uint8_t* mac_addr, uint8_t sendStatus) {
  Serial.print("Trang Thai Cua Goi Tin: ");
  if (sendStatus == 0) {
    Serial.println("Thanh Cong !");
    ESPNOW::FlagBatTay = true;
    Serial.print("Bat Tay Thanh Cong Voi Channel: ");
    Serial.println(ESPNOW::channels[ESPNOW::currentChannelIndex]);
  } else {
    ESPNOW::FlagBatTay = false;
    Serial.println("That Bai !");

    ESPNOW::currentChannelIndex = (ESPNOW::currentChannelIndex + 1) % (sizeof(ESPNOW::channels) / sizeof(ESPNOW::channels[0]));


    // ESPNOW::WiFiChannel++;
    // if (ESPNOW::WiFiChannel > MAX_CHANNEL) {
    //   ESPNOW::WiFiChannel = 1;
    // }
  }
}
#endif

void ESPNOW::DuLieuNhanTuNode(uint8_t* MAC, uint8_t* DuLieu, uint8_t ChieuDaiDuLieu) {
  if (*DuLieu != 13) {
    for (int i = 0; i < 6; ++i) {
      MacCuaNodeGuiToiSlave[i] = MAC[i];
    }
    DapAn = *DuLieu;
    sprintf(IDCuaSlave, "%02d", IDTuyChinhCuaSlave);  // Chuyển ID từ dạng 1 sang 01, ...

    snprintf(DuLieuGuiToiMasTer, sizeof(DuLieuGuiToiMasTer),
             "%s/%02x%02x%02x%02x%02x%02x/%c",
             IDCuaSlave, MacCuaNodeGuiToiSlave[0], MacCuaNodeGuiToiSlave[1],
             MacCuaNodeGuiToiSlave[2], MacCuaNodeGuiToiSlave[3],
             MacCuaNodeGuiToiSlave[4], MacCuaNodeGuiToiSlave[5], DapAn);

#ifdef debug
    Serial.print("Dữ liệu gửi tới Master: ");
    Serial.println(DuLieuGuiToiMasTer);
#endif

    esp_now_send(MACMasterDungDinhDang, (uint8_t*)DuLieuGuiToiMasTer, strlen(DuLieuGuiToiMasTer));
  }
}
