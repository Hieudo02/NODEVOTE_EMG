#include "74HC595.h"

uint8_t LIB_74HC595::LATCH_PIN, LIB_74HC595::DATA_PIN, LIB_74HC595::CLOCK_PIN, LIB_74HC595::EN_PIN;

void LIB_74HC595::TatLED(void) {
  LIB_74HC595::send_data(0x0000);
}

void LIB_74HC595::setup(uint8_t latch, uint8_t data, uint8_t clock, uint8_t en) {
  LIB_74HC595::LATCH_PIN = latch;
  LIB_74HC595::DATA_PIN = data;
  LIB_74HC595::CLOCK_PIN = clock;
  LIB_74HC595::EN_PIN = en;

  pinMode(LIB_74HC595::LATCH_PIN, OUTPUT);
  pinMode(LIB_74HC595::DATA_PIN, OUTPUT);
  pinMode(LIB_74HC595::CLOCK_PIN, OUTPUT);
  pinMode(LIB_74HC595::EN_PIN, OUTPUT);

  digitalWrite(LIB_74HC595::LATCH_PIN, LOW);
  digitalWrite(LIB_74HC595::DATA_PIN, LOW);
  digitalWrite(LIB_74HC595::CLOCK_PIN, LOW);
  digitalWrite(LIB_74HC595::EN_PIN, LOW);

  LIB_74HC595::TatLED();
}

void LIB_74HC595::clock_signal(void) {
  digitalWrite(LIB_74HC595::CLOCK_PIN, HIGH);
  // delayMicroseconds(500);
  digitalWrite(LIB_74HC595::CLOCK_PIN, LOW);
  // delayMicroseconds(500);
}

void LIB_74HC595::latch_enable(void) {
  digitalWrite(LIB_74HC595::LATCH_PIN, HIGH);
  // delayMicroseconds(500);
  digitalWrite(LIB_74HC595::LATCH_PIN, LOW);
}

void LIB_74HC595::send_data(unsigned int data_out) {
  for (uint8_t i = 0; i < 16; i++) {
    if ((data_out >> i) & (0x01)) digitalWrite(LIB_74HC595::DATA_PIN, HIGH);
    else digitalWrite(LIB_74HC595::DATA_PIN, LOW);

    LIB_74HC595::clock_signal();
  }
  LIB_74HC595::latch_enable();  // Data finally submitted
}

void LIB_74HC595::BatLED(uint8_t button, TenMauLED_t tenmauled) {
  // { 13, 12, 14, 4 };  // {A, B, C, D}
  unsigned int data = 0;
  switch (button) {
    case 13:
      data = (tenmauled == RED) ? 0x0800 : ((tenmauled == GREEN) ? 0x0400 : 0x0200);
      break;
    case 12:
      data = (tenmauled == RED) ? 0x4000 : ((tenmauled == GREEN) ? 0x2000 : 0x1000);
      break;
    case 14:
      data = (tenmauled == RED) ? 0x0008 : ((tenmauled == GREEN) ? 0x0004 : 0x0002);
      break;
    case 4:
      data = (tenmauled == RED) ? 0x0010 : ((tenmauled == GREEN) ? 0x0020 : 0x0040);
      break;
    default:
      break;
  }
  LIB_74HC595::send_data(data);
}

void LIB_74HC595::NhayLEDGreen(uint8_t button) {
  // Có kêu còi
  // { 13, 12, 14, 4 };  // {A, B, C, D}
  unsigned int data = 0;
  switch (button) {
    // case 13:
    case 0:
      data = 0x0401;
      break;
    // case 12:
    case 1:
      data = 0x2001;
      break;
    // case 14:
    case 2:
      data = 0x0005;
      break;
    // case 4:
    case 3:
      data = 0x0021;
      break;
    default:
      break;
  }

  for (uint8_t i = 0; i < 10; i++) {
    send_data(data);
    delay(100);
    send_data(0x0000);
    delay(100);
  }
}

void LIB_74HC595::NhayLEDGreenTheoNutNhan(uint8_t button) {
  // Có kêu còi
  // { 13, 12, 14, 4 };  // {A, B, C, D}
  unsigned int data = 0;
  switch (button) {
    case 13:
      data = 0x0401;
      break;
    case 12:
      data = 0x2001;
      break;
    case 14:
      data = 0x0005;
      break;
    case 4:
      data = 0x0021;
      break;
    default:
      break;
  }

  for (uint8_t i = 0; i < 15; i++) {
    send_data(data);
    delay(100);
    send_data(0x0000);
    delay(100);
  }
}

void LIB_74HC595::NhayLEDRed(uint8_t button, uint8_t NutDapAnDungGuiTuServer) {
  // Có kêu còi
  // { 13, 12, 14, 4 };  // {A, B, C, D}
  unsigned int data1 = 0;  // Sáng nút đúng (chứa bit của nút sai (nháy) và nút đúng)
  unsigned int data2 = 0;  // Nháy nút sai
  switch (button) {
    case 13:  // A
      switch (NutDapAnDungGuiTuServer) {
        case 1:  // B (server)
          data1 = 0x2801;
          data2 = 0x2000;
          break;
        case 2:  // C (server)
          data1 = 0x0805;
          data2 = 0x0004;
          break;
        case 3:  // D (server)
          data1 = 0x0821;
          data2 = 0x0020;
          break;
      }
      break;
    case 12:  // B
      switch (NutDapAnDungGuiTuServer) {
        case 0:  // A (server)
          data1 = 0x4401;
          data2 = 0x0400;
          break;
        case 2:  // C (server)
          data1 = 0x4005;
          data2 = 0x0004;
          break;
        case 3:  // D (server)
          data1 = 0x4021;
          data2 = 0x0020;
          break;
      }
      break;
    case 14:  // C
      switch (NutDapAnDungGuiTuServer) {
        case 0:  // A (server)
          data1 = 0x0409;
          data2 = 0x0400;
          break;
        case 1:  // B (server)
          data1 = 0x2009;
          data2 = 0x2000;
          break;
        case 3:  // D (server)
          data1 = 0x0029;
          data2 = 0x0020;
          break;
      }
      break;
    case 4:  // D
      switch (NutDapAnDungGuiTuServer) {
        case 0:  // A (server)
          data1 = 0x0411;
          data2 = 0x0400;
          break;
        case 1:  // B (server)
          data1 = 0x2011;
          data2 = 0x2000;
          break;
        case 2:  // C (server)
          data1 = 0x0015;
          data2 = 0x0004;
          break;
      }
      break;
    default:
      break;
  }

  for (uint8_t i = 0; i < 15; i++) {
    send_data(data1);
    delay(100);
    send_data(data2);
    delay(100);
  }
  send_data(0x0000);
}

void LIB_74HC595::NhayLEDPinYeu(uint8_t batterryPin) {
  if (analogRead(batterryPin) < 500) {
    send_data(0x0080);
    delay(1000);
    send_data(0x0080);
    delay(1000);
  }
}