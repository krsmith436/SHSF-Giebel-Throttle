void driveMotor(int intCab) {
  // Constrain throttle value to account for max values not being a factor of the step value.
  cabs[intCab].throttle = constrain(cabs[intCab].throttle, cabs[intCab].maxReverse, cabs[intCab].maxForward);
  //
  // Set direction.
  if (cabs[intCab].throttle == 0) {cabs[intCab].dir = STOP;}
  if (cabs[intCab].throttle > 0) {cabs[intCab].dir = FORWARD;}
  if (cabs[intCab].throttle < 0) {cabs[intCab].dir = REVERSE;}
  //
  // Set track direction based on engine facing direction.
  uint8_t trackDirection = cabs[intCab].dir;
  if ((cabs[intCab].dir == FORWARD) && (cabs[intCab].engineFacing == WEST)) {
    trackDirection = REVERSE;
  }
  if ((cabs[intCab].dir == REVERSE) && (cabs[intCab].engineFacing == WEST)) {
    trackDirection = FORWARD;
  }
  // Drive motor.
  switch (trackDirection) {
    case STOP:
      pwm.setPin(cabs[intCab].L298ToPin[IN1], 0); // Turns pin fully off.
Serial.print("Setting STOP PWM ");
Serial.print(cabs[intCab].L298ToPin[IN1]);
Serial.println(": OFF");
      pwm.setPin(cabs[intCab].L298ToPin[IN2], 0); // Turns pin fully off.
Serial.print("Setting PWM ");
Serial.print(cabs[intCab].L298ToPin[IN2]);
Serial.println(": OFF");
      pwm.setPWM(cabs[intCab].L298ToPin[ENABLE], 0, 4096); // Set PWM fully off.
Serial.print("Setting PWM ");
Serial.print(cabs[intCab].L298ToPin[ENABLE]);
Serial.println(": OFF");
      break;
    case FORWARD:
      pwm.setPWM(cabs[intCab].L298ToPin[IN1], 4096, 0); // Set PWM fully on.
Serial.print("Setting FORWARD PWM ");
Serial.print(cabs[intCab].L298ToPin[IN1]);
Serial.println(": ON");
      pwm.setPWM(cabs[intCab].L298ToPin[IN2], 0, 4096); // Set PWM fully off.
Serial.print("Setting PWM ");
Serial.print(cabs[intCab].L298ToPin[IN2]);
Serial.println(": OFF");
      pwm.setPWM(cabs[intCab].L298ToPin[ENABLE], 0, cabs[intCab].throttle); // Set PWM.
Serial.print("Setting PWM ");
Serial.print(cabs[intCab].L298ToPin[ENABLE]);
Serial.print(": ");
Serial.println(cabs[intCab].throttle);
      break;
    case REVERSE:
      pwm.setPin(cabs[intCab].L298ToPin[IN1], 0); // Turns pin fully off.
Serial.print("Setting REVERSE PWM ");
Serial.print(cabs[intCab].L298ToPin[IN1]);
Serial.println(": OFF");
      pwm.setPin(cabs[intCab].L298ToPin[IN2], 4096); // Turns pin fully on.
Serial.print("Setting PWM ");
Serial.print(cabs[intCab].L298ToPin[IN2]);
Serial.println(": ON");
      pwm.setPWM(cabs[intCab].L298ToPin[ENABLE], 0, abs(cabs[intCab].throttle)); // Set PWM.
Serial.print("Setting PWM ");
Serial.print(cabs[intCab].L298ToPin[ENABLE]);
Serial.print(": ");
Serial.println(cabs[intCab].throttle);
      break;
  }
}
//
//
void handleSoundButton(int curPin) {
  unsigned long currentMillis = millis();
  if ((currentMillis - soundButton.previousMillis) >= soundButton.interval) {
//    bool swTest = testSlideSwitch.read();
    if (testSlideSwitch.read()) { // Operate Mode.
      Wire.beginTransmission(SOUND_I2C_ADDR);
      Wire.write(1); // 1 = Request for Whistle/Horn on Smiths Valley device.
      Wire.endTransmission(); // this is what actually sends the data.
    }
    else { // Test Mode.
      pwm.setPWMFreq(intTestValue);
      //
      Serial.print (F("PWM Fq [Hz]: "));
      Serial.println(intTestValue);
    }
    //
    soundButton.previousMillis = currentMillis;
  }
}
//
// This works for non-negative numbers.
int roundToMultiple(int toRound, int multiple)
{
    return (toRound + (multiple / 2)) / multiple * multiple;
}
//
//
void handleThrottleButtons(int curPin) {
  unsigned long currentMillis = millis();
  //
  for (int i = 0; i < NUMBER_OF_CABS; i ++) {
    for (int j = 0; j < 3; j++) { // 3 = number of buttons.
      if (cabs[i].buttonToPin[j] == curPin) {
        switch (j) {
          case STOP:
            if ((currentMillis - cabs[i].buttonPreviousMillis[STOP]) >= cabs[i].buttonInterval){
              cabs[i].throttle = 0;
              //
              driveMotor(i);
              //
              cabs[i].buttonPreviousMillis[STOP] = currentMillis;
            }
            break;
          case FORWARD:
            if ((currentMillis - cabs[i].buttonPreviousMillis[FORWARD]) >= cabs[i].buttonInterval){
                if (cabs[i].throttle == 0) {
                  cabs[i].throttle = cabs[i].minForward;
                }
                else if ((cabs[i].throttle >= cabs[i].minReverse) && (cabs[i].throttle < 0)) {
                  cabs[i].throttle = 0;
                }
                else {
                  cabs[i].throttle += cabs[i].stepValue;
                }
                //
                driveMotor(i);
                //
                cabs[i].buttonPreviousMillis[FORWARD] = currentMillis;
            }
            break;
          case REVERSE:
            if ((currentMillis - cabs[i].buttonPreviousMillis[REVERSE]) >= cabs[i].buttonInterval){
                if (cabs[i].throttle == 0) {
                  cabs[i].throttle = cabs[i].minReverse;
                }
                else if ((cabs[i].throttle <= cabs[i].minForward) && (cabs[i].throttle > 0)) {
                  cabs[i].throttle = 0;
                }
                else {
                  cabs[i].throttle -= cabs[i].stepValue;
                }
                //
                driveMotor(i);
                //
                cabs[i].buttonPreviousMillis[REVERSE] = currentMillis;
            }
            break;
          default:
            cabs[i].throttle = 0;
            String message = stringAssembler("Pin %p does not exist in handleThrottleButtons.\n", curPin);
            Serial.println(message);
            break;
        }
      }
    }
  }
}
//
//
void dsplyValues(void) {
  if (blnLogoTimedOut) {
    uint8_t dsplyThrottle = 0; // throttle value to display.
    uint8_t rh = 13; // row height.
    uint8_t c1 = 7; // column 1 start, Cab.
    uint8_t c2 = 30; // column 2 start, Throttle. (3 chartacters = 33)
    uint8_t c3 = 75; // column 3 start, Direction.
    uint8_t c4 = 104; // column 4 start, Engine Facing.
    String strTemp = "";
    //
    // picture loop  
    u8g2.firstPage();
    do {
      u8g2_prepareName(); // Upper case font.
      strTemp = MODULE_NAME;
      strTemp.toUpperCase();
      u8g2.setCursor(4,0);
      u8g2.print(strTemp); // first row.
      u8g2_prepareValues();
      u8g2.drawStr(0,rh,"Cab  Thrtl  Dir  Face"); // second row.
      //
      for (int i=0; i < NUMBER_OF_CABS; i ++) {
        u8g2.setCursor(c1,(i+2)*rh);
        u8g2.print(cabs[i].cabName);
        //
        // Use print(u8x8_u8toa(value, digits)) or print(u8x8_u16toa(value, digits)) 
        // to print numbers with constant width (numbers are prefixed with 0 if required).
        u8g2.setCursor(c2,(i+2)*rh);
        // Found that u8x8_u8toa does like 4 characters!!!
        //u8g2.print(u8x8_u8toa(abs(cabs[i].throttle), 4)); // Display four characters for throttle.
        u8g2.print(abs(cabs[i].throttle));
        //
        switch(cabs[i].dir) {
          case STOP:
            strTemp = "X";
            break;
          case FORWARD:
            strTemp = "F";
            break;
          case REVERSE:
            strTemp = "R";
            break;
        }
        u8g2.setCursor(c3,(i+2)*rh);
        u8g2.print(strTemp);
        //
        switch(cabs[i].engineFacing) {
          case STOP:
            strTemp = "X";
            break;
          case EAST:
            strTemp = "E";
            break;
          case WEST:
            strTemp = "W";
            break;
        }
        u8g2.setCursor(c4,(i+2)*rh);
        u8g2.print(strTemp);
      }
      //
//strTemp = stringAssembler("Pin %p does .\n", rh);
      intTestValue = map(analogRead(A1), 0, 1023, 20, 200);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = "PWM Fq [Hz]: ";
      strTemp.concat(intTestValue);
      u8g2.setCursor(0,54);
      u8g2.print(strTemp);
    } while ( u8g2.nextPage() );
  }
}
//
//
void dsplyLogo(void) {
  String strTemp = "";
  // picture loop  
  u8g2.firstPage();  
  do {
    u8g2_prepareName();
    u8g2.drawXBMP(64, 0, LOGO_WIDTH, LOGO_HEIGHT, shsf_logo_bits);
    u8g2.drawStr( 5, 20, "SMITHS");
    u8g2.drawStr( 5, 35, "VALLEY");
  } while( u8g2.nextPage() );
}
//
//
void u8g2_prepareName(void) {
  u8g2.setFont(u8g2_font_pxplustandynewtv_8u);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
//
//
void u8g2_prepareValues(void) {
  u8g2.setFont(u8g2_font_luBS08_tr); // u8g2_font_luBS08_tr,u8g2_font_spleen6x12_mr
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
//
//
void setupButtons(void) {
  soundButton.previousMillis = 0;
  soundButton.interval = 800;
}
//
//
void setupCabs(void) {
  cabs[CAB_A].cabName = "A";
  cabs[CAB_A].throttle = 0;
  cabs[CAB_A].stepValue = 100;
  cabs[CAB_A].minForward = 700;
  cabs[CAB_A].maxForward = 4095;
  cabs[CAB_A].minReverse = -700;
  cabs[CAB_A].maxReverse = -4095;
  cabs[CAB_A].dir = STOP;
  cabs[CAB_A].engineFacing = STOP;
  cabs[CAB_A].buttonToPin[STOP] = STOP_A_PIN;
  cabs[CAB_A].buttonToPin[FORWARD] = FORWARD_A_PIN;
  cabs[CAB_A].buttonToPin[REVERSE] = REVERSE_A_PIN;
  cabs[CAB_A].buttonPreviousMillis[STOP] = 0;
  cabs[CAB_A].buttonPreviousMillis[FORWARD] = 0;
  cabs[CAB_A].buttonPreviousMillis[REVERSE] = 0;
  cabs[CAB_A].buttonInterval = 400;
  cabs[CAB_A].L298ToPin[0] = L298_ENA;
  cabs[CAB_A].L298ToPin[1] = L298_IN1;
  cabs[CAB_A].L298ToPin[2] = L298_IN2;
  cabs[CAB_B].cabName = "B";
  cabs[CAB_B].throttle = 0;
  cabs[CAB_B].stepValue = 1;
  cabs[CAB_B].minForward = 700;
  cabs[CAB_B].maxForward = 4095;
  cabs[CAB_B].minReverse = -700;
  cabs[CAB_B].maxReverse = -4095;
  cabs[CAB_B].dir = STOP;
  cabs[CAB_B].engineFacing = STOP;
  cabs[CAB_B].buttonToPin[STOP] = STOP_B_PIN;
  cabs[CAB_B].buttonToPin[FORWARD] = FORWARD_B_PIN;
  cabs[CAB_B].buttonToPin[REVERSE] = REVERSE_B_PIN;
  cabs[CAB_B].buttonPreviousMillis[STOP] = 0;
  cabs[CAB_B].buttonPreviousMillis[FORWARD] = 0;
  cabs[CAB_B].buttonPreviousMillis[REVERSE] = 0;
  cabs[CAB_B].buttonInterval = 400;
  cabs[CAB_B].L298ToPin[0] = L298_ENB;
  cabs[CAB_B].L298ToPin[1] = L298_IN3;
  cabs[CAB_B].L298ToPin[2] = L298_IN4;
}
