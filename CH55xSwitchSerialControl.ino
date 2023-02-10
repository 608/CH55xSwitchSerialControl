#include <Serial.h>
#include <HardwareSerial.h>
#include "HIDdevice.h"
#define MAX_BUFFER 32

__xdata char pc_report_str[MAX_BUFFER];
__xdata uint16_t idx = 0;
__xdata uint32_t timeoutCnt = 0;

__xdata uint8_t pc_lx, pc_ly, pc_rx, pc_ry;

__xdata bool isNx2 = false;
__xdata bool isText = false;

USB_JoystickReport_Input_t pc_report;

void resetDirections() {
  memset(&pc_report, 0, sizeof(USB_JoystickReport_Input_t));
  pc_report.LX = 128;
  pc_report.LY = 128;
  pc_report.RX = 128;
  pc_report.RY = 128;
  pc_report.Hat = HAT_NEUTRAL;
}

void parseLine(char* line) {
  __xdata uint16_t btns = BUTTON_NONE;
  __xdata uint8_t hat = HAT_NEUTRAL;
  __xdata uint8_t lx = STICK_NEUTRAL;
  __xdata uint8_t ly = STICK_NEUTRAL;
  __xdata uint8_t rx = STICK_NEUTRAL;
  __xdata uint8_t ry = STICK_NEUTRAL;
  __xdata uint8_t keyValue = STICK_NEUTRAL;

  if (strncmp(line, "end", 3) == 0) {
    // pokecon
  } else if (strncmp(line, "RELEASE", 7) == 0) {
    // nx ver 1.0
  } else if (strncmp(line, "Button", 6) == 0) {
    // nx ver 1.0
    if (strncmp(&line[7], "SELECT", 6) == 0) {
      btns = BUTTON_MINUS;
    } else if (line[8] == 'T') {
      btns = BUTTON_PLUS;
    } else if (strncmp(&line[7], "LCLICK", 6) == 0) {
      btns = BUTTON_LCLICK;
    } else if (strncmp(&line[7], "RCLICK", 6) == 0) {
      btns = BUTTON_RCLICK;
    } else if (line[7] == 'H') {
      btns = BUTTON_HOME;
    } else if (line[7] == 'C') {
      btns = BUTTON_CAPTURE;
    } else if (strncmp(&line[7], "RELEASE", 7) == 0) {
      btns = BUTTON_NONE;
    } else if (line[7] == 'A') {
      btns = BUTTON_A;
    } else if (line[7] == 'B') {
      btns = BUTTON_B;
    } else if (line[7] == 'X') {
      btns = BUTTON_X;
    } else if (line[7] == 'Y') {
      btns = BUTTON_Y;
    } else if (line[7] == 'L') {
      btns = BUTTON_L;
    } else if (line[7] == 'R') {
      btns = BUTTON_R;
    } else if (line[8] == 'L') {
      btns = BUTTON_ZL;
    } else if (line[8] == 'R') {
      btns = BUTTON_ZR;
    }
  } else if (strncmp(line, "HAT", 3) == 0) {
    // nx ver 1.0
    if (strncmp(&line[4], "CENTER", 6) == 0) {
      hat = HAT_NEUTRAL;
    } else if (strncmp(&line[4], "TOP_RIGHT", 9) == 0) {
      hat = HAT_UP_RIGHT;
    } else if (line[4] == 'R') {
      hat = HAT_RIGHT;
    } else if (strncmp(&line[4], "BOTTOM_RIGHT", 12) == 0) {
      hat = HAT_DOWN_RIGHT;
    } else if (strncmp(&line[4], "BOTTOM_LEFT", 11) == 0) {
      hat = HAT_DOWN_LEFT;
    } else if (strncmp(&line[4], "BOTTOM", 6) == 0) {
      hat = HAT_DOWN;
    } else if (line[4] == 'L') {
      hat = HAT_LEFT;
    } else if (strncmp(&line[4], "TOP_LEFT", 8) == 0) {
      hat = HAT_UP_LEFT;
    } else if (strncmp(&line[4], "TOP", 3) == 0) {
      hat = HAT_UP;
    }
  } else if (strncmp(line, "LX", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      lx = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      lx = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      lx = STICK_NEUTRAL;
    }
  } else if (strncmp(line, "LY", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      ly = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      ly = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      ly = STICK_NEUTRAL;
    }
  } else if (strncmp(line, "RX", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      rx = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      rx = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      rx = STICK_NEUTRAL;
    }
  } else if (strncmp(line, "RY", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      ry = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      ry = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      ry = STICK_NEUTRAL;
    }
  } else if (line[0] == '\"') {
    // pokecon
    __xdata uint8_t char_pos = 1;
    while (line[char_pos] != 0 && line[char_pos] != '\"' || (line[char_pos + 1] != 0 && line[char_pos + 1] != '\r' && line[char_pos + 1] != '\n')) {
      pushKey(line[char_pos]);
      char_pos++;
    }
  } else if (strncmp(line, "Key", 3) == 0) {
    // pokecon
    __xdata uint8_t char_pos = 4;

    keyValue = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      keyValue *= 10;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        keyValue += (line[char_pos] - '0');
      }
      char_pos++;
    }
    pushKey(keyValue);
  } else if (strncmp(line, "Press", 5) == 0) {
    // pokecon
    __xdata uint8_t char_pos = 6;

    keyValue = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      keyValue *= 10;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        keyValue += (line[char_pos] - '0');
      }
      char_pos++;
    }
    pressKey(keyValue);
  } else if (strncmp(line, "Release", 7) == 0) {
    // pokecon
    __xdata uint8_t char_pos = 8;

    keyValue = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      keyValue *= 10;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        keyValue += (line[char_pos] - '0');
      }
      char_pos++;
    }
    releaseKey(keyValue);
  } else if (line[0] == 0xaa) {
    // nx2 ver 2.00 - 2.07
    btns = line[5] | (line[6] << 8);
    hat = line[7];
    if (line[8] & 1) lx = STICK_MIN;
    if (line[8] & 2) lx = STICK_MAX;
    if (line[8] & 4) ly = STICK_MIN;
    if (line[8] & 8) ly = STICK_MAX;
    if (line[9] & 1) rx = STICK_MIN;
    if (line[9] & 2) rx = STICK_MAX;
    if (line[9] & 4) ry = STICK_MIN;
    if (line[9] & 8) ry = STICK_MAX;
  } else if (line[0] == 0xab) {
    // nx2 ver 2.08
    btns = line[1] | (line[2] << 8);
    hat = line[3];
    lx = line[4];
    ly = line[5];
    rx = line[6];
    ry = line[7];

    // keyboard
    if (line[8] == 1) {
      // normal press
      pressKey(line[9]);
    } else if (line[8] == 2) {
      // normal release
      releaseKey(line[9]);
    } else if (line[8] == 3) {
      // special press
      pressSpecialKey(line[9]);
    } else if (line[8] == 4) {
      // special release
      releaseSpecialKey(line[9]);
    } else if (line[8] == 5) {
      // all release
      releaseAllKey();
    }
  } else if (line[0] >= '0' && line[0] <= '9') {
    // pokecon
    __xdata uint8_t char_pos = 0;

    btns = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      btns *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        btns += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        btns += (line[char_pos] - 'A' + 10);
      } else {
        btns += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    hat = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      hat *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        hat += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        hat += (line[char_pos] - 'A' + 10);
      } else {
        hat += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_lx *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_lx += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_lx += (line[char_pos] - 'A' + 10);
      } else {
        pc_lx += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_ly *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_ly += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_ly += (line[char_pos] - 'A' + 10);
      } else {
        pc_ly += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_rx *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_rx += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_rx += (line[char_pos] - 'A' + 10);
      } else {
        pc_rx += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_ry *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_ry += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_ry += (line[char_pos] - 'A' + 10);
      } else {
        pc_ry += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }

    __xdata int use_right = btns & 0x1;
    __xdata int use_left = btns & 0x2;

    if ((use_right != 0) & (use_left != 0)) {
      lx = pc_lx;
      ly = pc_ly;
      rx = pc_rx;
      ry = pc_ry;
    } else if (use_right != 0) {
      rx = pc_lx;
      ry = pc_ly;
    } else if (use_left != 0) {
      lx = pc_lx;
      ly = pc_ly;
    }

    btns >>= 2;
  }

  pc_report.Button = btns;
  pc_report.Hat = hat;

  pc_report.LX = lx;
  pc_report.LY = ly;
  pc_report.RX = rx;
  pc_report.RY = ry;
}

void setup() {
  USBInit();
  Serial0_begin(9600);
  Serial1_begin(9600);
  resetDirections();
}


/*void Uart1_ISR(void) __interrupt(INT_NO_UART1) {  
    if (U1RI){
        uart1IntRxHandler();
        U1RI =0;
    }
    if (U1TI){
        uart1IntTxHandler();
        U1TI =0;
    }
    //isr_uart_func();
}*/

void loop() {
  while (Serial0_available() || Serial1_available()) {
    timeoutCnt = 1;
    char c = 0;
    if (Serial0_available()) {
      c = Serial0_read();
    } else {
      c = Serial1_read();
    }

    if (c == 0xaa) {
      if (idx == 0) {
        isNx2 = true;
        isText = false;
      }
    } else if (c == 0xab) {
      if (idx == 0) {
        isNx2 = true;
        isText = false;
      }
    } else if (c == '\"') {
      if (idx == 0) {
        isText = true;
        isNx2 = false;
      }
    } else {
      if (idx == 0) {
        isNx2 = false;
        isText = false;
      }
    }

    if ((c != '\n' || isNx2 || isText) && idx < MAX_BUFFER)
      pc_report_str[idx++] = c;

    if ((c == '\r' && !isNx2 && !isText) || (isNx2 && idx == 11) || (isText && c == '\r' && pc_report_str[idx - 2] == '\"')) {
      idx = 0;
      timeoutCnt = 0;
      parseLine(pc_report_str);
      if (!isText) sendReport((uint8_t*)&pc_report);
      memset(pc_report_str, 0, sizeof(pc_report_str));
    }
  }
  if (timeoutCnt > 1000) {
    idx = 0;
    timeoutCnt = 0;
  } else {
    if (timeoutCnt > 0) timeoutCnt++;
  }
}