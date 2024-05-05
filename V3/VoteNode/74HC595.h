#ifndef _74HC595_H
#define _74HC595_H

#include <Arduino.h>

class LIB_74HC595 {
public:
  typedef enum {
    RED,
    GREEN,
    BLUE,
  } TenMauLED_t;

  static uint8_t LATCH_PIN, DATA_PIN, CLOCK_PIN, EN_PIN;

  static void setup(uint8_t latch, uint8_t data, uint8_t clock, uint8_t en);
  static void send_data(unsigned int data_out);
  void BatLED(uint8_t button, TenMauLED_t tenmauled);
  static void TatLED(void);
  void NhayLEDGreen(uint8_t button);
  void NhayLEDGreenTheoNutNhan(uint8_t button);
  // void NhayLEDRed(uint8_t button);
  void NhayLEDRed(uint8_t button, uint8_t NutDapAnDungGuiTuServer);
  void NhayLEDPinYeu(uint8_t batterryPin);
private:
  static void clock_signal(void);
  static void latch_enable(void);
};



#endif
