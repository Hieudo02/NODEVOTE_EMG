#include "03_POSTGET.h"
#include <ArduinoJson.h>

// wss://virtualclass-4.emg.edu.vn:3030/ws?realm=emgverse&position=-10,114&gid=0x123456789

String POSTGET::realm;
String POSTGET::ip_host;
String POSTGET::port;
String POSTGET::position;
String POSTGET::gid;

int POSTGET::prev = 0;
int POSTGET::cur = 0;

char POSTGET::DuLieuServerGuiVe[20];
char POSTGET::IDCuaDapAnNhanTuServer[20];

int POSTGET::DapAnDungGuiTuServer;

bool POSTGET::FlagChoPhepThamGiaGame = false;
bool POSTGET::FlagChoPhepGuiDapAnLenServer = false;
bool POSTGET::FlagLedSangLienTuc = false;
int POSTGET::count = 0;

std::vector<std::string> POSTGET::FIFONhanDapAn;

String url = "";

void POSTGET::KetNoiVaoWebSocket() {
  const String url = "/ws?realm=" + POSTGET::realm + "&position=" + POSTGET::position + "&gid=" + POSTGET::gid;

  webSocket.beginSSL(POSTGET::ip_host.c_str(), atoi(POSTGET::port.c_str()), url.c_str(), NULL, "arduino");
  // webSocket.beginSSL(ip_host, port, url, NULL, "arduino");
  webSocket.onEvent(WEBSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void POSTGET::WEBSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    // case WStype_DISCONNECTED:
    //   Serial.printf("[WSc] Disconnected!\n");
    //   break;
    // case WStype_CONNECTED:
    //   Serial.printf("[WSc] Connected to url: %s\n", payload);
    //   break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      StaticJsonDocument<60> filter;
      filter["type"] = true;
      filter["data"]["id"] = true;
      filter["data"]["answer"] = true;

      StaticJsonDocument<200> doc;

      DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

      if (!error) {
        strncpy(POSTGET::DuLieuServerGuiVe, doc["type"], sizeof(POSTGET::DuLieuServerGuiVe));

        if (strcmp(POSTGET::DuLieuServerGuiVe, "gameState") == 0 || strcmp(POSTGET::DuLieuServerGuiVe, "displayPaused") == 0 || strcmp(POSTGET::DuLieuServerGuiVe, "roomPlayer") == 0) {
          POSTGET::FlagChoPhepThamGiaGame = true;
          // Serial.print("So Nguoi Da Tham Gia: ");
          // Serial.println(POSTGET::count);
          POSTGET::count++;
        } else if (strcmp(POSTGET::DuLieuServerGuiVe, "showQuestion") == 0) {
          POSTGET::prev = cur;
          POSTGET::cur = showQ;
          POSTGET::FlagChoPhepGuiDapAnLenServer = true;
          POSTGET::FlagChoPhepThamGiaGame = false;

          // new Bấm nút led giữ sáng liên tục
          POSTGET::FlagLedSangLienTuc = true;
        } else if (strcmp(POSTGET::DuLieuServerGuiVe, "clearBoard") == 0) {
          POSTGET::prev = cur;
          POSTGET::cur = cl;
          POSTGET::FlagChoPhepGuiDapAnLenServer = false;
        } else if (strcmp(POSTGET::DuLieuServerGuiVe, "answerCorrect") == 0) {
          POSTGET::prev = cur;
          POSTGET::cur = ansC;
          strncpy(POSTGET::IDCuaDapAnNhanTuServer, doc["data"]["id"], sizeof(POSTGET::IDCuaDapAnNhanTuServer));
          POSTGET::DapAnDungGuiTuServer = doc["data"]["answer"].as<int>();
          
          POSTGET::FIFONhanDapAn.push_back(POSTGET::IDCuaDapAnNhanTuServer);
        } else if (strcmp(POSTGET::DuLieuServerGuiVe, "answerWrong") == 0) {
          POSTGET::prev = cur;
          POSTGET::cur = ansW;

          strncpy(POSTGET::IDCuaDapAnNhanTuServer, doc["data"]["id"], sizeof(POSTGET::IDCuaDapAnNhanTuServer));
          POSTGET::DapAnDungGuiTuServer = doc["data"]["answer"].as<int>();

          POSTGET::FIFONhanDapAn.push_back(POSTGET::IDCuaDapAnNhanTuServer);

        } else if (strcmp(POSTGET::DuLieuServerGuiVe, "finished") == 0) {
          POSTGET::prev = cur;
          POSTGET::cur = fn;

          POSTGET::FlagChoPhepThamGiaGame = true; // Kết thúc game bấm để vô lại chơi tiếp
          POSTGET::FlagChoPhepGuiDapAnLenServer = false;

          // new Bấm nút không cho led giữ sáng nữa
          POSTGET::FlagLedSangLienTuc = false;
        } else if (strcmp(POSTGET::DuLieuServerGuiVe, "timerTick") == 0) {
          POSTGET::prev = cur;
          POSTGET::cur = ts;
        }

      } else {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
  }
}

StaticJsonDocument<192> jsonDoc;
String jsonString;

// {
//     "type": "playerJoin",
//     "realm": "emgverse",
//     "position": "-10,113",
//     "data": {
//         "id": "0x12345678901234567890123456789012",
//         "name": "John#12345"
//     }
// }

void POSTGET::playerJoin(char playerMAC[], char IndexButton) {
  jsonDoc.clear();  // Clear the JSON document for reuse

  jsonDoc["type"] = "playerJoin";
  jsonDoc["realm"] = realm;
  jsonDoc["position"] = position;

  JsonObject data = jsonDoc.createNestedObject("data");
  data["id"] = playerMAC;
  data["name"] = playerMAC;
  data["choice"] = IndexButton - 48;

  // Serialize JSON document to string
  jsonString = "";
  serializeJson(jsonDoc, jsonString);

  webSocket.sendTXT(jsonString);
  Serial.println(jsonString);
}

void POSTGET::setAnswer(char playerMAC[], char DapAn) {
  jsonDoc.clear();  // Clear the JSON document for reuse

  jsonDoc["type"] = "playerSetAnswer";
  jsonDoc["realm"] = realm;
  jsonDoc["position"] = position;

  JsonObject data = jsonDoc.createNestedObject("data");
  data["id"] = playerMAC;
  data["choice"] = DapAn - 48;

  // Serialize JSON document to string
  jsonString = "";
  serializeJson(jsonDoc, jsonString);

  webSocket.sendTXT(jsonString);
  Serial.println(jsonString);
}

void POSTGET::resetAnswer(char DapAn) {
  jsonDoc.clear();  // Clear the JSON document for reuse

  jsonDoc["type"] = "playerSetAnswer";
  jsonDoc["realm"] = realm;
  jsonDoc["position"] = position;

  JsonObject data = jsonDoc.createNestedObject("data");
  data["choice"] = DapAn - 48;

  // Serialize JSON document to string
  jsonString = "";
  serializeJson(jsonDoc, jsonString);

  webSocket.sendTXT(jsonString);
  Serial.println(jsonString);
}