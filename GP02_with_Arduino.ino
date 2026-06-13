// GPS Tracker - Arduino Uno
// Uses SoftwareSerial for GPS (no hardware Serial1 on Uno)

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>          // Required for GPS UART on Uno
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

TinyGPSPlus gps;

// SoftwareSerial on D2 (RX from GPS TX) and D3 (TX to GPS RX)
#define RX_PIN 2
#define TX_PIN 3
SoftwareSerial gpsSerial(RX_PIN, TX_PIN);

// Helper: print 2-digit zero-padded number
void printPadded2(int val) {
  if (val < 10) display.print("0");
  display.print(val);
}

// Helper: print 4-digit zero-padded number
void printPadded4(int val) {
  if (val < 1000) display.print("0");
  if (val < 100)  display.print("0");
  if (val < 10)   display.print("0");
  display.print(val);
}

void setup() {
  gpsSerial.begin(9600);            // SoftwareSerial for GPS

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  delay(10);
  display.setCursor(0, 0);
  display.println("Waiting for GPS...");
  display.display();
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      displayLocationInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No GPS detected!");
    display.display();
    while (true);
  }
}

void displayLocationInfo() {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Lat: ");
  if (gps.location.isValid()) display.println(gps.location.lat(), 6);
  else display.println("Invalid");

  display.print("Lng: ");
  if (gps.location.isValid()) display.println(gps.location.lng(), 6);
  else display.println("Invalid");

  display.print("Sats: ");
  display.println(gps.satellites.value());

  display.print("Alt: ");
  display.print(gps.altitude.meters());
  display.println(" m");

  display.print("Spd: ");
  display.print(gps.speed.kmph());
  display.println(" km/h");

  display.print("Course: ");
  display.print(gps.course.deg());
  display.write(247);
  display.println();

  display.print("UTC: ");
  if (gps.time.isValid()) {
    printPadded2(gps.time.hour());   display.print(":");
    printPadded2(gps.time.minute()); display.print(":");
    printPadded2(gps.time.second()); display.println();
  } else display.println("Invalid");

  display.print("Date: ");
  if (gps.date.isValid()) {
    printPadded2(gps.date.day());   display.print("/");
    printPadded2(gps.date.month()); display.print("/");
    printPadded4(gps.date.year());  display.println();
  } else display.println("Invalid");

  display.display();
}
