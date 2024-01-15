#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "HIDdevice.h"
#include "USBconstant.h"
#include "USBhandler.h"

__code uint16_t _asciimap[128] = {
  KEY_RESERVED,   // NUL
  KEY_RESERVED,   // SOH
  KEY_RESERVED,   // STX
  KEY_RESERVED,   // ETX
  KEY_RESERVED,   // EOT
  KEY_RESERVED,   // ENQ
  KEY_RESERVED,   // ACK
  KEY_RESERVED,   // BEL
  KEY_BACKSPACE,  // BS   Backspace
  KEY_TAB,        // TAB  Tab
  KEY_ENTER,      // LF   Enter
  KEY_RESERVED,   // VT
  KEY_RESERVED,   // FF
  KEY_RESERVED,   // CR
  KEY_RESERVED,   // SO
  KEY_RESERVED,   // SI
  KEY_RESERVED,   // DEL
  KEY_RESERVED,   // DC1
  KEY_RESERVED,   // DC2
  KEY_RESERVED,   // DC3
  KEY_RESERVED,   // DC4
  KEY_RESERVED,   // NAK
  KEY_RESERVED,   // SYN
  KEY_RESERVED,   // ETB
  KEY_RESERVED,   // CAN
  KEY_RESERVED,   // EM
  KEY_RESERVED,   // SUB
  KEY_RESERVED,   // ESC
  KEY_RESERVED,   // FS
  KEY_RESERVED,   // GS
  KEY_RESERVED,   // RS
  KEY_RESERVED,   // US

  KEY_SPACE,                          // ' ' Space
  KEY_1 | MOD_LEFT_SHIFT,             // !
  KEY_2 | MOD_LEFT_SHIFT,             // "
  KEY_3 | MOD_LEFT_SHIFT,             // #
  KEY_4 | MOD_LEFT_SHIFT,             // $
  KEY_5 | MOD_LEFT_SHIFT,             // %
  KEY_6 | MOD_LEFT_SHIFT,             // &
  KEY_7 | MOD_LEFT_SHIFT,             // '
  KEY_8 | MOD_LEFT_SHIFT,             // (
  KEY_9 | MOD_LEFT_SHIFT,             // )
  KEY_QUOTE | MOD_LEFT_SHIFT,         // *
  KEY_SEMICOLON | MOD_LEFT_SHIFT,     // +
  KEY_COMMA,                          // ,
  KEY_MINUS,                          // -
  KEY_PERIOD,                         // .
  KEY_SLASH,                          // /
  KEY_0,                              // 0
  KEY_1,                              // 1
  KEY_2,                              // 2
  KEY_3,                              // 3
  KEY_4,                              // 4
  KEY_5,                              // 5
  KEY_6,                              // 6
  KEY_7,                              // 7
  KEY_8,                              // 8
  KEY_9,                              // 9
  KEY_QUOTE,                          // :
  KEY_SEMICOLON,                      // ;
  KEY_COMMA | MOD_LEFT_SHIFT,         // <
  KEY_MINUS | MOD_LEFT_SHIFT,         // =
  KEY_PERIOD | MOD_LEFT_SHIFT,        // >
  KEY_SLASH | MOD_LEFT_SHIFT,         // ?
  KEY_LEFT_BRACE,                     // @
  KEY_A | MOD_LEFT_SHIFT,             // A
  KEY_B | MOD_LEFT_SHIFT,             // B
  KEY_C | MOD_LEFT_SHIFT,             // C
  KEY_D | MOD_LEFT_SHIFT,             // D
  KEY_E | MOD_LEFT_SHIFT,             // E
  KEY_F | MOD_LEFT_SHIFT,             // F
  KEY_G | MOD_LEFT_SHIFT,             // G
  KEY_H | MOD_LEFT_SHIFT,             // H
  KEY_I | MOD_LEFT_SHIFT,             // I
  KEY_J | MOD_LEFT_SHIFT,             // J
  KEY_K | MOD_LEFT_SHIFT,             // K
  KEY_L | MOD_LEFT_SHIFT,             // L
  KEY_M | MOD_LEFT_SHIFT,             // M
  KEY_N | MOD_LEFT_SHIFT,             // N
  KEY_O | MOD_LEFT_SHIFT,             // O
  KEY_P | MOD_LEFT_SHIFT,             // P
  KEY_Q | MOD_LEFT_SHIFT,             // Q
  KEY_R | MOD_LEFT_SHIFT,             // R
  KEY_S | MOD_LEFT_SHIFT,             // S
  KEY_T | MOD_LEFT_SHIFT,             // T
  KEY_U | MOD_LEFT_SHIFT,             // U
  KEY_V | MOD_LEFT_SHIFT,             // V
  KEY_W | MOD_LEFT_SHIFT,             // W
  KEY_X | MOD_LEFT_SHIFT,             // X
  KEY_Y | MOD_LEFT_SHIFT,             // Y
  KEY_Z | MOD_LEFT_SHIFT,             // Z
  KEY_RIGHT_BRACE,                    // [
  KEY_JP_BACKSLASH,                   // bslash
  KEY_BACKSLASH,                      // ]
  KEY_EQUAL,                          // ^
  KEY_JP_BACKSLASH | MOD_LEFT_SHIFT,  // _
  KEY_LEFT_BRACE | MOD_LEFT_SHIFT,    // `
  KEY_A,                              // a
  KEY_B,                              // b
  KEY_C,                              // c
  KEY_D,                              // d
  KEY_E,                              // e
  KEY_F,                              // f
  KEY_G,                              // g
  KEY_H,                              // h
  KEY_I,                              // i
  KEY_J,                              // j
  KEY_K,                              // k
  KEY_L,                              // l
  KEY_M,                              // m
  KEY_N,                              // n
  KEY_O,                              // o
  KEY_P,                              // p
  KEY_Q,                              // q
  KEY_R,                              // r
  KEY_S,                              // s
  KEY_T,                              // t
  KEY_U,                              // u
  KEY_V,                              // v
  KEY_W,                              // w
  KEY_X,                              // x
  KEY_Y,                              // y
  KEY_Z,                              // z
  KEY_RIGHT_BRACE | MOD_LEFT_SHIFT,   // {
  KEY_JP_YEN | MOD_LEFT_SHIFT,        // |
  KEY_BACKSLASH | MOD_LEFT_SHIFT,     // }
  KEY_EQUAL | MOD_LEFT_SHIFT,         // ~
  KEY_RESERVED,                       // 127 - DEL
  // 7-bit ASCII codes end here
};

extern __xdata __at(EP1_ADDR)
uint8_t Ep1Buffer[];

extern __xdata __at(EP2_ADDR)
uint8_t Ep2Buffer[];

__xdata uint8_t HIDKey[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

extern void USB_EP1_IN() {
  UEP1_T_LEN = 0;
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;  // Default NAK
}

extern void USB_EP1_OUT() {
  if (U_TOG_OK)  // Discard unsynchronized packets
  {
  }
}

void USB_EP1_send() {
  for (uint8_t i = 0; i < sizeof(HIDKey); i++) {
    Ep1Buffer[64 + i] = HIDKey[i];
  }
  UEP1_T_LEN = sizeof(HIDKey);
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

void USBInit(void) {
  USBDeviceCfg();
  USBDeviceEndPointCfg();
  USBDeviceIntCfg();
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0;
  UEP2_T_LEN = 0;
}

void sendReport(uint8_t* p) {
  for (uint8_t i = 0; i < 8; i++) {
    Ep2Buffer[i + 64] = p[i];
  }
  UEP2_T_LEN = 8;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

void releaseAllKey(void) {
  for (uint8_t i = 0; i < sizeof(HIDKey); i++) {  //load data for upload
    HIDKey[i] = 0;
  }
  USB_EP1_send();
}

void pressSpecialKey(uint8_t c) {

  if (c != KEY_JP_HANZEN && c != KEY_JP_BACKSLASH && c != KEY_JP_HIRAGANA && c != KEY_JP_YEN && c != KEY_JP_HENKAN && c != KEY_JP_MUHENKAN && c != KEY_ENTER && c != KEY_BACKSPACE && c != KEY_DELETE && c != KEY_UP_ARROW && c != KEY_DOWN_ARROW && c != KEY_LEFT_ARROW && c != KEY_RIGHT_ARROW) {
    uint16_t k = _asciimap[c & 0x7F];
    HIDKey[0] = k >> 8;
    c = k & 0xFF;
  }
  uint8_t i;

  if (HIDKey[2] != c && HIDKey[3] != c && HIDKey[4] != c && HIDKey[5] != c && HIDKey[6] != c && HIDKey[7] != c) {

    for (i = 2; i < 8; i++) {
      if (HIDKey[i] == 0x00) {
        HIDKey[i] = c;
        break;
      }
    }
    if (i == 8) {
      //setWriteError();
      return;
    }
  }
  USB_EP1_send();
}

void pressKey(uint8_t c) {

  uint16_t k = _asciimap[c & 0x7F];
  HIDKey[0] = k >> 8;
  c = k & 0xFF;
  uint8_t i;

  if (HIDKey[2] != c && HIDKey[3] != c && HIDKey[4] != c && HIDKey[5] != c && HIDKey[6] != c && HIDKey[7] != c) {

    for (i = 2; i < 8; i++) {
      if (HIDKey[i] == 0x00) {
        HIDKey[i] = c;
        break;
      }
    }
    if (i == 8) {
      //setWriteError();
      return;
    }
  }
  USB_EP1_send();
}

void releaseSpecialKey(uint8_t c) {

  if (c != KEY_JP_HANZEN && c != KEY_JP_BACKSLASH && c != KEY_JP_HIRAGANA && c != KEY_JP_YEN && c != KEY_JP_HENKAN && c != KEY_JP_MUHENKAN && c != KEY_ENTER && c != KEY_BACKSPACE && c != KEY_DELETE && c != KEY_UP_ARROW && c != KEY_DOWN_ARROW && c != KEY_LEFT_ARROW && c != KEY_RIGHT_ARROW) {
    uint16_t k = _asciimap[c & 0x7F];
    HIDKey[0] &= ~(k >> 8);
    c = k & 0xFF;
  }
  uint8_t i;

  for (i = 2; i < 8; i++) {
    if (HIDKey[i] == c) {
      HIDKey[i] = 0;
      break;
    }
  }
  if (i == 8) {
    //setWriteError();
    return;
  }
  USB_EP1_send();
}

void releaseKey(uint8_t c) {

  uint16_t k = _asciimap[c & 0x7F];
  HIDKey[0] &= ~(k >> 8);
  c = k & 0xFF;
  uint8_t i;

  for (i = 2; i < 8; i++) {
    if (HIDKey[i] == c) {
      HIDKey[i] = 0;
      break;
    }
  }
  if (i == 8) {
    //setWriteError();
    return;
  }
  USB_EP1_send();
}

void pushKey(uint8_t c) {
  pressKey(c);
  delay(30);
  releaseKey(c);
  delay(30);
}

void pushSpecialKey(uint8_t c) {
  pressSpecialKey(c);
  delay(30);
  releaseSpecialKey(c);
  delay(30);
}