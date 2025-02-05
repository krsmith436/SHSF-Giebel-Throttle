// all # define options should be defined here,  this keeps them together in a nice way.
//
const uint8_t NUMBER_OF_CABS = 2;
const uint8_t CAB_A = 0; // array index for cabs.
const uint8_t CAB_B = 1; // array index for cabs.
const uint8_t STOP = 0;
const uint8_t FORWARD = 1;
const uint8_t REVERSE = 2;
const uint8_t EAST = 1;
const uint8_t WEST = 2;
const uint8_t ENABLE = 0; // array index for PWM pins.
const uint8_t IN1 = 1; // array index for PWM pins.
const uint8_t IN2 = 2; // array index for PWM pins.
//
struct cab {
  char engineNumber[5]; // engine number; 4 characters max, +1 for NULL character.
  char roadAbbreviation[5]; // road abbreviation; 4 characters max, +1 for NULL character.
  int throttle; // train’s speed in the range from -4095 to 4095.
  int stepValue; // amount to increment/decrement train’s speed.
  int minForward; // minimum speed the train needs to start going forward.
  int maxForward; // maximum speed the train will reach going forward.
  int minReverse; // minimum speed the train needs to start going reverse.
  int maxReverse; // maximum speed the train will reach going reverse.
  uint8_t dir; // direction of train’s movement, STOP, FORWARD or REVERSE.
  uint8_t engineFacing; // direction the train's engine is facing, STOP, EAST or WEST.
  uint8_t buttonToPin[3]; // array of pin numbers for STOP, FORWARD and REVERSE.
  unsigned long buttonPreviousMillis[3]; // array of time since last button press for STOP, FORWARD and REVERSE.
  unsigned long buttonInterval; // interval at which to look for new press (de-bounce).
  uint8_t L298ToPin[3]; // array of L298 pin numbers for ENABLE, IN1 and IN2.
};
//
struct button {
  unsigned long previousMillis; // time since last button press.
  unsigned long interval; // interval at which to look for new press (de-bounce).
};
//--------------------- I2C Addresses ----------------------
// TINY_CODE_READER_I2C_ADDRESS (0x0c), defined in z_QR_Code_Reader.h
const uint8_t PWM_I2C_ADDR = 0x41;
const uint8_t SOUND_I2C_ADDR = 0x09; // External device, Smiths Valley address.
//
//---------------------- Create pads -----------------------
// OLED pins 10, 11, 12 and 13, DO NOT USE!!!
  Pad stopbuttonA(6, INPUT, "throttle");
  Pad forwardbuttonA(7, INPUT, "throttle");
  Pad reversebuttonA(8, INPUT, "throttle");
  Pad soundbuttonA(9, INPUT, "sound");
  //
  Pad stopbuttonB(3, INPUT, "throttle");
  Pad forwardbuttonB(4, INPUT, "throttle");
  Pad reversebuttonB(5, INPUT, "throttle");
  //
  Pad testSlideSwitch(2, INPUT_PULLUP);
//
//-------------------- Pin assignments ---------------------
const uint8_t TEST_ANALOG_INPUT = A1;
//
//---------------- PWM Channel assignments -----------------
const uint8_t L298_ENA = 0;
const uint8_t L298_IN1 = 1;
const uint8_t L298_IN2 = 2;
const uint8_t L298_IN3 = 3;
const uint8_t L298_IN4 = 4;
const uint8_t L298_ENB = 5;
//
// for communications
const unsigned long COM_BAUD_RATE = 115200;
//
// for OLED
// Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
// Reference https://github.com/olikraus/u8g2/wiki/u8g2reference
#include <U8g2lib.h>
//
// 'shsf_logo_black_64x61.xbm'
#define LOGO_WIDTH 64
#define LOGO_HEIGHT 61
static const unsigned char shsf_logo_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x86, 0xCF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x8E, 
  0xD8, 0x20, 0x00, 0x00, 0x00, 0x00, 0x38, 0x8E, 0x4F, 0xF0, 0x00, 0x00, 
  0x00, 0x00, 0x3C, 0x9E, 0x44, 0x90, 0x00, 0x00, 0x00, 0x00, 0x20, 0xB2, 
  0x6D, 0xB8, 0x01, 0x00, 0x00, 0xC0, 0x63, 0x22, 0x28, 0x64, 0x00, 0x00, 
  0x00, 0x60, 0x46, 0x00, 0x00, 0xCC, 0x01, 0x00, 0x00, 0x20, 0x44, 0x00, 
  0x00, 0xFC, 0x01, 0x00, 0x00, 0x60, 0x04, 0x00, 0x00, 0x60, 0x60, 0x00, 
  0x00, 0xCC, 0x07, 0x00, 0x00, 0x40, 0xF0, 0x00, 0x00, 0x98, 0x01, 0x00, 
  0x00, 0x00, 0xB0, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x36, 0x01, 
  0x80, 0x21, 0x00, 0x00, 0x00, 0x00, 0x64, 0x03, 0x00, 0x23, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x6C, 0x00, 0x20, 0x1E, 0xFF, 0x1F, 0xF0, 0xFF, 0x78, 0x00, 
  0xF0, 0x08, 0xFF, 0xE3, 0xCF, 0xFF, 0x10, 0x00, 0xC0, 0x03, 0xFF, 0xFC, 
  0x3F, 0xFF, 0x80, 0x05, 0xC0, 0x07, 0x7F, 0xF7, 0xDF, 0xFC, 0xE0, 0x07, 
  0x40, 0x00, 0xBF, 0xF1, 0x9F, 0xF9, 0x80, 0x02, 0x44, 0x00, 0xDF, 0xF0, 
  0x1F, 0xF6, 0x00, 0x01, 0x7C, 0x00, 0x6F, 0xF0, 0x1F, 0xF6, 0xC0, 0x01, 
  0xF0, 0x01, 0x37, 0xF0, 0x1F, 0xEC, 0x80, 0x10, 0x00, 0x01, 0x17, 0xF0, 
  0x1F, 0xD8, 0x00, 0x1F, 0x00, 0x00, 0x1B, 0xF0, 0x1F, 0xD8, 0x80, 0x09, 
  0x00, 0x00, 0xFB, 0xFF, 0xFF, 0xDF, 0x00, 0x0C, 0x00, 0x00, 0xFB, 0xFF, 
  0xFF, 0xBF, 0x00, 0x06, 0xFC, 0x00, 0x8D, 0xFF, 0x3F, 0xBC, 0x00, 0x3F, 
  0xF0, 0x01, 0xED, 0x6D, 0xB2, 0xBB, 0x00, 0x2F, 0x20, 0x00, 0x8D, 0x8A, 
  0x36, 0xB1, 0x00, 0x00, 0x20, 0x00, 0xBD, 0x98, 0x72, 0xB1, 0x00, 0x20, 
  0xF8, 0x00, 0x85, 0xA8, 0x22, 0xB9, 0x00, 0x37, 0xBC, 0x00, 0xFD, 0xFF, 
  0xFF, 0xBF, 0x00, 0x3F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xBF, 0x00, 0x20, 
  0x08, 0x00, 0xFB, 0xFB, 0x7F, 0xDD, 0x80, 0x30, 0xC8, 0x01, 0x1B, 0xF0, 
  0x1F, 0xD0, 0x80, 0x03, 0xF8, 0x00, 0x17, 0xF0, 0x1F, 0xD8, 0x00, 0x07, 
  0x18, 0x00, 0x37, 0xF0, 0x1F, 0xEC, 0x00, 0x0D, 0x10, 0x00, 0x6F, 0xF0, 
  0x1F, 0xF4, 0xC0, 0x1F, 0x90, 0x03, 0xDF, 0xF0, 0x1F, 0xF7, 0x80, 0x01, 
  0xE0, 0x01, 0x9F, 0xF1, 0x9F, 0xFB, 0x00, 0x00, 0x20, 0x04, 0x7F, 0xF7, 
  0xFF, 0xFC, 0x00, 0x00, 0x80, 0x07, 0xFF, 0xFC, 0x3F, 0xFF, 0x10, 0x00, 
  0xC0, 0x09, 0xFF, 0xF3, 0xC7, 0xFF, 0x30, 0x00, 0xC0, 0x0F, 0xFF, 0x0F, 
  0xF8, 0xFF, 0xE0, 0x00, 0x00, 0x34, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x03, 
  0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x60, 0x03, 0x00, 0xCE, 0x00, 0x00, 
  0x00, 0x00, 0x31, 0x03, 0x00, 0xE6, 0x01, 0x00, 0x00, 0x80, 0x83, 0x01, 
  0x00, 0x20, 0x03, 0x00, 0x00, 0x60, 0xC6, 0x00, 0x00, 0x68, 0x06, 0x00, 
  0x00, 0x20, 0x4F, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x80, 0x0D, 0x00, 
  0x00, 0x98, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0xF0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
