#include "02_WiFi.h"      //
#include <EEPROM.h>       // Thư viện để lưu dữ liệu thông tin WiFi vào bộ nhớ ROM.
#include <ESP8266WiFi.h>// Thư viện WiFi của ESP8266.
#include <ESP8266WebServer.h> //
#include "WEBConfig.h"    // Giao diện HTML trang cấu hình WiFi cho board.

//Phần bắt đầu của WiFi ConFig SSID, để phân biệt các board khác dự án.
//----------------------------------------------------------------------
#define _TenDuAn "EMGS2@"
#define EEPROM_SSID 415
#define EEPROM_PASS 447

ESP8266WebServer server(80);
IPAddress ip_ap(192, 168, 1, 13);
IPAddress gateway_ap(192, 168, 1, 13);
IPAddress subnet_ap(255, 255, 255, 0);
//----------------------------------------------------------------------
//===============================================================================
// Những biến là static nên cần phải được khai báo lại tại
// đầu file .cpp theo cú pháp sau thì mới truy xuất được đến biến static.
bool WIFI::DaLuuMACVaoEEPROM = false;
//===============================================================================
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
String WIFI::LaySoMAC(void) {
#pragma region LaySoMAC
  //===============================================================================
  //---------------------------------------------------------------
  // Lấy số MAC của ESP32, đây là số IMEI của board.
  //---------------------------------------------------------------
  String MAC;
  MAC = WiFi.macAddress();
  MAC.replace(":", "");

#ifdef debug
  Serial.println("");
  Serial.print("MAC: ");
  Serial.println(MAC);
#endif

  // Phát WiFi với số MAC của thiết bị để user biết WiFi là của
  // thiết bị nào đang phát ra để mà cấu hình WiFi cho đúng với thiết bị đó.
  String str = _TenDuAn + MAC;
  str.toCharArray(this->AP_SSID, 24);

#ifdef debug
  Serial.println(this->AP_SSID);
#endif
  //---------------------------------------------------------------

  return MAC;
  //===============================================================================
#pragma endregion LaySoMAC
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::ThietLapAP(void) {
#pragma region ThietLapAP
  //===============================================================================
  if (this->DaBatAP == false) {
    // Bật cờ này lên để k phải gọi lại tác vụ thiết lập AP này nhiều lần.
    this->DaBatAP = true;

    // Chuyển sang mode phát Access Point của ESP32.
    // Lưu ý phải là mode WIFI_AP_STA để lúc này ESP32 vừa
    // có thể phát Access Point vừa là Station kết nối WiFi
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip_ap, gateway_ap, subnet_ap);
    WiFi.softAP(this->AP_SSID, this->AP_PASS);

    ServerON();
#ifdef debug
    Serial.println("Đang thiết lập Access Point...");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("WiFi do ESP32 phát ra: ");
    Serial.print(this->AP_SSID);
    Serial.print("|Mật khẩu: ");
    Serial.println(this->AP_PASS);
#endif
  } else {
    server.handleClient();
#ifdef debug
    Serial.println("Access Point đã được thiết lập!");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("WiFi do ESP32 phát ra: ");
    Serial.print(this->AP_SSID);
    Serial.print("|Mật khẩu: ");
    Serial.println(this->AP_PASS);
#endif
  }
  //===============================================================================
#pragma endregion ThietLapAP
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::TrangChu(void) {
  String s = FPSTR(MainpageHTML);
  server.send(200, "text/html", s);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::StyleCSS(void) {
  String s = FPSTR(style_css);
  server.send(200, "text/css", s);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::CodeJS(void) {
  String s = FPSTR(code_js);
  server.send(200, "text/js", s);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::ServerON(void) {
#pragma region ServerON
  server.on("/", WIFI::TrangChu);                          // Mở trang web để cấu hình WiFi cho board
  server.on("/style.css", WIFI::StyleCSS);                 // Gửi style và css qua file style.css
  server.on("/code.js", WIFI::CodeJS);                     // Gửi code js qua file code.js
  server.on("/LuuMACMasterVaoEEPROM", WIFI::LuuMACMasterVaoEEPROM);  // Gọi hàm khi user bấm nút LuuMACMasterVaoEEPROM
  server.begin();                                          // Khởi động server
#pragma endregion ServerON
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::LuuMACMasterVaoEEPROM(void) {
#pragma region LuuMACMasterVaoEEPROM
  //===============================================================================
  String MACMaster = server.arg("MACMaster");

  // Trước tiên cần xóa trắng thông số board đã lưu trước đó.
  for (int i = 0; i < EEPROM_SSID; ++i) {
    EEPROM.write(i, 0);
  }

  for (int i = 0; i < MACMaster.length(); ++i) {
    EEPROM.write(i, MACMaster[i]);
  }

  #ifdef debug
  Serial.println("Đã Lưu MACMaster vào EEPROM...");
 #endif

  // The write command does not write directly to EEPROM. It just prepares an array
  // of bytes which must then be saved in the memory area using the commit() command.
  EEPROM.commit();
  delay(500);

  WIFI::DaLuuMACVaoEEPROM = true;

  String str;
  str = "Lưu thành công MACMaster:";
  str += MACMaster;
  server.send(200, "text/html", str);
  //===============================================================================
#pragma endregion LuuMACMasterVaoEEPROM
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
