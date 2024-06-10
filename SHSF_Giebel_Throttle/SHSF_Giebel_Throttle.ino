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
#include <Wire.h> // for Inter-Integrated Circuit (I2C).
#include <SPI.h> // for Organic Light Emitting Diode (OLED).
#include "RTC.h" // for Real Time Clock (RTC).
#include <NTPClient.h> // for Network Time Protocol (NTP).
#include <WiFiS3.h> // for Wireless Fidelity (WiFi).
#include <WiFiUdp.h> // for Wireless Fidelity (WiFi).
#include "Arduino_Secrets.h" // for Wireless Fidelity (WiFi).
#include <Adafruit_PWMServoDriver.h> // for 16-Channel Servo Driver.
#include <Tweakly.h> // for non-blocking (no use of delay()) schedule of tasks and input processing.
#include "SHSF_Giebel_Throttle.h"
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
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);
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
bool blnRtcBbatteryFault = false; // flag to replace the internal 3.3V battery for Real Time Clock (RTC).
///////please enter your sensitive data in the Secret tab/Arduino_Secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int wifiStatus = WL_IDLE_STATUS;
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
  // Initialize Real Time Clock.
  RTC.begin();
  RTCTime myTime;
  RTC.getTime(myTime);
  //
  if (!RTC.isRunning()) {
    // this means the RTC is waking up "as new"
    if (myTime.getYear() == 2000) {
      blnRtcBbatteryFault = true;
    } else {
      blnRtcBbatteryFault = false;
    }
    // Get the current date and time from an NTP server and convert.
    connectToWiFi();
    Serial.println(F("\nStarting connection to time server..."));
    timeClient.begin();
    timeClient.update();
    auto timeZoneOffsetHours = -5;
    auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
    Serial.print(F("Unix time = "));
    Serial.println(unixTime);
    myTime = RTCTime(unixTime);
    RTC.setTime(myTime);
  }
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
  timerLogo.attach(5000, []{ blnLogoTimedOut = true; }, DISPATCH_ONCE);
  timerRefreshDisplay.attach(1000, dsplyValues);
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
