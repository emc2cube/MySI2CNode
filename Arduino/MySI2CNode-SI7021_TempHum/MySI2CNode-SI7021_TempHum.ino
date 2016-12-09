/**
   The MySensors Arduino library handles the wireless radio link and protocol
   between your home built sensors/actuators and HA controller of choice.
   The sensors forms a self healing radio network with optional repeaters. Each
   repeater and gateway builds a routing tables in EEPROM which keeps track of the
   network topology allowing messages to be routed to nodes.

   Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
   Copyright (C) 2013-2015 Sensnology AB
   Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors

   Documentation: http://www.mysensors.org
   Support Forum: http://forum.mysensors.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   REVISION HISTORY
   Version 1.0 - Henrik EKblad

   DESCRIPTION
   This sketch provides an example how to implement a humidity/temperature
   sensor using a Si7021 I2C module
   http://www.mysensors.org/build/humidity
*/

// Customize your device name.
String Location = "Plants";     // Location of the device, will be sent to the gateway with each device

// Enable debug prints
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

// Wait a few ms between radio Tx
#define MESSAGEWAIT 500

#define MY_NODE_ID AUTO
//#define MY_PARENT_NODE_ID 0

#define BATTERY_MONITORING                  // Enable battery monitoring

// possible values: RF24_PA_LOW (is default on gateway), RF24_PA_MED, RF24_PA_HIGH, RF24_PA_MAX (is default on nodes)
//#define MY_RF24_PA_LEVEL RF24_PA_LOW      // Enable to save battery if sensor not too far from gateway. Probably not worth it.

// RF channel for the sensor net, 0-127. Channel 76 is the default in all library examples
//#define MY_RF24_CHANNEL     76

//RF24_250KBPS for 250kbs (default), RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
//#define MY_RF24_DATARATE      RF24_250KBPS

// Set this to true if you want to send values altough the values did not change.
// This is only recommended when not running on batteries.
const bool SEND_ALWAYS = false;

#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>
#include "i2c_SI7021.h"
SI7021 si7021;

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1

const unsigned long SLEEP_TIME = 1800000UL; // Sleep time between reads (in milliseconds), 30min
#if defined MY_DEBUG
SLEEP_TIME = 15000UL; // If debug, TX every 15s
#endif
float lastTemp, lastHum;
boolean metric = true;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

#if defined BATTERY_MONITORING
int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;
#endif // end BATTERY_MONITORING

void setup() {
  if (!si7021.initialize()) {
    Serial.println("Can't find SI7021 sensor!");
    while (1) {};
  }
  // use the 1.1 V internal reference
#if defined(__AVR_ATmega2560__)
  analogReference(INTERNAL1V1);
#else
  analogReference(INTERNAL);
#endif
  metric = getConfig().isMetric;
  Serial.println("Mobile Temperature and Humidity sensor node ready.");
}

void presentation()
{
  // Send the Sketch Version Information to the Gateway
  sleep(MESSAGEWAIT);
  sendSketchInfo("Temperature Humidity node", "2.0");

  // Register all sensors to gw (they will be created as child devices)
  String humReg = Location + " Hum";
  sleep(MESSAGEWAIT);
  present(CHILD_ID_HUM, S_HUM, humReg.c_str());
  String tempReg = Location + " Temp";
  sleep(MESSAGEWAIT);
  present(CHILD_ID_TEMP, S_TEMP, tempReg.c_str());
}

void loop()
{
  // Fetch temperature from sensor
  float temperature;
  si7021.getTemperature(temperature);
  temperature = round(temperature * 10) / 10.0; // 0.1°C accuracy is sensistive enough
  if (SEND_ALWAYS || temperature != lastTemp) {
    lastTemp = temperature;
    if (!metric) {
      temperature = temperature * 9.0 / 5.0 + 32.0;
    }
    wait(MESSAGEWAIT);
    send(msgTemp.set(temperature, 1));
#if defined MY_DEBUG
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(metric ? F(" °C") : F(" °F"));
#endif
  }

  // Fetch humidity from sensor
  float humidity;
  si7021.getHumidity(humidity);
  humidity = round(humidity); // 1% RH accuracy is sensistive enough
  // Clip humidity values to a valid range
  // If sent RH > 100% domoticz will ignore both temperature and humidity readings
  humidity = humidity > 100 ? 100 : humidity;
  humidity = humidity < 0   ? 0   : humidity;
  if (SEND_ALWAYS || humidity != lastHum) {
    lastHum = humidity;
    wait(MESSAGEWAIT);
    send(msgHum.set(humidity, 0));
#if defined MY_DEBUG
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F(" %"));
#endif
  }

#if defined BATTERY_MONITORING
  // get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
#ifdef MY_DEBUG
  Serial.print(F("Battery voltage ADC: "));
  Serial.println(sensorValue);
#endif
  // 1M, 470K divider across battery and using internal ADC ref of 1.1V
  // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
  // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
  // 3.44/1023 = Volts per bit = 0.003363075
  int batteryPcnt = sensorValue / 10;
#ifdef MY_DEBUG
  float batteryV  = sensorValue * 0.003363075;
  Serial.print("Battery Voltage: ");
  Serial.print(batteryV);
  Serial.println(" V");
  Serial.print("Battery percent: ");
  Serial.print(batteryPcnt);
  Serial.println(" %");
#endif
  if (abs(batteryPcnt - oldBatteryPcnt) <= 1) {
    batteryPcnt = oldBatteryPcnt;
  }
  if (oldBatteryPcnt != batteryPcnt) {
    wait(MESSAGEWAIT);
    sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  }
#endif // end BATTERY_MONITORING

  sleep(SLEEP_TIME); //sleep a bit

}
