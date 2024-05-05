#ifndef _POSTGET_h
#define _POSTGET_h

#include "VoteGWMaster.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <WebSocketsClient.h>
#include <vector>
#include <algorithm>
#include <string>

// Các trạng thái để xác định khi nhận answerCorrect xong mới gửi ESPNow về Node
#define showQ 0 // showQuestion
#define ts 1    // timerTick
#define cl 2    // clearBoard
#define ansC 3  // answerCorrect
#define fn 4    // finished

class POSTGET {
public:
  static String realm;
  static String ip_host;
  static String port;
  static String position;

  WebSocketsClient webSocket;         //
  static char DuLieuServerGuiVe[20];  //
  static char DuLieuServerGuiVeTruocDo[20];  //
  static char IDCuaDapAnNhanTuServer[20];
  static bool FlagChoPhepThamGiaGame;
  static bool FlagChoPhepGuiDapAnLenServer;
  static bool FlagGuiResetDapAn;

  static int prev;
  static int cur;

  static int count;
  
  std::vector<std::string> DanhSachMACsThamGiaGame;  // Vector MAC để quản lý những MAC nào đã tham gia game
  std::vector<std::string> FIFODuLieu;  // Ống chứa dữ liệu Node gửi lên FIFO: First In First Out
  static std::vector<std::string> FIFODapAnDung; // Ống chứa dữ liệu đáp án đúng gửi từ Server


public:
  void KetNoiVaoWebSocket(void);
  void playerJoin(char playerMAC[], char IndexButton);
  void setAnswer(char playerMAC[], char DapAn);
  void resetAnswer(char DapAn);

private:
  static void WEBSocketEvent(WStype_t type, uint8_t* payload, size_t length);
};

#endif
