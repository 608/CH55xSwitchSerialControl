#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "HIDGamepad.h"
#include "USBconstant.h"
#include "USBhandler.h"

extern __xdata __at(EP1_ADDR)
uint8_t Ep1Buffer[];

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
    Ep1Buffer[i + 64] = p[i];
  }
  UEP1_T_LEN = 8;
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}


