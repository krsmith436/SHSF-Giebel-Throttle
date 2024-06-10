/**
 * RTC_NTPSync
 * 
 * This example shows how to set the RTC (Real Time Clock) on the Portenta C33 / UNO R4 WiFi
 * to the current date and time retrieved from an NTP server on the Internet (pool.ntp.org).
 * Then the current time from the RTC is printed to the Serial port.
 * 
 * Instructions:
 * 1. Download the NTPClient library (https://github.com/arduino-libraries/NTPClient) through the Library Manager
 * 2. Change the WiFi credentials in the arduino_secrets.h file to match your WiFi network.
 * 3. Upload this sketch to Portenta C33 / UNO R4 WiFi.
 * 4. Open the Serial Monitor.
 * 
 * Initial author: Sebastian Romero @sebromero
 * 
 * Find the full UNO R4 WiFi RTC documentation here:
 * https://docs.arduino.cc/tutorials/uno-r4-wifi/rtc
 */
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(F("signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}
//
//
void connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Communication with WiFi module failed!"));
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println(F("Please upgrade the firmware"));
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println(F("Connected to WiFi"));
  printWifiStatus();
}
