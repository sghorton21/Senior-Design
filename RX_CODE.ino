#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_DPS310.h>
#include <Wire.h>

RF24 nrf24l01(9, 10); // CE, CSN pins
Adafruit_DPS310 dps310_tx;
TwoWire i2cbus;

float SPL_1, SPL_2, SPL_3, SPL_AVG, ALT_DRONE;
String DATA_SEND;

float GetSPL(int SPL_ADDRESS) {
  i2cbus.beginTransmission(SPL_ADDRESS);
  i2cbus.write(0x0A);
  i2cbus.endTransmission(false);
  i2cbus.requestFrom(SPL_ADDRESS, 1);
  float SPL_READING = i2cbus.read();
  return SPL_READING;
}

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  dps310_tx.getEvents(&temp_event, &pressure_event);
  ALT_DRONE = dps310_tx.readAltitude(1013.25);
  return ALT_DRONE;
}

void SendString() {
  DATA_SEND =  String(ALT_DRONE, 2) + " | " +
               String(SPL_AVG);
  nrf24l01.write(DATA_SEND.c_str(), DATA_SEND.length() + 1);
  // Serial.println(DATA_SEND.length());
  // Serial.println("Drone Altitude and SPL Value Sent");
}


void setup() {
  Serial.begin(9600);
  nrf24l01.begin();
  nrf24l01.setDataRate(RF24_2MBPS);
  nrf24l01.setPALevel(RF24_PA_HIGH);
  nrf24l01.stopListening();
  nrf24l01.openWritingPipe(0xF0F0F0F0F0);
  dps310_tx.begin_I2C(0x77, &Wire);
  dps310_tx.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps310_tx.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  i2cbus.begin();
  // pinMode(13, OUTPUT); // Set pin 13 as output for the LED
}

void loop() {
  SPL_1 = GetSPL(0x45);
  SPL_2 = GetSPL(0x47);
  SPL_3 = GetSPL(0x48);
  SPL_AVG = (SPL_1 + SPL_2 + SPL_3) / 3;
  GetAlt();
  SendString();
  Serial.println(DATA_SEND);
}
