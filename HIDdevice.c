#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "HIDdevice.h"
#include "USBconstant.h"
#include "USBhandler.h"

#define EMULATE_PROCON

extern __xdata __at(EP1_ADDR)
uint8_t Ep1Buffer[];

extern __xdata __at(EP2_ADDR)
uint8_t Ep2Buffer[];

__xdata uint8_t HIDKey[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

#ifdef EMULATE_PROCON

__xdata uint8_t controller_color[] = {0xFF, 0xFF, 0xFF};
__xdata uint8_t button_color[] = {0xFF, 0xFF, 0xFF};
__xdata uint8_t left_grip_color[] = {0xFF, 0xFF, 0xFF};
__xdata uint8_t right_grip_color[] = {0xFF, 0xFF, 0xFF};

#define COUNTER_INCREMENT 3

static __xdata bool startReport = false;
static __xdata bool imu_enable = false;
USB_ExtendedReport_t pc_report;
// Private functions (definition)
static void prepare_reply(uint8_t code, uint8_t command, uint8_t* data, size_t length);
static void prepare_uart_reply(uint8_t code, uint8_t subcommand, uint8_t* data, size_t length);
static void prepare_spi_reply(SPI_Address_t address, size_t size);
static void prepare_standard_report();
static void prepare_extended_report();
static void prepare_8101();
static void spi_read(SPI_Address_t address, size_t size, uint8_t* buf);
void sendReport(void);

__xdata uint8_t mac_address[] = {0xD4, 0xF0, 0x57, 0x8D, 0x74, 0x23};
__xdata uint8_t replyBuffer[64];
static __xdata uint8_t counter = 0;
static __xdata bool nextPacketReady = false;

#else
__xdata USB_JoystickReport_Input_t controller_report;
#endif

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

#ifdef EMULATE_PROCON

static size_t min_size(size_t s1, size_t s2) {
    return s1 > s2 ? s2 : s1;
}

extern void USB_EP2_IN() {
  UEP2_T_LEN = 0;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;  // Default NAK
}

extern void USB_EP2_OUT() {
    if (Ep2Buffer[0] == 0x80) {
        switch (Ep2Buffer[1]) {
            case 0x01: {
                prepare_8101();
                break;
            }
            case 0x02:
            case 0x03: {
                prepare_reply(0x81, Ep2Buffer[1], NULL, 0);
                break;
            }
            case 0x04: {
                startReport = true;
                prepare_standard_report();
                break;
            }
            case 0x05: {
                startReport = false;
                break;
            }
            default: {
                // TODO
                prepare_reply(0x81, Ep2Buffer[1], NULL, 0);
                break;
            }
        }
    } else if (Ep2Buffer[0] == 0x01) { //&& USB_RX_LEN > 16) {
        Switch_Subcommand_t subcommand = Ep2Buffer[10];
        switch (subcommand) {
            case SUBCOMMAND_BLUETOOTH_MANUAL_PAIRING: {
                __xdata uint8_t buf[] = {0x03};
                prepare_uart_reply(0x81, subcommand, buf, sizeof(buf));
                break;
            }
            case SUBCOMMAND_REQUEST_DEVICE_INFO: {
                size_t n = sizeof(mac_address); // = 6
                __xdata uint8_t buf[12];
                buf[0] = 0x03; buf[1] = 0x48; // Firmware version
                buf[2] = 0x03; // Pro Controller
                buf[3] = 0x02; // Unkown
                // MAC address is flipped (big-endian)
                for (unsigned int i = 0; i < n; i++) {
                    buf[(n + 3) - i] = mac_address[i];
                }
                buf[10] = 0x03; // Unknown
                buf[11] = 0x02; // Use colors in SPI memory, and use grip colors (added in Switch firmware 5.0)
                prepare_uart_reply(0x82, subcommand, buf, sizeof(buf));
                break;
            }
            case SUBCOMMAND_SET_INPUT_REPORT_MODE:
            case SUBCOMMAND_SET_SHIPMENT_LOW_POWER_STATE:
            case SUBCOMMAND_SET_PLAYER_LIGHTS:
            case SUBCOMMAND_SET_HOME_LIGHTS:
            case SUBCOMMAND_ENABLE_VIBRATION: {
                prepare_uart_reply(0x80, subcommand, NULL, 0);
                break;
            }
            case SUBCOMMAND_ENABLE_IMU: {
                if (Ep2Buffer[11] == 0)
                {
                    imu_enable = false;
                }
                else
                {
                    imu_enable = true;
                }
                prepare_uart_reply(0x80, subcommand, NULL, 0);
                break;
            }
            case SUBCOMMAND_TRIGGER_BUTTONS_ELAPSED_TIME: {
                prepare_uart_reply(0x83, subcommand, NULL, 0);
                break;
            }
            case SUBCOMMAND_SET_NFC_IR_MCU_CONFIG: {
                __xdata uint8_t buf[] = {0x01, 0x00, 0xFF, 0x00, 0x03, 0x00, 0x05, 0x01};
                prepare_uart_reply(0xA0, subcommand, buf, sizeof(buf));
                break;
            }
            case SUBCOMMAND_SPI_FLASH_READ: {
                // SPI
                // Addresses are little-endian, so 80 60 means address 0x6080
                SPI_Address_t address = (Ep2Buffer[12] << 8) | Ep2Buffer[11];
                size_t size = (size_t) Ep2Buffer[15];
                prepare_spi_reply(address, size);
                break;
            }
            default: {
                // TODO
                prepare_uart_reply(0x80, subcommand, NULL, 0);
                break;
            }
        }
    }
    sendReport();
}

static void prepare_reply(uint8_t code, uint8_t command, uint8_t* data, size_t length) {
    if (nextPacketReady) return;
    memset(replyBuffer, 0, sizeof(replyBuffer));
    replyBuffer[0] = code;
    replyBuffer[1] = command;
    //memcpy(&replyBuffer[2], &data[0], length);
    for (int i=0;i<length;i++) {
        replyBuffer[i+2] = data[i];
    }
    nextPacketReady = true;
}

static void prepare_uart_reply(uint8_t code, uint8_t subcommand, uint8_t* data, size_t length) {
    if (nextPacketReady) return;
    memset(replyBuffer, 0, sizeof(replyBuffer));
    replyBuffer[0] = 0x21;

    counter += COUNTER_INCREMENT;
    replyBuffer[1] = counter;

    //disable_rx_isr();
    //USB_StandardReport_t *selectedReport = &((*selectedReportPtr)->standardReport);
    size_t n = sizeof(USB_StandardReport_t);
    //enable_rx_isr();
    memcpy(&replyBuffer[2], &pc_report.standardReport, n);
    replyBuffer[n + 2] = code;
    replyBuffer[n + 3] = subcommand;
    memcpy(&replyBuffer[n + 4], &data[0], length);
    nextPacketReady = true;
}

static void prepare_spi_reply(SPI_Address_t address, size_t size) {
    __xdata uint8_t data[32];
    // Populate buffer with data read from SPI flash
    spi_read(address, size, data);

    __xdata uint8_t spiReplyBuffer[32];
    // Big-endian
    spiReplyBuffer[0] = address & 0xFF;
    spiReplyBuffer[1] = (address >> 8) & 0xFF;
    spiReplyBuffer[2] = 0x00;
    spiReplyBuffer[3] = 0x00;
    spiReplyBuffer[4] = size;
    memcpy(&spiReplyBuffer[5], &data[0], size);

    prepare_uart_reply(0x90, SUBCOMMAND_SPI_FLASH_READ, spiReplyBuffer, sizeof(spiReplyBuffer));
}

static void prepare_standard_report() {
    if (nextPacketReady) return;
    counter += COUNTER_INCREMENT;
    //disable_rx_isr();
    prepare_reply(0x30, counter, (uint8_t*)&pc_report.standardReport, sizeof(USB_StandardReport_t));
    //enable_rx_isr();
}

static void prepare_extended_report() {
    if (nextPacketReady) return;
    counter += COUNTER_INCREMENT;
    //disable_rx_isr();
    prepare_reply(0x30, counter, (uint8_t*)&pc_report, sizeof(USB_ExtendedReport_t));
    //enable_rx_isr();
}

static void prepare_8101() {
    if (nextPacketReady) return;
    //size_t n = sizeof(mac_address); // = 6
    __xdata uint8_t buf[8];
    buf[0] = 0x00;
    buf[1] = 0x03; // Pro Controller
    memcpy(&buf[2], &mac_address[0], 6);
    prepare_reply(0x81, 0x01, buf, sizeof(buf));
}

void spi_read(SPI_Address_t address, size_t size, uint8_t* buf) {
    memset(buf, 0xFF, size);
    switch (address) {
        default:
        case ADDRESS_SERIAL_NUMBER: {
            // All 0xFF, leave buf as it is
            break;
        }
        case ADDRESS_CONTROLLER_COLOR: {
            if (size >= 3) {
                memcpy(&buf[0], &controller_color[0], sizeof(controller_color));
            }
            if (size >= 6) {
                memcpy(&buf[3], &button_color[0], sizeof(button_color));
            }
            if (size >= 9) {
                memcpy(&buf[6], &left_grip_color[0], sizeof(left_grip_color));
            }
            if (size >= 12) {
                memcpy(&buf[9], &right_grip_color[0], sizeof(right_grip_color));
            }
            break;
        }
        case ADDRESS_FACTORY_PARAMETERS_1: {
            __xdata uint8_t factory_parameters_1[] = {0x50, 0xfd, 0x00, 0x00, 0xc6, 0x0f, 0x0f, 0x30, 0x61, 0x96, 0x30, 0xf3,
                                              0xd4, 0x14, 0x54, 0x41, 0x15, 0x54, 0xc7, 0x79, 0x9c, 0x33, 0x36, 0x63};
            memcpy(&buf[0], &factory_parameters_1[0], min_size(size, sizeof(factory_parameters_1)));
            break;
        }
        case ADDRESS_FACTORY_PARAMETERS_2: {
            __xdata uint8_t factory_parameters_2[] = {0x0f, 0x30, 0x61, 0x96, 0x30, 0xf3, 0xd4, 0x14, 0x54,
                                              0x41, 0x15, 0x54, 0xc7, 0x79, 0x9c, 0x33, 0x36, 0x63};
            memcpy(&buf[0], &factory_parameters_2[0], min_size(size, sizeof(factory_parameters_2)));
            break;
        }
        case ADDRESS_FACTORY_CALIBRATION_1: {
            __xdata uint8_t factory_calibration_1[] = {0xE6, 0xFF, 0x3A, 0x00, 0x39, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
                                               0xF7, 0xFF, 0xFC, 0xFF, 0x00, 0x00, 0xE7, 0x3B, 0xE7, 0x3B, 0xE7, 0x3B};
            memcpy(&buf[0], &factory_calibration_1[0], min_size(size, sizeof(factory_calibration_1)));
            break;
        }
        case ADDRESS_FACTORY_CALIBRATION_2: {
            __xdata uint8_t factory_calibration_2[] = {0xba, 0x15, 0x62, 0x11, 0xb8, 0x7f, 0x29, 0x06, 0x5b, 0xff, 0xe7, 0x7e,
                                               0x0e, 0x36, 0x56, 0x9e, 0x85, 0x60, 0xff, 0x32, 0x32, 0x32, 0xff, 0xff,
                                               0xff};
            memcpy(&buf[0], &factory_calibration_2[0], min_size(size, sizeof(factory_calibration_2)));
            break;
        }
        case ADDRESS_STICKS_CALIBRATION: {
            if (size > 22) {
                buf[22] = 0xB2;
            }
            if (size > 23) {
                buf[23] = 0xA1;
            }
            // spi_response(data[11:13], bytes.fromhex('ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff b2 a1'))
            break;
        }
        case ADDRESS_IMU_CALIBRATION: {
            __xdata uint8_t imu_calibration[] = {0xbe, 0xff, 0x3e, 0x00, 0xf0, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
                                         0xfe, 0xff, 0xfe, 0xff, 0x08, 0x00, 0xe7, 0x3b, 0xe7, 0x3b, 0xe7, 0x3b};
            memcpy(&buf[0], &imu_calibration[0], min_size(size, sizeof(imu_calibration)));
            break;
        }
    }
}

#endif

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

#ifdef EMULATE_PROCON
void sendReport(void) {
    if (!nextPacketReady) {
        // No requests from Switch, use standard report
        if (startReport)
        {
            if (imu_enable)
            {
                //Serial_SendString("imu_enable\n");
                prepare_extended_report();
            }
            else
            {
                //Serial_SendString("imu_disable\n");
                prepare_standard_report();
            }
        }
    }

    if (nextPacketReady)
    {
        for (uint8_t i = 0; i <64; i++) {
          Ep2Buffer[i + 64] = replyBuffer[i];
        }
        UEP2_T_LEN = 64;
        UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
        nextPacketReady = false;
    }
}
#else
void sendReport(uint8_t* p) {
  for (uint8_t i = 0; i < 8; i++) {
    Ep2Buffer[i + 64] = p[i];
  }
  UEP2_T_LEN = 8;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}
#endif

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