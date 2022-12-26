#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "HIDdevice.h"
#include "USBconstant.h"
#include "USBhandler.h"

extern __xdata __at(EP1_ADDR)
uint8_t Ep1Buffer[];

extern __xdata __at(EP2_ADDR)
uint8_t Ep2Buffer[];

__xdata uint8_t HIDKey[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
__xdata USB_JoystickReport_Input_t controller_report;

volatile __xdata uint8_t UpPoint1_Busy = 0;  //Flag of whether upload pointer is busy

extern void USB_EP1_IN() {
  UEP1_T_LEN = 0;
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;  // Default NAK
  UpPoint1_Busy = 0;                                        //Clear busy flag
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
    uint16_t k = _asciimap[c];
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

  uint16_t k = _asciimap[c];
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
    uint16_t k = _asciimap[c];
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

  uint16_t k = _asciimap[c];
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