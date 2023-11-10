// Edited 11-10-2023 - CC

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_DPS310.h>
#include <Wire.h>
RF24 radio(10, 9); // CE, CSN pins
Adafruit_DPS310 altTx;
TwoWire splTx;

float SPL_DRONE, ALT_DRONE;
const byte address[6] = "00001";
const int DB_REGISTER = 0x0A;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  altTx.begin_I2C(0x77, &Wire);
  altTx.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  altTx.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  splTx.begin();
  pinMode(13, OUTPUT);
}

void loop() {
  float SPL_1 = GetSPL(0x48);
  float SPL_2 = GetSPL(0x47);
  float SPL_3 = GetSPL(0x46);
  float SPL_4 = GetSPL(0x45);
  float SPL_AVG = (SPL_1 + SPL_2 + SPL_3 + SPL_4) / 4;
  GetAltitude();
  TransmitData();
  Serial.print("Altitude: ");
  Serial.print(ALT_DRONE);
  Serial.print(" meters | SPL: ");
  Serial.print(SPL_DRONE);
  Serial.println(" dB");
  Serial.println("Drone Altitude and SPL Value Sent");
}

float GetSPL(int SPL_ADDRESS) {
  splTx.beginTransmission(SPL_ADDRESS);
  splTx.write(DB_REGISTER);
  splTx.endTransmission(false);
  splTx.requestFrom(SPL_ADDRESS, 1);
  SPL_DRONE = splTx.read();
  return SPL_DRONE;
}

float GetAltitude() {
  sensors_event_t TEMP_EVENT, PRESSURE_EVENT;
  altTx.getEvents(&TEMP_EVENT, &PRESSURE_EVENT);
  ALT_DRONE = altTx.readAltitude(1013.25);
  return ALT_DRONE;
}

void TransmitData() {
  String sensorData = "Altitude: " + String(ALT_DRONE, 2) + " meters SPL: " + String(SPL_DRONE);
  // NRF24L01 data transmission
  radio.write(sensorData.c_str(), sensorData.length() + 1);
}
// This code records the altitude and SPL value per second, outputs it to the Serial Monitor, and has an
// output message indicating the message was sent. There are still issues getting accurate values
// for the pressure sensor. The next step is configuring the receving end of the NRF24L01 to collect
// these values and perform calculations for predicted SPL values based on the Reference Altitude and
// the Drone Altitude.
