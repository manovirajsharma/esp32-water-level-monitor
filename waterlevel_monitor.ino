#define BLYNK_TEMPLATE_ID "TMPL3ean_kc3Z"
#define BLYNK_TEMPLATE_NAME "water level monitoring system"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <U8g2lib.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define LED1 2
#define LED2 4
#define LED3 5
#define LED4 18
#define trig 12
#define echo 13
#define relay 14

//Enter your tank max value(CM)
int MaxLevel = 13;

int Level1 = (MaxLevel * 75) / 100;
int Level2 = (MaxLevel * 65) / 100;
int Level3 = (MaxLevel * 55) / 100;
int Level4 = (MaxLevel * 35) / 100;

// Initialize the OLED (u8g2) — SH1106 driver, 128x64, hardware I2C
// If your OLED uses an SSD1306 chip instead, swap this line for:
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

BlynkTimer timer;


//Enter your WIFI SSID and password
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// Track current status text so both lines can be redrawn together
String levelStatus = "";
String motorStatus = "Motor is OFF";

void setup() {
  // Debug console
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  u8g2.clearBuffer();
  u8g2.drawStr(0, 30, "System");
  u8g2.drawStr(0, 46, "Loading..");
  u8g2.sendBuffer();
  delay(4000);
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

// Redraws both status lines together — called after either changes
void updateDisplay() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 30, "WLevel:");
  u8g2.drawStr(50, 30, levelStatus.c_str());
  u8g2.drawStr(0, 46, motorStatus.c_str());
  u8g2.sendBuffer();
}

//Get the ultrasonic sensor values
void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  int distance = t / 29 / 2;

  Serial.println(distance);

  int blynkDistance = (distance - MaxLevel) * -1;
  if (distance <= MaxLevel) {
    Blynk.virtualWrite(V0, blynkDistance);
  } else {
    Blynk.virtualWrite(V0, 0);
  }

  if (Level1 <= distance) {
    levelStatus = "Very Low";
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  } else if (Level2 <= distance && Level1 > distance) {
    levelStatus = "Low";
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  } else if (Level3 <= distance && Level2 > distance) {
    levelStatus = "Medium";
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
  } else if (Level4 <= distance && Level3 > distance) {
    levelStatus = "Full";
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
  }

  updateDisplay();
}

//Get the button value
BLYNK_WRITE(V1) {
  bool Relay = param.asInt();
  if (Relay == 1) {
    digitalWrite(relay, LOW);
    motorStatus = "Motor is ON";
  } else {
    digitalWrite(relay, HIGH);
    motorStatus = "Motor is OFF";
  }
  updateDisplay();
}

void loop() {
  ultrasonic();
  Blynk.run(); // Run the Blynk library
  delay(1000); 
}
