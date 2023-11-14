#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_DPS310.h>
#include <Wire.h>

RF24 radio(10, 9); // CE, CSN pins

Adafruit_DPS310 altTx;
TwoWire splTx;

const byte address[6] = "00001";
float SEA_LEVEL_HPA = 1013.25;
int SPL_ADDRESS = 0x48, DB_REGISTER = 0x0A;

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
  pinMode(13, OUTPUT); // Set pin 13 as output for the LED
}

void loop() {
  sensors_event_t TEMP_EVENT, PRESSURE_EVENT;

  altTx.getEvents(&TEMP_EVENT, &PRESSURE_EVENT);
  float ALT_DRONE = altTx.readAltitude(SEA_LEVEL_HPA);

  splTx.beginTransmission(SPL_ADDRESS);
  splTx.write(DB_REGISTER);
  splTx.endTransmission(false);
  splTx.requestFrom(SPL_ADDRESS, 1);
  byte DB = splTx.read();

  // Print altitude and SPL values in Serial Monitor
  Serial.print("Altitude: ");
  Serial.print(ALT_DRONE);
  Serial.print(" meters | SPL: ");
  Serial.print(DB);
  Serial.println(" dB");

  // Combine sensor data into a string
  String sensorData = "Altitude: " + String(ALT_DRONE, 2) + " meters SPL: " + String(DB);

  // NRF24L01 data transmission
  radio.write(sensorData.c_str(), sensorData.length() + 1);
  Serial.println("Drone Altitude and SPL Value Sent");

  // Turn on the LED to indicate message transmission
  digitalWrite(13, HIGH);
  delay(100); // Optional delay for LED indication
  digitalWrite(13, LOW);

  delay(1000);
}

// This code records the altitude and SPL value per second, outputs it to the Serial Monitor, and has an
// output message indicating the message was sent. There are still issues getting accurate values
// for the pressure sensor. The next step is configuring the receving end of the NRF24L01 to collect
// these values and perform calculations for predicted SPL values based on the Reference Altitude and
// the Drone Altitude.
