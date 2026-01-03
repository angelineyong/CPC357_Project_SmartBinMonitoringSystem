#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include "DHT.h"

/* ===================== WIFI CONFIG ===================== */
// Replace with your own WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

/* ===================== INFLUXDB CONFIG ===================== */
// Replace with your own InfluxDB details
const char* INFLUX_URL    = "https://your-region.aws.cloud2.influxdata.com";
const char* INFLUX_ORG    = "YOUR_ORG_NAME";
const char* INFLUX_BUCKET = "YOUR_BUCKET_NAME";
const char* INFLUX_TOKEN  = "YOUR_INFLUXDB_TOKEN";

/* ===================== PIN DEFINITIONS ===================== */
#define PIR_PIN     27
#define SERVO_PIN   14
#define MQ135_AO    34

#define RED_LED     16
#define YELLOW_LED  17
#define GREEN_LED   18

// Ultrasonic Sensor (powered by 3.3V)
#define US_TRIG     25
#define US_ECHO     26

// DHT11
#define DHTPIN      4
#define DHTTYPE     DHT11

/* ===================== OBJECTS ===================== */
Servo myServo;
DHT dht(DHTPIN, DHTTYPE);

/* ===================== SERVO CONTROL ===================== */
bool servoActivated = false;
unsigned long servoStartTime = 0;
const unsigned long servoHoldTime = 2000; // Servo stays open for 2 seconds

/* ===================== BIN CALIBRATION ===================== */
const float BIN_DEPTH_CM = 30.0;     // Distance from sensor to bin bottom
const float EMPTY_OFFSET_CM = 2.0;   // Sensor mounting offset

/* ===================== GLOBAL VARIABLES ===================== */
int fillPercent = 0;
float temperature = 0;
int gasValue = 0;
int motion = 0;

/* ===================== WIFI CONNECTION ===================== */
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

/* ===================== INFLUXDB SEND FUNCTION ===================== */
void sendToInflux(float temp, int gas, int fill, int motionState) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure secureClient;
  secureClient.setInsecure();

  HTTPClient http;

  String url = String(INFLUX_URL) +
               "/api/v2/write?org=" + INFLUX_ORG +
               "&bucket=" + INFLUX_BUCKET +
               "&precision=s";

  // Line protocol format
  String data = "smartbin ";
  data += "temperature=" + String(temp) + ",";
  data += "gas=" + String(gas) + ",";
  data += "fill=" + String(fill) + ",";
  data += "motion=" + String(motionState);

  http.begin(secureClient, url);
  http.addHeader("Authorization", "Token " + String(INFLUX_TOKEN));
  http.addHeader("Content-Type", "text/plain");

  int responseCode = http.POST(data);
  Serial.print("InfluxDB response code: ");
  Serial.println(responseCode);

  http.end();
}

/* ===================== ULTRASONIC FUNCTION ===================== */
float readDistanceCm() {
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);

  long duration = pulseIn(US_ECHO, HIGH, 30000);
  if (duration == 0) return -1;

  return (duration * 0.0343) / 2.0;
}

/* ===================== LED CONTROL ===================== */
void setTrashLEDs(int percent) {
  if (percent <= 33) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } 
  else if (percent <= 66) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } 
  else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

/* ===================== SETUP ===================== */
void setup() {
  Serial.begin(9600);
  delay(1000);

  connectWiFi();

  pinMode(PIR_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0); // Lid closed

  dht.begin();

  Serial.println("Smart Bin System Started");
}

/* ===================== LOOP ===================== */
void loop() {

  // --- MQ135 Gas Sensor ---
  gasValue = analogRead(MQ135_AO);

  // --- Temperature Sensor ---
  temperature = dht.readTemperature();

  // --- Ultrasonic Trash Level ---
  float distance = readDistanceCm();
  if (distance > 0) {
    float effectiveDepth = BIN_DEPTH_CM - EMPTY_OFFSET_CM;
    float fill = (effectiveDepth - (distance - EMPTY_OFFSET_CM)) / effectiveDepth;
    fill = constrain(fill, 0, 1);
    fillPercent = (int)(fill * 100);
    setTrashLEDs(fillPercent);
  }

  // --- PIR Sensor + Servo ---
  motion = digitalRead(PIR_PIN);
  if (motion == HIGH && !servoActivated) {
    myServo.write(90);  // Open lid
    servoActivated = true;
    servoStartTime = millis();
  }

  if (servoActivated && millis() - servoStartTime >= servoHoldTime) {
    myServo.write(0);   // Close lid
    servoActivated = false;
  }

  // --- Send Data to InfluxDB ---
  sendToInflux(temperature, gasValue, fillPercent, motion);

  delay(1000);
}
