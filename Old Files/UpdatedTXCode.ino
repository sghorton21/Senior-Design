#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_DPS310.h>
#include <Wire.h>

RF24 radio(9, 10); // CE, CSN pins for NRF24L01

Adafruit_DPS310 dps;
const byte address[6] = "00001";
float SEA_LEVEL_HPA = 1013.25;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  dps.begin_I2C(0x77, &Wire);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
}

void loop() {
  sensors_event_t TEMP_EVENT, PRESSURE_EVENT;
  dps.getEvents(&TEMP_EVENT, &PRESSURE_EVENT);

  float altitude = dps.readAltitude(SEA_LEVEL_HPA); // Read altitude

  int SPL_ADDRESS = 0x48, DB_REGISTER = 0x0A;
  Wire.beginTransmission(SPL_ADDRESS);
  Wire.write(DB_REGISTER);
  Wire.endTransmission();
  Wire.requestFrom(SPL_ADDRESS, 1);
  byte DB = Wire.read(); // Read SPL value

  // Combine sensor data into a string
  String combinedMessage = "SPL: " + String(DB) + " dB | Altitude: " + String(altitude, 2) + " m";

  radio.write(combinedMessage.c_str(), combinedMessage.length() + 1); // Send the combined message through NRF24L01

  delay(1000); // Adjust delay based on your requirements
}

