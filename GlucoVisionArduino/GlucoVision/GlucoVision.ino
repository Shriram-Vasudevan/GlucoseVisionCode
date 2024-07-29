#include <ArduinoJson.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int irLedPin = 9;
const int photodiodePin = A0;
const float referenceVoltage = 5.0; 

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

const char ssid[] = "Fios-G3n3L";
const char password[] = "gaffe54eeg23cod";

const char serverURL[] = "xpk1u9se04.execute-api.us-east-2.amazonaws.com";
const char path[] = "/beta/getGlucoseLevel";
const char apiKey[] = "kONSJyNNjx3D9J7ZP4sRu10tSsO2JgWf4i7w11H1";

WiFiSSLClient client;

int status = WL_IDLE_STATUS;

void setup() {
  pinMode(irLedPin, OUTPUT);
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000); 
  display.clearDisplay();
  display.setTextSize(1);     
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);  

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  getReading();
}

void loop() {

}

void getReading() {
  delay(5000);

  digitalWrite(irLedPin, HIGH);
  delay(1000);

  int adcValueOn = analogRead(photodiodePin);
  float voltageOn = (adcValueOn / 1023.0) * referenceVoltage;
  Serial.print("LED On - ADC Value: ");
  Serial.print(adcValueOn);
  Serial.print(" | Voltage: ");
  Serial.print(voltageOn);
  Serial.println(" V");

  digitalWrite(irLedPin, LOW);
  delay(1000);

  if (client.connect(serverURL, 443)) {
    Serial.println("Connected to server");

    String jsonBody = "{\"input\": [" + String(voltageOn * 10) + "]}";
    client.println("POST " + String(path) + " HTTP/1.1");
    client.println("Host: " + String(serverURL));
    client.println("Content-Type: application/json");
    client.println("x-api-key: " + String(apiKey));
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(jsonBody.length());
    client.println();
    client.println(jsonBody);
  } else {
    Serial.println("Connection to server failed");
  }

  while (client.connected()) {
    String response = readResponse();
    if (response.length() > 0) {
      Serial.println("Response: ");
      Serial.println(response);
      displayReading(response);
      break;
    }
  }
  client.stop();
}

void displayReading(String reading) {
  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, reading);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* bodyValue = doc["body"];

  Serial.print(bodyValue);

  float number = String(bodyValue).toFloat();
  String numberStr = String(number, 2);

  Serial.print(numberStr);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(numberStr, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  int y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y); 
  display.print(numberStr);
  display.display();
}


String readResponse() {
  String response = "";
  bool seenBrace = false;
  while (client.available()) {
    char c = client.read();
    if (c == '{') {
      seenBrace = true;
    }
    if (seenBrace) {
      response += c;
    }
  }
  return response;
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}



// void loop() {
//   digitalWrite(irLedPin, HIGH);
//   delay(1000);

//   int adcValueOn = analogRead(photodiodePin);
//   float voltageOn = (adcValueOn / 1023.0) * referenceVoltage;
//   Serial.print("LED On - ADC Value: ");
//   Serial.print(adcValueOn);
//   Serial.print(" | Voltage: ");
//   Serial.print(voltageOn);
//   Serial.println(" V");

//   digitalWrite(irLedPin, LOW);
//   delay(1000);

//   int adcValueOff = analogRead(photodiodePin);
//   float voltageOff = (adcValueOff / 1023.0) * referenceVoltage;
//   Serial.print("LED Off - ADC Value: ");
//   Serial.print(adcValueOff);
//   Serial.print(" | Voltage: ");
//   Serial.print(voltageOff);
//   Serial.println(" V");

//   display.clearDisplay();
//   display.setCursor(0, 0); 
//   display.print("LED On - ADC: ");
//   display.print(adcValueOn);
//   display.print(" | V: ");
//   display.println(voltageOn);
//   display.print("LED Off - ADC: ");
//   display.print(adcValueOff);
//   display.print(" | V: ");
//   display.println(voltageOff);
//   display.display();

//   delay(500);
// }