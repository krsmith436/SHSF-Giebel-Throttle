/***************************************************************************
  Sketch Name: SHSF_Giebel_Throttle
  Written By: Kevin R. Smith
  Created: 2024-Feb-08

  This sketch is part of a DC control system for the Smith, Huotari & Santa Fe (SHSF) HO scale model railroad.
  Throttle features:
  - PWM control for track power
  - Wireless handheld keypad
  - Parametric display

  Hardware used:
  1) Arduino Uno Clone from Dylan's gift of Lafvintech Smart Car Kit
  2) RF keypad from Lynda and Mom's gift of Life-Like Radio Double Diesel train set
  3) 4N33 Optoisolator
  4) 128x64 OLED, SH1106

****************************************************************************
  Adafruit PCA9685 16-Channel Servo Driver
  Arduino Library Docs: http://adafruit.github.io/Adafruit-PWM-Servo-Driver-Library/html/class_adafruit___p_w_m_servo_driver.html
  (https://adafru.it/Au7)
****************************************************************************
*/
//-----------------Calling libraries needed to run-----------//
#include <Wire.h> // for I2C.
#include <SPI.h> // for OLED.
#include <Tweakly.h> // for non-blocking (no use of delay()) schedule of tasks and input processing.
#include "SHSF_Giebel_Throttle.h"
#include <Adafruit_PWMServoDriver.h> // for 16-Channel Servo Driver.
//
//------------------Object Instantiation------------------//
// U8g2 Contructor List (Picture Loop Page Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SH1106_128X64_NONAME_1_4W_SW_SPI u8g2(/* rotation=*/ U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 12, /* reset=*/ U8X8_PIN_NONE);
//
// Adafruit PCA9685 16-Channel Servo Driver
// Arduino Library Docs: http://adafruit.github.io/Adafruit-PWM-Servo-Driver-Library/html/class_adafruit___p_w_m_servo_driver.html
// (https://adafru.it/Au7)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PWM_I2C_ADDR);
//
//-------------------Tweakly---------------------//
TickTimer timerLogo;
TickTimer timerRefreshDisplay;
TickTimer timerTestModeSwitch;
inputHunter huntThrottleButtons;
inputHunter huntSoundButton;
doList listTestSlideSwitch;
//
//-----------------GLOBAL VARIABLES-------------------//
struct cab cabs[NUMBER_OF_CABS];
struct button soundButton;
bool blnLogoTimedOut = false; // variable for indicating the logo has timed out.
bool blnTestMode = false; // variable for indicating the Test/Operate slide switch position.
int intTestNumber = 0; // test number to run in Test mode.
//
void setup() {
  Serial.begin(COM_BAUD_RATE);
  //
  Serial.println(F("SH&SF - Giebel Throttle"));
  Serial.println(F("Starting setup."));
  //
  // Initialize display.
  u8g2.begin();
  dsplyLogo();
  //
  // Initialize I2C as Host.
  Wire.begin();
  //
  // Initialize PWM driver.
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(100);
  //
  // Initialize cabs.
  setupCabs();
  driveMotor(CAB_A);
  driveMotor(CAB_B);
  //
  // Initialize buttons.
  setupButtons();
  //
  // Initialize Tweakly.
  listTestSlideSwitch.addTask([]{ blnTestMode = testSlideSwitch.read(); });
  listTestSlideSwitch.addTask([]{ intTestNumber = (blnTestMode) ? intTestNumber:0; });
  timerLogo.attach(8000, []{ blnLogoTimedOut = true; }, DISPATCH_ONCE);
  timerRefreshDisplay.attach(500, dsplyValues);
  timerTestModeSwitch.attach(500, []{ listTestSlideSwitch.next(); });
  huntThrottleButtons.assign("throttle", handleThrottleButtons);
  huntSoundButton.assign("sound", handleSoundButton);
  //
  Serial.print(F("Setup is complete.\n"));
}

void loop() {
  //Call Tweakly forever
  TweaklyRun();
  huntThrottleButtons.recognition();
  huntSoundButton.recognition();
  //Put your code here :-)
  //
}
