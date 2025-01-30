//
//
String getTestMessageText(bool setTestValue) {
  String strTemp = "";
  int intTestValue = -1;
  int index = -1;
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
    case 1: // PWM Frequency
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 20, 200);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = F("PWM Fq [Hz]: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        pwm.setPWMFreq(intTestValue);
      }
      break;
    case 2: // Throttle minimum
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 100, 1000);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = F("Min [Steps]: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        cabs[CAB_A].minForward = intTestValue;
        cabs[CAB_A].minReverse = -intTestValue;
      }
      break;
    case 3: // Throttle maximum
      intTestValue = map(analogRead(TEST_ANALOG_INPUT), 0, 1023, 2000, 4095);
      intTestValue = roundToMultiple(intTestValue,10);
      strTemp = F("Max [Steps]: ");
      strTemp.concat(intTestValue);
      if (setTestValue) {
        cabs[CAB_A].maxForward = intTestValue;
        cabs[CAB_A].maxReverse = -intTestValue;
      }
      break;
    case 4: // Step value
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
