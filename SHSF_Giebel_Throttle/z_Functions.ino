// This works for non-negative numbers.
int roundToMultiple(int toRound, int multiple)
{
    return (toRound + (multiple / 2)) / multiple * multiple;
}
//
//
void handleSoundButton(int curPin) {
  unsigned long currentMillis = millis();
  if ((currentMillis - soundButton.previousMillis) >= soundButton.interval) {
    if (!blnTestMode) {
    // Operate Mode.
      Wire.beginTransmission(SOUND_I2C_ADDR);
      Wire.write(1); // 1 = Request for Whistle/Horn on Smiths Valley device.
      Wire.endTransmission(); // this is what actually sends the data.
    }
  else {
    // Test Mode.
    Serial.println(getTestMessageText(true)); // Set the test value.
  }
    //
    soundButton.previousMillis = currentMillis;
  }
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
                cabs[i].buttonPreviousMillis[REVERSE] = currentMillis;
            }
            break;
          default:
            cabs[i].throttle = 0;
            String message = stringAssembler("Pin %p does not exist in handleThrottleButtons.\n", curPin);
            Serial.println(message);
            return;
        }
        // Constrain throttle value to account for max values not being a factor of the step value.
        cabs[i].throttle = constrain(cabs[i].throttle, cabs[i].maxReverse, cabs[i].maxForward);
        //
        // Set direction.
        if (cabs[i].throttle == 0) {cabs[i].dir = STOP;}
        if (cabs[i].throttle > 0) {cabs[i].dir = FORWARD;}
        if (cabs[i].throttle < 0) {cabs[i].dir = REVERSE;}
        //
        driveMotor(i);
      }
    }
  }
}
//
//
void dsplyValues(void) { // This is the main function.
  if (blnLogoTimedOut) {
    uint8_t rh = 13; // row height.
    uint8_t c1 = 7; // column 1 start, Cab.
    uint8_t c2 = 30; // column 2 start, Throttle. (3 chartacters = 33)
    uint8_t c3 = 75; // column 3 start, Direction.
    uint8_t c4 = 104; // column 4 start, Engine Facing.
    uint8_t bh = 19; // bar height, Throttle.
    uint8_t bw = 65; // bar width, Throttle.
    uint8_t bs = 128 - bw; // bar start, Throttle.
    uint8_t iw = 14; // icon width, Direction.
    uint8_t is = bs - iw - 3; // icon start, Direction.
    String strTemp = "";
    //
    // picture loop  
    u8g2.firstPage();
    do {
      u8g2_prepareName(); // Upper case font.
      u8g2.setCursor(4,0);
      u8g2.print("GIEBEL THROTTLE"); // first row.
      u8g2_prepareValues();
      //
      if (!blnTestMode) {
      // Operate Mode.
        //
        // Perform a read action on QR code reader.
        if (!tiny_code_reader_read(&results)) {
          Serial.println(F("No person sensor results found on the i2c bus"));
          strTemp = "No sensor found!";
        }
        else {
          if (results.content_length == 30) {
            strTemp = splitQRcode(CAB_A);
          }
          else {
            strTemp = getPwmFrequency();
          }
        }
        // Bottom line text is added here to be the same font as the title.
        u8g2.setCursor(4,54);
        u8g2.print(strTemp);
        //
        u8g2_prepareCabName();
        //
        for (int i=0; i < NUMBER_OF_CABS; i ++) {
          u8g2.setCursor(0, 4 + rh + i*bh);
          // This will toggle display of engine number and road abbreviation.
          if (blnDsplyEngineNumber) {
            u8g2.print(cabs[i].engineNumber);
          }
          else {
            u8g2.print(cabs[i].roadAbbreviation);
          }
          //
          //Draw speed bar frame.
          u8g2.drawFrame(bs, rh + i*bh, bw, bh);
          //Draw speed bar and direction arrow.
          switch(cabs[i].dir) {
            case STOP:
              u8g2.drawLine(is + iw,rh + i*bh, is,rh + (i+1)*bh);
              u8g2.drawLine(is,rh + i*bh, is + iw,rh + (i+1)*bh);
              break;
            case FORWARD:
              u8g2.drawBox(bs, rh + i*bh, bw*cabs[i].throttle/cabs[i].maxForward, bh);
              u8g2.drawTriangle(is,rh + i*bh, is + iw,rh + i*bh + bh/2, is,rh + (i+1)*bh);
              break;
            case REVERSE:
              u8g2.drawBox(bs, rh + i*bh, bw*cabs[i].throttle/cabs[i].maxReverse, bh);
              u8g2.drawTriangle(is + iw,rh + i*bh, is + iw,rh + (i+1)*bh, is,rh + (i*bh) + (bh/2));
              break;
          }
        }
      }
      else {
      // Test Mode.
        //
        u8g2.drawStr(0,rh,"Cab  Thrtl  Dir  Face"); // second row.
        //
        for (int i=0; i < NUMBER_OF_CABS; i ++) {
          u8g2.setCursor(c1,(i+2)*rh);
          strTemp = (i == CAB_A) ? "A" : "B";
          u8g2.print(strTemp);
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
        // Bottom line text.
        strTemp = getTestMessageText(false);
        u8g2.setCursor(0,54);
        u8g2.print(strTemp);
      }
    } while ( u8g2.nextPage() );
  }
}
//
//
String getPwmFrequency(void) {
  int intFreq = round(pwm.getOscillatorFrequency() / (4096 * pwm.readPrescale())) - 1;
  String strTemp = "PWM [Hz]: ";
  strTemp.concat(intFreq);
  return strTemp;
}
//
//
void driveMotor(int intCab) {
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
      pwm.setPWM(cabs[intCab].L298ToPin[IN1], 0, 4096); // Turns pin fully off.
      pwm.setPWM(cabs[intCab].L298ToPin[IN2], 0, 4096); // Turns pin fully off.
      break;
    case FORWARD:
      pwm.setPWM(cabs[intCab].L298ToPin[IN1], 4096, 0); // Set PWM fully on.
      pwm.setPWM(cabs[intCab].L298ToPin[IN2], 0, 4096); // Set PWM fully off.
      break;
    case REVERSE:
      pwm.setPWM(cabs[intCab].L298ToPin[IN1], 0, 4096); // Turns pin fully off.
      pwm.setPWM(cabs[intCab].L298ToPin[IN2], 4096, 0); // Turns pin fully on.
      break;
  }
  pwm.setPWM(cabs[intCab].L298ToPin[ENABLE], 0, abs(cabs[intCab].throttle)); // Set PWM.
}
//
//
void dsplyLogo(void) {
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
  u8g2.setFont(u8g2_font_luBS08_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
//
//
void u8g2_prepareCabName(void) {
  u8g2.setFont(u8g2_font_spleen8x16_mr);
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
  strcpy(cabs[CAB_A].engineNumber, "   A");
  strcpy(cabs[CAB_A].roadAbbreviation, "   A");
  cabs[CAB_A].throttle = 0;
  cabs[CAB_A].stepValue = 50;
  cabs[CAB_A].minForward = 100;
  cabs[CAB_A].maxForward = 1500;
  cabs[CAB_A].minReverse = -50;
  cabs[CAB_A].maxReverse = -1500;
  cabs[CAB_A].dir = STOP;
  cabs[CAB_A].engineFacing = STOP;
  cabs[CAB_A].buttonToPin[STOP] = stopbuttonA.pinNumber();
  cabs[CAB_A].buttonToPin[FORWARD] = forwardbuttonA.pinNumber();
  cabs[CAB_A].buttonToPin[REVERSE] = reversebuttonA.pinNumber();
  cabs[CAB_A].buttonPreviousMillis[STOP] = 0;
  cabs[CAB_A].buttonPreviousMillis[FORWARD] = 0;
  cabs[CAB_A].buttonPreviousMillis[REVERSE] = 0;
  cabs[CAB_A].buttonInterval = 400;
  cabs[CAB_A].L298ToPin[0] = L298_ENA;
  cabs[CAB_A].L298ToPin[1] = L298_IN1;
  cabs[CAB_A].L298ToPin[2] = L298_IN2;
  strcpy(cabs[CAB_B].engineNumber, "   B");
  strcpy(cabs[CAB_B].roadAbbreviation, "   B");
  cabs[CAB_B].throttle = 0;
  cabs[CAB_B].stepValue = 1;
  cabs[CAB_B].minForward = 700;
  cabs[CAB_B].maxForward = 4095;
  cabs[CAB_B].minReverse = -700;
  cabs[CAB_B].maxReverse = -4095;
  cabs[CAB_B].dir = STOP;
  cabs[CAB_B].engineFacing = STOP;
  cabs[CAB_B].buttonToPin[STOP] = stopbuttonB.pinNumber();
  cabs[CAB_B].buttonToPin[FORWARD] = forwardbuttonB.pinNumber();
  cabs[CAB_B].buttonToPin[REVERSE] = reversebuttonB.pinNumber();
  cabs[CAB_B].buttonPreviousMillis[STOP] = 0;
  cabs[CAB_B].buttonPreviousMillis[FORWARD] = 0;
  cabs[CAB_B].buttonPreviousMillis[REVERSE] = 0;
  cabs[CAB_B].buttonInterval = 400;
  cabs[CAB_B].L298ToPin[0] = L298_ENB;
  cabs[CAB_B].L298ToPin[1] = L298_IN3;
  cabs[CAB_B].L298ToPin[2] = L298_IN4;
}
