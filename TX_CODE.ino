// Updated 11-14-2023 1:16pm

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_DPS310.h>
#include <Wire.h>

RF24 radio(10, 9); // CE, CSN pins

Adafruit_DPS310 DPS310_TX;
TwoWire SPL_MASTER;

const byte address[6] = "00001";
float SPL_READING, SPL_1, SPL_2, SPL_3, SPL_AVG, ALT_READING;

float GetSPL(int SPL_ADDRESS) {
  SPL_MASTER.beginTransmission(SPL_ADDRESS);
  SPL_MASTER.write(0x0A);
  SPL_MASTER.endTransmission(false);
  SPL_MASTER.requestFrom(SPL_ADDRESS, 1);
  SPL_READING = SPL_MASTER.read();
  return SPL_READING;
}

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  DPS310_TX.getEvents(&temp_event, &pressure_event);
  ALT_READING = DPS310_TX.readAltitude(1013.25);
  return ALT_READING;
}

void SendString() {
  String DATA_SEND = "Altitude: " + String(ALT_READING, 2) + " meters SPL: " + String(SPL_AVG);
  radio.write(DATA_SEND.c_str(), DATA_SEND.length() + 1);
  Serial.println("Drone Altitude and SPL Value Sent");
}

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  DPS310_TX.begin_I2C(0x77, &Wire);
  DPS310_TX.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  DPS310_TX.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  SPL_MASTER.begin();
  pinMode(13, OUTPUT); // Set pin 13 as output for the LED
}

void loop() {
  SPL_1 = GetSPL(0x45);
  SPL_2 = GetSPL(0x47);
  SPL_3 = GetSPL(0x48);
  GetAlt();
}
