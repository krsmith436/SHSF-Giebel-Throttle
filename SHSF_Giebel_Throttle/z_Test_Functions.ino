//
//
String getTestMessageText(bool setTestValue) {
  String strTemp = "";
  int intTestValue = -1;
  int index = -1;
  //
  // Perform a read action on the I2C address of the sensor to get the
  // current face information detected.
  if (!tiny_code_reader_read(&results)) {
    Serial.println("No person sensor results found on the i2c bus");
    return strTemp;
  }
  //
  switch (intTestNumber) {
    case 0: // Select test to run
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 0, 50);
      strTemp = F("Run Test # ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        intTestNumber = intTestValue;
      }
      break;
    case 1: // Set locomotive parameters
      //
      //*****************************************************************
      // Still need to add logic that once valid data has been read, do
      // not update again until the Sound button is pressed.
      //*****************************************************************
      //
      if (results.content_length == 0) {
        Serial.println(F("No code found"));
        strTemp = "No code found";
      }
      else {
          Serial.print(F("Found '"));
          Serial.print((char*)results.content_bytes);
          Serial.print("'\n");
          //
          strTemp = splitQRcode();
      }
      break;
    case 2: // PWM Frequency
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 20, 200);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = F("PWM Fq [Hz]: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        pwm.setPWMFreq(intTestValue);
      }
      break;
    case 3: // Throttle minimum
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 100, 1000);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = F("Min [Steps]: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        cabs[CAB_A].minForward = intTestValue;
        cabs[CAB_A].minReverse = -intTestValue;
      }
      break;
    case 4: // Throttle maximum
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 2000, 4095);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = F("Max [Steps]: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        cabs[CAB_A].maxForward = intTestValue;
        cabs[CAB_A].maxReverse = -intTestValue;
      }
      break;
    case 5: // Step value
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 0, 500);
      intTestValue = roundToMultiple(intTestValue,5);
      strTemp = F("Step Value: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        cabs[CAB_A].stepValue = intTestValue;
      }
      break;
    default:
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 0, 50);
      strTemp = F("No Test #");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        intTestNumber = intTestValue;
      }
  }
  return strTemp;
}
