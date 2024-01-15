#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

#define STICK_MIN 0x00
#define STICK_NEUTRAL 0x80
#define STICK_MAX 0xFF

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

typedef struct {
  uint16_t Button;
  uint8_t Hat;
  uint8_t LX;
  uint8_t LY;
  uint8_t RX;
  uint8_t RY;
  uint8_t VendorSpec;
} USB_JoystickReport_Input_t;

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

void USBInit(void);
void sendReport(uint8_t* p);
void pressKey(uint8_t c);
void releaseKey(uint8_t c);
void pressSpecialKey(uint8_t c);
void releaseSpecialKey(uint8_t c);
void pushKey(uint8_t c);
void pushSpecialKey(uint8_t c);
void releaseAllKey(void);
