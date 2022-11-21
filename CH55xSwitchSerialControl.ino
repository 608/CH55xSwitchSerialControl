#include <Serial.h>
#include "HIDGamepad.h"
#define MAX_BUFFER 32

__xdata char pc_report_str[MAX_BUFFER];
__xdata uint16_t idx = 0;

__xdata uint8_t pc_lx, pc_ly, pc_rx, pc_ry;

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
  __xdata uint16_t p_btns;
  __xdata uint8_t hat;

  if (strncmp(line, "end", 3) == 0) {
    // pokecon
    resetDirections();
  } else if (strncmp(line, "RELEASE", 7) == 0) {
    // nx ver 1.0
    resetDirections();
  } else if (strncmp(line, "Button", 6) == 0) {
    // nx ver 1.0
    pc_report.Button = BUTTON_A;
    if (strncmp(&line[7], "SELECT", 6) == 0) {
      pc_report.Button = BUTTON_MINUS;
    } else if (strncmp(&line[7], "START", 5) == 0) {
      pc_report.Button = BUTTON_PLUS;
    } else if (strncmp(&line[7], "LCLICK", 6) == 0) {
      pc_report.Button = BUTTON_LCLICK;
    } else if (strncmp(&line[7], "RCLICK", 6) == 0) {
      pc_report.Button = BUTTON_RCLICK;
    } else if (strncmp(&line[7], "HOME", 4) == 0) {
      pc_report.Button = BUTTON_HOME;
    } else if (strncmp(&line[7], "CAPTURE", 7) == 0) {
      pc_report.Button = BUTTON_CAPTURE;
    } else if (strncmp(&line[7], "RELEASE", 7) == 0) {
      pc_report.Button = BUTTON_NONE;
    } else if (strncmp(&line[7], "A", 1) == 0) {
      pc_report.Button = BUTTON_A;
    } else if (strncmp(&line[7], "B", 1) == 0) {
      pc_report.Button = BUTTON_B;
    } else if (strncmp(&line[7], "X", 1) == 0) {
      pc_report.Button = BUTTON_X;
    } else if (strncmp(&line[7], "Y", 1) == 0) {
      pc_report.Button = BUTTON_Y;
    } else if (strncmp(&line[7], "L", 1) == 0) {
      pc_report.Button = BUTTON_L;
    } else if (strncmp(&line[7], "R", 1) == 0) {
      pc_report.Button = BUTTON_R;
    } else if (strncmp(&line[7], "ZL", 2) == 0) {
      pc_report.Button = BUTTON_ZL;
    } else if (strncmp(&line[7], "ZR", 2) == 0) {
      pc_report.Button = BUTTON_ZR;
    }
  } else if (strncmp(line, "HAT", 3) == 0) {
    // nx ver 1.0
    if (strncmp(&line[4], "CENTER", 6) == 0) {
      pc_report.Hat = HAT_NEUTRAL;
    } else if (strncmp(&line[4], "TOP_RIGHT", 9) == 0) {
      pc_report.Hat = HAT_UP_RIGHT;
    } else if (strncmp(&line[4], "RIGHT", 5) == 0) {
      pc_report.Hat = HAT_RIGHT;
    } else if (strncmp(&line[4], "BOTTOM_RIGHT", 12) == 0) {
      pc_report.Hat = HAT_DOWN_RIGHT;
    } else if (strncmp(&line[4], "BOTTOM_LEFT", 11) == 0) {
      pc_report.Hat = HAT_DOWN_LEFT;
    } else if (strncmp(&line[4], "BOTTOM", 6) == 0) {
      pc_report.Hat = HAT_DOWN;
    } else if (strncmp(&line[4], "LEFT", 16) == 4) {
      pc_report.Hat = HAT_LEFT;
    } else if (strncmp(&line[4], "TOP_LEFT", 8) == 0) {
      pc_report.Hat = HAT_UP_LEFT;
    } else if (strncmp(&line[4], "TOP", 3) == 0) {
      pc_report.Hat = HAT_UP;
    }
  } else if (strncmp(line, "LX", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      pc_report.LX = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      pc_report.LX = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      pc_report.LX = STICK_NEUTRAL;
    }
  } else if (strncmp(line, "LY", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      pc_report.LY = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      pc_report.LY = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      pc_report.LY = STICK_NEUTRAL;
    }
  } else if (strncmp(line, "RX", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      pc_report.RX = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      pc_report.RX = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      pc_report.RX = STICK_NEUTRAL;
    }
  } else if (strncmp(line, "RY", 2) == 0) {
    // nx ver 1.0
    if (strncmp(&line[3], "MIN", 3) == 0) {
      pc_report.RY = STICK_MIN;
    } else if (strncmp(&line[3], "MAX", 3) == 0) {
      pc_report.RY = STICK_MAX;
    } else if (strncmp(&line[3], "CENTER", 6) == 0) {
      pc_report.RY = STICK_NEUTRAL;
    }
  } else if (line[0] == 0xaa) {
    // nx2
    pc_report.Button = line[5] | (line[6] << 8);
    pc_report.Hat = line[7];
    pc_report.LX = STICK_NEUTRAL;
    pc_report.LY = STICK_NEUTRAL;
    pc_report.RX = STICK_NEUTRAL;
    pc_report.RY = STICK_NEUTRAL;
    if (line[8] & 1) pc_report.LX = STICK_MIN;
    if (line[8] & 2) pc_report.LX = STICK_MAX;
    if (line[8] & 4) pc_report.LY = STICK_MIN;
    if (line[8] & 8) pc_report.LY = STICK_MAX;
    if (line[9] & 1) pc_report.RX = STICK_MIN;
    if (line[9] & 2) pc_report.RX = STICK_MAX;
    if (line[9] & 4) pc_report.RY = STICK_MIN;
    if (line[9] & 8) pc_report.RY = STICK_MAX;
  } else if (line[0] >= '0' && line[0] <= '9') {
    // pokecon or nx2 ver2.08
    __xdata uint8_t char_pos = 0;

    p_btns = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      p_btns *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        p_btns += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        p_btns += (line[char_pos] - 'A' + 10);
      } else {
        p_btns += (line[char_pos] - 'a' + 10);
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

    while (line[char_pos] != '\r') {
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

    // HAT : 0(TOP) to 7(TOP_LEFT) in clockwise | 8(NEUTRAL)
    pc_report.Hat = hat;

    // we use bit array for buttons(2 Bytes), which last 2 bits are flags of directions
    __xdata bool use_right = p_btns & 0x1;
    __xdata bool use_left = p_btns & 0x2;

    // Left stick
    if (use_left) {
      pc_report.LX = pc_lx;
      pc_report.LY = pc_ly;
    }

    // Right stick
    if (use_right & use_left) {
      pc_report.RX = pc_rx;
      pc_report.RY = pc_ry;
    } else if (use_right) {
      pc_report.RX = pc_lx;
      pc_report.RY = pc_ly;
    }

    p_btns >>= 2;
    pc_report.Button = p_btns;
  }
}

void setup() {
  USBInit();
  Serial0_begin(9600);
  resetDirections();
}

__xdata bool isNx2 = false;

void loop() {
  while (Serial0_available()) {
    char c = Serial0_read();

    if (c == 0xaa) {
      isNx2 = true;
    }

    if ((c != '\n' || isNx2) && idx < MAX_BUFFER)
      pc_report_str[idx++] = c;

    if (c == '\r' || (isNx2 && idx == 11)) {
      idx = 0;
      parseLine(pc_report_str);
      sendReport((uint8_t*)&pc_report);
      memset(pc_report_str, 0, sizeof(pc_report_str));
    }
  }
}