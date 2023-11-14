// Senior Design Code
// TX Side (on drone)
// Christopher Cortes
// 10-18-2023

// Include libraries
  #include <Adafruit_DPS310.h>
  #include <Wire.h>

// Define objects
  Adafruit_DPS310 altTx;
  TwoWire splTx;

// Define variables
  float DB, ALT_DRONE, SEA_LEVEL_HPA = 1013.25;
  int SPL_ADDRESS = 0X48, DB_REGISTER = 0X0A;
  sensors_event_t TEMP_EVENT, PRESSURE_EVENT;

// Setup function
  void setup() {
    // SPL Init
    Serial.begin(115200);
    splTx.begin();

    // DPS310 init
    altTx.begin_I2C(0x77, &Wire);
    altTx.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    altTx.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  }

void loop() {
  // DPS310
    altTx.getEvents(&TEMP_EVENT, &PRESSURE_EVENT);
    ALT_DRONE = altTx.readAltitude(SEA_LEVEL_HPA);

  // SPL
    splTx.beginTransmission(SPL_ADDRESS);
    splTx.write(DB_REGISTER);
    splTx.endTransmission(false);
    splTx.requestFrom(SPL_ADDRESS, 1);
    byte DB = splTx.read();

  // Print
    Serial.print("Drone elevation = ");
    Serial.print(ALT_DRONE);
    Serial.println(" meters");
    Serial.print("Drone loudness = ");
    Serial.print(DB);
    Serial.println(" SPL");
  
  // Delay
    delay(1000);
