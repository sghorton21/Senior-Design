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
  // Print altitude and SPL values in Serial Monitor
  // Serial.print("Altitude: ");
  // Serial.print(ALT_DRONE);
  // Serial.print(" meters | SPL: ");
  // Serial.print(DB);
  // Serial.println(" dB");

  // Combine sensor data into a string


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
