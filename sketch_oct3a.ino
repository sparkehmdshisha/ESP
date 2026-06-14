#include <esp_task_wdt.h>
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>    // Grafik-Bibliothek
#include <Adafruit_ILI9341.h> 

//3 seconds WDT
#define WDT_TIMEOUT 3


// CS-Pins definieren
#define TFT_CS_PIN 15     // Chip Select für das TFT-Display (HSPI)
#define MAX31855_CS1_PIN 5 // Chip Select für den ersten MAX31855 (VSPI)
#define MAX31855_CS2_PIN 17 // Chip Select für den zweiten MAX31855 (VSPI)
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

#define RelayPin1 23
#define RelayPin2 34

// MAX31855 Sensor Objekte erstellen
Adafruit_MAX31855 thermocouple1(MAX31855_CS1_PIN);
Adafruit_MAX31855 thermocouple2(MAX31855_CS2_PIN);

//tft Objekt erstellen
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

//const int
const int Temp1 = 150;
const int Temp2 = 300;

double tempC1;
double tempC2;

enum MenuState {
    MAIN_MENU,
    DEVICE_CONTROL,
    SETTINGS,
  };

  // Aktueller Menü-Zustand
MenuState currentMenu = MAIN_MENU;


void setup() {
  Serial.begin(115200);
  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);
   //add current thread to WDT watch
  pinMode(TFT_CS_PIN, OUTPUT);
  pinMode(MAX31855_CS1_PIN, OUTPUT);
  pinMode(MAX31855_CS2_PIN, OUTPUT);

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);

  // Relais zu Beginn ausschalten
  digitalWrite(relay1Pin, LOW); // Relais 1 aus
  digitalWrite(relay2Pin, LOW); // Relais 2 aus

  //tft Display
  tft.begin();
  tft.setRotation(3); // Rotation des Displays (je nach Ausrichtung)
  drawIntro();
    
  

  if (!thermocouple1.begin()) {
    Serial.println("Sensor 1 konnte nicht initialisiert werden!");
    while (1);
  }

  if (!thermocouple2.begin()) {
    Serial.println("Sensor 2 konnte nicht initialisiert werden!");
    while (1);
  }
  Serial.println("Sensoren sind bereit.");


  

}


void loop() {
  // resetting WDT every 2s, 5 times only
  if (millis() - last >= 2000 && i < 5) {
      Serial.println("Resetting WDT...");
      esp_task_wdt_reset();
      last = millis();
      i++;
      if (i == 5) {
        Serial.println("Stopping WDT reset. CPU should reboot in 3s");
      }
  }
  else{
    float currentTemp = readMax31855_1();
  }

  delay(1000)
}




void checktemp(float temp1, float temp2){
     // Temperatur von Sensor 1 auslesen
  tempC1 = thermocouple1.readCelsius();
  
  // Temperatur von Sensor 2 auslesen
  tempC2 = thermocouple2.readCelsius();
  

  // Überprüfen ob eine Fehlermeldung aufgetreten ist
  if (isnan(tempC1)) {
    Serial.println("Fehler beim Auslesen von Sensor 1");} 
    else {
      Serial.print("Sensor 1: ");
      Serial.print(tempC1);
        Serial.println(" °C");

      if (isnan(tempC2)) {
      Serial.println("Fehler beim Auslesen von Sensor 2");} 
      else {
        Serial.print("Sensor 2: ");
        Serial.print(tempC2);
        Serial.println(" °C");
        if (tempC1 > Temp1){
            digitalWrite(RelayPin1, HIGH);}
        else{
            digitalWrite(RelayPin1, LOW);}
        if (tempC2 > Temp2){
            digitalWrite(RalayPin2,High)}
        else{
            digitalWrite(RalayPin2,LOW)
        }
      }
    }

  }

void drawIntro(){

  }




}
