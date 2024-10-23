/*
 * Andrey's super simple CO2 Meter Code
 * NodeMCU + 1306 OLED display + SCD4X co2 sensor
 * All rights reserved.
 *
 *
 * 0-400.    - NORMAL OUTDOOR AIR
 * 400-1000  - NORMAL FOR INDOOR
 * 1000-2000 - Common complaints of drowsiness or poor air quality
 * 2000-5000 - Headaches, fatigue, stagnant, stuffiness, poor concentration, loss of focus, increased heart rate, nausea
 * >50,000 ( >5% )  - Toxicity due to oxygen deprivation occurs
 * >100,000ppm ( >1e0% ) - Oxygen deprivation in seconds: convulsions, coma, and death
 
 */

#include <Arduino.h>
#include <SensirionI2CScd4x.h>
#include <Wire.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

SensirionI2CScd4x scd4x;

//SSD1306Wire display(0x3c, SDA, SCL); // nodemcu (club )
SSD1306Wire display(0x3c, D1, D2); // my sensor- clone esp8266 board with display


const int buzzer = D6; //buzzer to arduino pin 9

void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

void setup() {


    pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    Wire.begin();

    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    uint16_t error;
    char errorMessage[256];

    scd4x.begin(Wire);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        printSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("Waiting for first measurement... (5 sec)");
}


void loop() {
    uint16_t error;
    char errorMessage[256];

    delay(100);

    // Read Measurement
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool isDataReady = false;
    
    error = scd4x.getDataReadyFlag(isDataReady);

    if (error) {
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return;
    }
    if (!isDataReady) {
        return;
    }
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
    } else {
        Serial.print("Co2:");
        Serial.print(co2);
        Serial.print("\t");
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);

        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
       
        String co2string;
        String tempstring;
        String humstring;
        co2string = String(co2);
        tempstring = String(temperature);
        humstring = String(humidity);

        display.drawString(0, 0, "CO2:");
        display.drawString(45,0,co2string);
        display.setFont(ArialMT_Plain_10);
        display.drawString(100,0,"ppm");
        display.setFont(ArialMT_Plain_16);

        

        if(co2 < 5000)
        {
          display.drawString(0, 16, "Humid:");
          display.drawString(55,16,humstring);
          display.drawString(100,16,"%");
          display.drawString(0, 32, "Temp:");
          display.drawString(55, 32, tempstring);
          display.drawString(100,32,"C");
        }else if( co2 >5000 && co2 < 20000){
          display.setFont(ArialMT_Plain_24);
          display.drawString(0,24,"HIGH CO2!");
          tone(buzzer, 1000); // Send 1KHz sound signal...
          delay(2000);         // ...for 1 sec
          noTone(buzzer);     // Stop sound...
          delay(200);         // ...for 1sec
        }else if(co2 > 20000) 
        {

          display.setFont(ArialMT_Plain_24);
          display.drawString(0,24,"FUCK RUN!");
          tone(buzzer, 1000); // Send 1KHz sound signal...
          delay(2000);         // ...for 1 sec
          noTone(buzzer);     // Stop sound...
          delay(200);         // ...for 1sec
        }       


        display.display();

    }
}
