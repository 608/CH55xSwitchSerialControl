#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

#define EMULATE_PROCON

#ifdef EMULATE_PROCON

// Battery levels
#define BATTERY_FULL        0x08
#define BATTERY_MEDIUM      0x06
#define BATTERY_LOW         0x04
#define BATTERY_CRITICAL    0x02
#define BATTERY_EMPTY       0x00
#define BATTERY_CHARGING    0x01 // Can be OR'ed

#define STICK_MIN 0x00
#define STICK_NEUTRAL 0x800
#define STICK_MAX 0xFFF

typedef enum {
    SUBCOMMAND_CONTROLLER_STATE_ONLY        = 0x00,
    SUBCOMMAND_BLUETOOTH_MANUAL_PAIRING     = 0x01,
    SUBCOMMAND_REQUEST_DEVICE_INFO          = 0x02,
    SUBCOMMAND_SET_INPUT_REPORT_MODE        = 0x03,
    SUBCOMMAND_TRIGGER_BUTTONS_ELAPSED_TIME = 0x04,
    SUBCOMMAND_GET_PAGE_LIST_STATE          = 0x05,
    SUBCOMMAND_SET_HCI_STATE                = 0x06,
    SUBCOMMAND_RESET_PAIRING_INFO           = 0x07,
    SUBCOMMAND_SET_SHIPMENT_LOW_POWER_STATE = 0x08,
    SUBCOMMAND_SPI_FLASH_READ               = 0x10,
    SUBCOMMAND_SPI_FLASH_WRITE              = 0x11,
    SUBCOMMAND_SPI_SECTOR_ERASE             = 0x12,
    SUBCOMMAND_RESET_NFC_IR_MCU             = 0x20,
    SUBCOMMAND_SET_NFC_IR_MCU_CONFIG        = 0x21,
    SUBCOMMAND_SET_NFC_IR_MCU_STATE         = 0x22,
    SUBCOMMAND_SET_PLAYER_LIGHTS            = 0x30,
    SUBCOMMAND_GET_PLAYER_LIGHTS            = 0x31,
    SUBCOMMAND_SET_HOME_LIGHTS              = 0x38,
    SUBCOMMAND_ENABLE_IMU                   = 0x40,
    SUBCOMMAND_SET_IMU_SENSITIVITY          = 0x41,
    SUBCOMMAND_WRITE_IMU_REGISTERS          = 0x42,
    SUBCOMMAND_READ_IMU_REGISTERS           = 0x43,
    SUBCOMMAND_ENABLE_VIBRATION             = 0x48,
    SUBCOMMAND_GET_REGULATED_VOLTAGE        = 0x50,
} Switch_Subcommand_t;

// https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/spi_flash_notes.md
typedef enum {
    ADDRESS_SERIAL_NUMBER         = 0x6000,
    ADDRESS_CONTROLLER_COLOR      = 0x6050,
    ADDRESS_FACTORY_PARAMETERS_1  = 0x6080,
    ADDRESS_FACTORY_PARAMETERS_2  = 0x6098,
    ADDRESS_FACTORY_CALIBRATION_1 = 0x6020,
    ADDRESS_FACTORY_CALIBRATION_2 = 0x603D,
    ADDRESS_STICKS_CALIBRATION    = 0x8010,
    ADDRESS_IMU_CALIBRATION       = 0x8028,
} SPI_Address_t;

// Standard input report sent to Switch (doesn't contain IMU data)
// Note that compilers can align and order bits in every byte however they want (endianness)
// Taken from https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md#standard-input-report-format
// The order in every byte is inverted
typedef //union {
    //uint8_t raw[11];
    struct {
        uint8_t connection_info: 4;
        uint8_t battery_level: 4;
        bool button_y: 1;
        bool button_x: 1;
        bool button_b: 1;
        bool button_a: 1;
        bool button_right_sr: 1;
        bool button_right_sl: 1;
        bool button_r: 1;
        bool button_zr: 1;
        bool button_minus: 1;
        bool button_plus: 1;
        bool button_thumb_r: 1;
        bool button_thumb_l: 1;
        bool button_home: 1;
        bool button_capture: 1;
        uint8_t dummy: 1;
        bool charging_grip: 1;
        bool dpad_down: 1;
        bool dpad_up: 1;
        bool dpad_right: 1;
        bool dpad_left: 1;
        bool button_left_sr: 1;
        bool button_left_sl: 1;
        bool button_l: 1;
        bool button_zl: 1;
        uint8_t analog[6];
        uint8_t vibrator_input_report;
    //};
} USB_StandardReport_t;

// Full (extended) input report sent to Switch, with IMU data
typedef //union {
    //uint8_t raw[29];
    struct {
        USB_StandardReport_t standardReport;
        int16_t imu[3 * 2 * 3]; // each axis is uint16_t, 3 axis per sensor, 2 sensors (accel and gyro), 3 reports
    //};
} USB_ExtendedReport_t;

#else
#define BUTTON_NONE 0x0000
#define BUTTON_Y 0x0001
#define BUTTON_B 0x0002
#define BUTTON_A 0x0004
#define BUTTON_X 0x0008
#define BUTTON_L 0x0010
#define BUTTON_R 0x0020
#define BUTTON_ZL 0x0040
#define BUTTON_ZR 0x0080
#define BUTTON_MINUS 0x0100
#define BUTTON_PLUS 0x0200
#define BUTTON_LCLICK 0x0400
#define BUTTON_RCLICK 0x0800
#define BUTTON_HOME 0x1000
#define BUTTON_CAPTURE 0x2000

#define HAT_UP 0x00
#define HAT_UP_RIGHT 0x01
#define HAT_RIGHT_UP 0x01
#define HAT_RIGHT 0x02
#define HAT_DOWN_RIGHT 0x03
#define HAT_RIGHT_DOWN 0x03
#define HAT_DOWN 0x04
#define HAT_DOWN_LEFT 0x05
#define HAT_LEFT_DOWN 0x05
#define HAT_LEFT 0x06
#define HAT_UP_LEFT 0x07
#define HAT_LEFT_UP 0x07
#define HAT_NEUTRAL 0x08

#define STICK_MIN 0x00
#define STICK_NEUTRAL 0x80
#define STICK_MAX 0xFF

typedef struct {
  uint16_t Button;
  uint8_t Hat;
  uint8_t LX;
  uint8_t LY;
  uint8_t RX;
  uint8_t RY;
  uint8_t VendorSpec;
} USB_JoystickReport_Input_t;
#endif

#define MOD_LEFT_CTRL (1 << 8)
#define MOD_LEFT_SHIFT (1 << 9)
#define MOD_LEFT_ALT (1 << 10)
#define MOD_LEFT_GUI (1 << 11)
#define MOD_RIGHT_CTRL (1 << 12)
#define MOD_RIGHT_SHIFT (1 << 13)
#define MOD_RIGHT_ALT (1 << 14)
#define MOD_RIGHT_GUI (uint16_t)(1 << 15)

#define KEY_RESERVED 0
#define KEY_ERROR_ROLLOVER 1
#define KEY_POST_FAIL 2
#define KEY_ERROR_UNDEFINED 3
#define KEY_A 4
#define KEY_B 5
#define KEY_C 6
#define KEY_D 7
#define KEY_E 8
#define KEY_F 9
#define KEY_G 10
#define KEY_H 11
#define KEY_I 12
#define KEY_J 13
#define KEY_K 14
#define KEY_L 15
#define KEY_M 16
#define KEY_N 17
#define KEY_O 18
#define KEY_P 19
#define KEY_Q 20
#define KEY_R 21
#define KEY_S 22
#define KEY_T 23
#define KEY_U 24
#define KEY_V 25
#define KEY_W 26
#define KEY_X 27
#define KEY_Y 28
#define KEY_Z 29
#define KEY_1 30
#define KEY_2 31
#define KEY_3 32
#define KEY_4 33
#define KEY_5 34
#define KEY_6 35
#define KEY_7 36
#define KEY_8 37
#define KEY_9 38
#define KEY_0 39
#define KEY_ENTER 40
#define KEY_RETURN 40  // Alias
#define KEY_ESC 41
#define KEY_BACKSPACE 42
#define KEY_TAB 43
#define KEY_SPACE 44
#define KEY_MINUS 45
#define KEY_EQUAL 46
#define KEY_LEFT_BRACE 47
#define KEY_RIGHT_BRACE 48
#define KEY_BACKSLASH 49
#define KEY_NON_US_NUM 50
#define KEY_SEMICOLON 51
#define KEY_QUOTE 52
#define KEY_TILDE 53
#define KEY_COMMA 54
#define KEY_PERIOD 55
#define KEY_SLASH 56
#define KEY_CAPS_LOCK 0x39
#define KEY_F1 0x3A
#define KEY_F2 0x3B
#define KEY_F3 0x3C
#define KEY_F4 0x3D
#define KEY_F5 0x3E
#define KEY_F6 0x3F
#define KEY_F7 0x40
#define KEY_F8 0x41
#define KEY_F9 0x42
#define KEY_F10 0x43
#define KEY_F11 0x44
#define KEY_F12 0x45
#define KEY_PRINT 0x46
#define KEY_PRINTSCREEN 0x46  // Alias
#define KEY_SCROLL_LOCK 0x47
#define KEY_PAUSE 0x48
#define KEY_INSERT 0x49
#define KEY_HOME 0x4A
#define KEY_PAGE_UP 0x4B
#define KEY_DELETE 0x4C
#define KEY_END 0x4D
#define KEY_PAGE_DOWN 0x4E
#define KEY_RIGHT_ARROW 0x4F
#define KEY_LEFT_ARROW 0x50
#define KEY_DOWN_ARROW 0x51
#define KEY_UP_ARROW 0x52
#define KEY_RIGHT 0x4F  // Alias
#define KEY_LEFT 0x50   // Alias
#define KEY_DOWN 0x51   // Alias
#define KEY_UP 0x52     // Alias
#define KEY_NUM_LOCK 0x53
#define KEYPAD_DIVIDE 0x54
#define KEYPAD_MULTIPLY 0x55
#define KEYPAD_SUBTRACT 0x56
#define KEYPAD_ADD 0x57
#define KEYPAD_ENTER 0x58
#define KEYPAD_1 0x59
#define KEYPAD_2 0x5A
#define KEYPAD_3 0x5B
#define KEYPAD_4 0x5C
#define KEYPAD_5 0x5D
#define KEYPAD_6 0x5E
#define KEYPAD_7 0x5F
#define KEYPAD_8 0x60
#define KEYPAD_9 0x61
#define KEYPAD_0 0x62
#define KEYPAD_DOT 0x63
#define KEY_NON_US 0x64
#define KEY_APPLICATION 0x65  // Context menu/right click
#define KEY_MENU 0x65         // Alias

// Most of the following keys will only work with Linux or not at all.
// F13+ keys are mostly used for laptop functions like ECO key.
#define KEY_POWER 0x66       // PowerOff (Ubuntu)
#define KEY_PAD_EQUALS 0x67  // Dont confuse with KEYPAD_EQUAL_SIGN
#define KEY_F13 0x68         // Tools (Ubunutu)
#define KEY_F14 0x69         // Launch5 (Ubuntu)
#define KEY_F15 0x6A         // Launch6 (Ubuntu)
#define KEY_F16 0x6B         // Launch7 (Ubuntu)
#define KEY_F17 0x6C         // Launch8 (Ubuntu)
#define KEY_F18 0x6D         // Launch9 (Ubuntu)
#define KEY_F19 0x6E         // Disabled (Ubuntu)
#define KEY_F20 0x6F         // AudioMicMute (Ubuntu)
#define KEY_F21 0x70         // Touchpad toggle (Ubuntu)
#define KEY_F22 0x71         // TouchpadOn (Ubuntu)
#define KEY_F23 0x72         // TouchpadOff Ubuntu)
#define KEY_F24 0x73         // Disabled (Ubuntu)
#define KEY_EXECUTE 0x74     // Open (Ubuntu)
#define KEY_HELP 0x75        // Help (Ubuntu)
#define KEY_MENU2 0x76       // Disabled (Ubuntu)
#define KEY_SELECT 0x77      // Disabled (Ubuntu)
#define KEY_STOP 0x78        // Cancel (Ubuntu)
#define KEY_AGAIN 0x79       // Redo (Ubuntu)
#define KEY_UNDO 0x7A        // Undo (Ubuntu)
#define KEY_CUT 0x7B         // Cut (Ubuntu)
#define KEY_COPY 0x7C        // Copy (Ubuntu)
#define KEY_PASTE 0x7D       // Paste (Ubuntu)
#define KEY_FIND 0x7E        // Find (Ubuntu)
#define KEY_MUTE 0x7F
#define KEY_VOLUME_MUTE 0x7F  // Alias
#define KEY_VOLUME_UP 0x80
#define KEY_VOLUME_DOWN 0x81
#define KEY_LOCKING_CAPS_LOCK 0x82    // Disabled (Ubuntu)
#define KEY_LOCKING_NUM_LOCK 0x83     // Disabled (Ubuntu)
#define KEY_LOCKING_SCROLL_LOCK 0x84  // Disabled (Ubuntu)
#define KEYPAD_COMMA 0x85             // .
#define KEYPAD_EQUAL_SIGN 0x86        // Disabled (Ubuntu) Dont confuse with KEYPAD_EQUAL
#define KEY_INTERNATIONAL1 0x87       // Disabled (Ubuntu)
#define KEY_INTERNATIONAL2 0x88       // Hiragana Katakana (Ubuntu)
#define KEY_INTERNATIONAL3 0x89       // Disabled (Ubuntu)
#define KEY_INTERNATIONAL4 0x8A       // Henkan (Ubuntu)
#define KEY_INTERNATIONAL5 0x8B       // Muhenkan (Ubuntu)
#define KEY_INTERNATIONAL6 0x8C       // Disabled (Ubuntu)
#define KEY_INTERNATIONAL7 0x8D       // Disabled (Ubuntu)
#define KEY_INTERNATIONAL8 0x8E       // Disabled (Ubuntu)
#define KEY_INTERNATIONAL9 0x8F       // Disabled (Ubuntu)
#define KEY_LANG1 0x90                // Disabled (Ubuntu)
#define KEY_LANG2 0x91                // Disabled (Ubuntu)
#define KEY_LANG3 0x92                // Katakana (Ubuntu)
#define KEY_LANG4 0x93                // Hiragana (Ubuntu)
#define KEY_LANG5 0x94                // Disabled (Ubuntu)
#define KEY_LANG6 0x95                // Disabled (Ubuntu)
#define KEY_LANG7 0x96                // Disabled (Ubuntu)
#define KEY_LANG8 0x97                // Disabled (Ubuntu)
#define KEY_LANG9 0x98                // Disabled (Ubuntu)
#define KEY_ALTERNATE_ERASE 0x99      // Disabled (Ubuntu)
#define KEY_SYSREQ_ATTENTION 0x9A     // Disabled (Ubuntu)
#define KEY_CANCEL 0x9B               // Disabled (Ubuntu)
#define KEY_CLEAR 0x9C                // Delete (Ubuntu)
#define KEY_PRIOR 0x9D                // Disabled (Ubuntu)
#define KEY_RETURN2 0x9E              // Disabled (Ubuntu) Do not confuse this with KEY_ENTER
#define KEY_SEPARATOR 0x9F            // Disabled (Ubuntu)
#define KEY_OUT 0xA0                  // Disabled (Ubuntu)
#define KEY_OPER 0xA1                 // Disabled (Ubuntu)
#define KEY_CLEAR_AGAIN 0xA2          // Disabled (Ubuntu)
#define KEY_CRSEL_PROPS 0xA3          // Disabled (Ubuntu)
#define KEY_EXSEL 0xA4                // Disabled (Ubuntu)

#define KEY_PAD_00 0xB0                // Disabled (Ubuntu)
#define KEY_PAD_000 0xB1               // Disabled (Ubuntu)
#define KEY_THOUSANDS_SEPARATOR 0xB2   // Disabled (Ubuntu)
#define KEY_DECIMAL_SEPARATOR 0xB3     // Disabled (Ubuntu)
#define KEY_CURRENCY_UNIT 0xB4         // Disabled (Ubuntu)
#define KEY_CURRENCY_SUB_UNIT 0xB5     // Disabled (Ubuntu)
#define KEYPAD_LEFT_BRACE 0xB6         // (
#define KEYPAD_RIGHT_BRACE 0xB7        // )
#define KEYPAD_LEFT_CURLY_BRACE 0xB8   // Disabled (Ubuntu)
#define KEYPAD_RIGHT_CURLY_BRACE 0xB9  // Disabled (Ubuntu)
#define KEYPAD_TAB 0xBA                // Disabled (Ubuntu)
#define KEYPAD_BACKSPACE 0xBB          // Disabled (Ubuntu)
#define KEYPAD_A 0xBC                  // Disabled (Ubuntu)
#define KEYPAD_B 0xBD                  // Disabled (Ubuntu)
#define KEYPAD_C 0xBE                  // Disabled (Ubuntu)
#define KEYPAD_D 0xBF                  // Disabled (Ubuntu)
#define KEYPAD_E 0xC0                  // Disabled (Ubuntu)
#define KEYPAD_F 0xC1                  // Disabled (Ubuntu)
#define KEYPAD_XOR 0xC2                // Disabled (Ubuntu)
#define KEYPAD_CARET 0xC3              // Disabled (Ubuntu)
#define KEYPAD_PERCENT 0xC4            // Disabled (Ubuntu)
#define KEYPAD_LESS_THAN 0xC5          // Disabled (Ubuntu)
#define KEYPAD_GREATER_THAN 0xC6       // Disabled (Ubuntu)
#define KEYPAD_AMPERSAND 0xC7          // Disabled (Ubuntu)
#define KEYPAD_DOUBLEAMPERSAND 0xC8    // Disabled (Ubuntu)
#define KEYPAD_PIPE 0xC9               // Disabled (Ubuntu)
#define KEYPAD_DOUBLEPIPE 0xCA         // Disabled (Ubuntu)
#define KEYPAD_COLON 0xCB              // Disabled (Ubuntu)
#define KEYPAD_POUND_SIGN 0xCC         // Disabled (Ubuntu)
#define KEYPAD_SPACE 0xCD              // Disabled (Ubuntu)
#define KEYPAD_AT_SIGN 0xCE            // Disabled (Ubuntu)
#define KEYPAD_EXCLAMATION_POINT 0xCF  // Disabled (Ubuntu)
#define KEYPAD_MEMORY_STORE 0xD0       // Disabled (Ubuntu)
#define KEYPAD_MEMORY_RECALL 0xD1      // Disabled (Ubuntu)
#define KEYPAD_MEMORY_CLEAR 0xD2       // Disabled (Ubuntu)
#define KEYPAD_MEMORY_ADD 0xD3         // Disabled (Ubuntu)
#define KEYPAD_MEMORY_SUBTRACT 0xD4    // Disabled (Ubuntu)
#define KEYPAD_MEMORY_MULTIPLY 0xD5    // Disabled (Ubuntu)
#define KEYPAD_MEMORY_DIVIDE 0xD6      // Disabled (Ubuntu)
#define KEYPAD_PLUS_MINUS 0xD7         // Disabled (Ubuntu)
#define KEYPAD_CLEAR 0xD8              // Delete (Ubuntu)
#define KEYPAD_CLEAR_ENTRY 0xD9        // Disabled (Ubuntu)
#define KEYPAD_BINARY 0xDA             // Disabled (Ubuntu)
#define KEYPAD_OCTAL 0xDB              // Disabled (Ubuntu)
#define KEYPAD_DECIMAL 0xDC            // Disabled (Ubuntu)
#define KEYPAD_HEXADECIMAL 0xDD        // Disabled (Ubuntu)

#define KEY_LEFT_CTRL 0xE0
#define KEY_LEFT_SHIFT 0xE1
#define KEY_LEFT_ALT 0xE2
#define KEY_LEFT_GUI 0xE3
#define KEY_LEFT_WINDOWS 0xE3  // Alias
#define KEY_RIGHT_CTRL 0xE4
#define KEY_RIGHT_SHIFT 0xE5
#define KEY_RIGHT_ALT 0xE6
#define KEY_RIGHT_GUI 0xE7
#define KEY_RIGHT_WINDOWS 0xE7  // Alias

#define KEY_JP_COLON        KEY_NON_US
#define KEY_JP_AT           KEY_NON_US
#define KEY_JP_CARET        KEY_NON_US

#define KEY_JP_HANZEN       KEY_TILDE
#define KEY_JP_KANJI        KEY_TILDE // Alias
#define KEY_JP_BACKSLASH    KEY_INTERNATIONAL1
#define KEY_JP_HIRAGANA     KEY_INTERNATIONAL2
#define KEY_JP_YEN          KEY_INTERNATIONAL3
#define KEY_JP_HENKAN       KEY_INTERNATIONAL4
#define KEY_JP_MUHENKAN     KEY_INTERNATIONAL5

__code uint16_t _asciimap[256] = {
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

  // The following characters belong to ISO-8859-15
  KEY_RESERVED,     // 128 - Unused
  KEY_RESERVED,     // 129 - Unused
  KEY_RESERVED,     // 130 - Unused
  KEY_RESERVED,     // 131 - Unused
  KEY_RESERVED,     // 132 - Unused
  KEY_RESERVED,     // 133 - Unused
  KEY_RESERVED,     // 134 - Unused
  KEY_RESERVED,     // 135 - Unused
  KEY_JP_HIRAGANA,  // 136 - Unused
  KEY_RESERVED,     // 137 - Unused
  KEY_RESERVED,     // 138 - Unused
  KEY_RESERVED,     // 139 - Unused
  KEY_RESERVED,     // 140 - Unused
  KEY_RESERVED,     // 141 - Unused
  KEY_RESERVED,     // 142 - Unused
  KEY_RESERVED,     // 143 - Unused
  KEY_RESERVED,     // 144 - Unused
  KEY_RESERVED,     // 145 - Unused
  KEY_RESERVED,     // 146 - Unused
  KEY_RESERVED,     // 147 - Unused
  KEY_RESERVED,     // 148 - Unused
  KEY_RESERVED,     // 149 - Unused
  KEY_RESERVED,     // 150 - Unused
  KEY_RESERVED,     // 151 - Unused
  KEY_RESERVED,     // 152 - Unused
  KEY_RESERVED,     // 153 - Unused
  KEY_RESERVED,     // 154 - Unused
  KEY_RESERVED,     // 155 - Unused
  KEY_RESERVED,     // 156 - Unused
  KEY_RESERVED,     // 157 - Unused
  KEY_RESERVED,     // 158 - Unused
  KEY_RESERVED,     // 159 - Unused
  KEY_RESERVED,     // 160 - Non-breaking Space
  KEY_RESERVED,     // 161 - Inverted Exclamation Mark
  KEY_RESERVED,     // 162 - Cent
  KEY_RESERVED,     // 163 - British Pound Sign
  KEY_RESERVED,     // 164 - Euro Sign
  KEY_RESERVED,     // 165 - Yen
  KEY_RESERVED,     // 166 - Capital 's' Inverted Circumflex
  KEY_RESERVED,     // 167 - Section Sign
  KEY_RESERVED,     // 168 - 's' Inverted Circumflex
  KEY_RESERVED,     // 169 - Copyright Sign
  KEY_RESERVED,     // 170 - Superscript 'a'
  KEY_RESERVED,     // 171 - Open Guillemet
  KEY_RESERVED,     // 172 - Logic Negation
  KEY_RESERVED,     // 173 - Soft Hypen
  KEY_RESERVED,     // 174 - Registered Trademark
  KEY_RESERVED,     // 175 - Macron
  KEY_RESERVED,     // 176 - Degree Symbol
  KEY_RESERVED,     // 177 - Plus-Minus
  KEY_RESERVED,     // 178 - Superscript '2'
  KEY_RESERVED,     // 179 - Superscript '3'
  KEY_RESERVED,     // 180 - Capital 'z' Inverted Circumflex
  KEY_RESERVED,     // 181 - Micro Symbol
  KEY_RESERVED,     // 182 - Paragraph Mark
  KEY_RESERVED,     // 183 - Interpunct
  KEY_RESERVED,     // 184 - 'z' Inverted Circumflex
  KEY_RESERVED,     // 185 - Superscript '1'
  KEY_RESERVED,     // 186 - Ordinal Indicator
  KEY_RESERVED,     // 187 - Closed Guillemet
  KEY_RESERVED,     // 188 - Capital 'oe'
  KEY_RESERVED,     // 189 - 'oe'
  KEY_RESERVED,     // 190 - Capital 'y' Umlaut
  KEY_RESERVED,     // 191 - Inverted Question Mark
  KEY_RESERVED,     // 192 - Capital 'a' Grave
  KEY_RESERVED,     // 193 - Capital 'a' Acute
  KEY_RESERVED,     // 194 - Capital 'a' Circumflex
  KEY_RESERVED,     // 195 - Capital 'a' Tilde
  KEY_RESERVED,     // 196 - Capital 'a' Umlaut
  KEY_RESERVED,     // 197 - Capital 'a' Circle
  KEY_RESERVED,     // 198 - Capital 'ae'
  KEY_RESERVED,     // 199 - Capital 'c' Cedilla
  KEY_RESERVED,     // 200 - Capital 'e' Grave
  KEY_RESERVED,     // 201 - Capital 'e' Acute
  KEY_RESERVED,     // 202 - Capital 'e' Circumflex
  KEY_RESERVED,     // 203 - Capital 'e' Umlaut
  KEY_RESERVED,     // 204 - Capital 'i' Grave
  KEY_RESERVED,     // 205 - Capital 'i' Acute
  KEY_RESERVED,     // 206 - Capital 'i' Circumflex
  KEY_RESERVED,     // 207 - Capital 'i' Umlaut
  KEY_RESERVED,     // 208 - Capital Eth
  KEY_RESERVED,     // 207 - Capital 'n' Tilde
  KEY_RESERVED,     // 210 - Capital 'o' Grave
  KEY_RESERVED,     // 211 - Capital 'o' Acute
  KEY_RESERVED,     // 212 - Capital 'o' Circumflex
  KEY_RESERVED,     // 213 - Capital 'o' Tilde
  KEY_RESERVED,     // 214 - Capital 'o' Umlaut
  KEY_RESERVED,     // 215 - Multiplication Sign
  KEY_RESERVED,     // 216 - Capital 'o' Barred
  KEY_RESERVED,     // 217 - Capital 'u' Grave
  KEY_RESERVED,     // 218 - Capital 'u' Acute
  KEY_RESERVED,     // 219 - Capital 'u' Circumflex
  KEY_RESERVED,     // 220 - Capital 'u' Umlaut
  KEY_RESERVED,     // 221 - Capital 'y' Acute
  KEY_RESERVED,     // 222 - Capital Thorn
  KEY_RESERVED,     // 223 - Eszett
  KEY_RESERVED,     // 224 - 'a' Grave
  KEY_RESERVED,     // 225 - 'a' Acute
  KEY_RESERVED,     // 226 - 'a' Circumflex
  KEY_RESERVED,     // 227 - 'a' Tilde
  KEY_RESERVED,     // 228 - 'a' Umlaut
  KEY_RESERVED,     // 229 - 'a' Circle
  KEY_RESERVED,     // 230 - 'ae'
  KEY_RESERVED,     // 231 - 'c' Cedilla
  KEY_RESERVED,     // 232 - 'e' Grave
  KEY_RESERVED,     // 233 - 'e' Acute
  KEY_RESERVED,     // 234 - 'e' Circumflex
  KEY_RESERVED,     // 235 - 'e' Umlaut
  KEY_RESERVED,     // 236 - 'i' Grave
  KEY_RESERVED,     // 237 - 'i' Acute
  KEY_RESERVED,     // 238 - 'i' Circumflex
  KEY_RESERVED,     // 239 - 'i' Umlaut
  KEY_RESERVED,     // 240 - Eth
  KEY_RESERVED,     // 241 - 'n' Tilde
  KEY_RESERVED,     // 242 - 'o' Grave
  KEY_RESERVED,     // 243 - 'o' Acute
  KEY_RESERVED,     // 244 - 'o' Circumflex
  KEY_RESERVED,     // 245 - 'o' Tilde
  KEY_RESERVED,     // 246 - 'o' Umlaut
  KEY_RESERVED,     // 247 - Multiplication Sign
  KEY_RESERVED,     // 248 - 'o' Barred
  KEY_RESERVED,     // 249 - 'u' Grave
  KEY_RESERVED,     // 250 - 'u' Acute
  KEY_RESERVED,     // 251 - 'u' Circumflex
  KEY_RESERVED,     // 252 - 'u' Umlaut
  KEY_RESERVED,     // 253 - 'y' Acute
  KEY_RESERVED,     // 254 - Thorn
  KEY_RESERVED,     // 255 - 'y' Umlaut
};

void USBInit(void);
#ifdef EMULATE_PROCON
void sendReport(void);
extern USB_ExtendedReport_t pc_report;
#else
void sendReport(uint8_t* p);
#endif
void pressKey(uint8_t c);
void releaseKey(uint8_t c);
void pressSpecialKey(uint8_t c);
void releaseSpecialKey(uint8_t c);
void pushKey(uint8_t c);
void releaseAllKey(void);
